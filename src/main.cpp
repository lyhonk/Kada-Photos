/*
引脚接法

DS3231模块
SDA -> 8
SCL -> 9

墨水屏驱动板(epdif.h)
BUSY_PIN  -> 7
RST_PIN   -> 6
DC_PIN    -> 5
CS_PIN    -> 4
HSPI_SCLK -> 12
HSPI_COPI -> 13

*/

#include <Arduino.h>
#include <SPI.h>
#include "epd7in3f.h"
#include "LittleFS.h"

#define useLittleFS
#include <BGWiFiConfig.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <RTClib.h>    // 使用 RTClib 库来操作 DS3231
#include <esp_sleep.h> // ESP32 睡眠管理库

#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP客户端设置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com", 8 * 3600, 60000); // UTC+8时区，每60秒更新一次

// DS3231 的中断引脚，与ESP32S3的GPIO10相连
#define DS3231_INTERRUPT_PIN 10

RTC_DS3231 rtc;

// BGWiFiConfig库 https://www.cbug.top/1app/bgwificonfig/#/
BGWiFiConfig wifipw;
void setRTCTimeFromNTP()
{
  Serial.println("正在获取网络时间...");

  timeClient.begin();

  if (timeClient.forceUpdate())
  {
    unsigned long epochTime = timeClient.getEpochTime();

    // 将时间设置到RTC
    rtc.adjust(DateTime(epochTime));

    DateTime now = rtc.now();
    Serial.printf("RTC时间已更新为: %04d-%02d-%02d %02d:%02d:%02d\n",
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());
  }
  else
  {
    Serial.println("获取网络时间失败!");
  }

  timeClient.end();
}

// 图片 URL 和本地保存路径
const char *imageUrl = "http://statics.biogeeker.com/demo.bmp";
const char *filePath = "/display.bmp";
const char *infoFilePath = "/demo.bmp";
bool downloadImage()
{
  if (!LittleFS.begin(true))
  { // 初始化 LittleFS 用于临时存储下载的图片
    Serial.println("LittleFS 挂载失败");
    return false;
  }

  WiFiClient client;
  HTTPClient http;

  Serial.print("正在下载图片：");
  Serial.println(imageUrl);

  http.begin(client, imageUrl);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK)
  {
    Serial.printf("HTTP request failed with code %d\n", httpCode);
    http.end();
    return false;
  }

  fs::File file = LittleFS.open(filePath, "w");
  if (!file)
  {
    Serial.println("打开文件失败！");
    http.end();
    return false;
  }

  WiFiClient *response = http.getStreamPtr();
  uint8_t buffer[1024];
  size_t len;

  while ((len = response->readBytes(buffer, sizeof(buffer))) > 0)
  {
    file.write(buffer, len);
  }

  file.close();
  http.end();

  Serial.println("图片已下载并保存！");
  return true;
}

Epd epd;

uint16_t width() { return EPD_WIDTH; }
uint16_t height() { return EPD_HEIGHT; }
uint16_t read16(fs::File &f)
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f)
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

bool drawBmp(const char *filename)
{
  fs::File bmpFS;

  // Open requested file on LittleFS
  bmpFS = LittleFS.open(filename, "r");
  Serial.println("读取图片中。。。。");

  uint32_t seekOffset, headerSize, paletteSize = 0;
  int16_t row;
  uint8_t r, g, b;
  uint16_t bitDepth;

  uint16_t magic = read16(bmpFS);
  if (magic != ('B' | ('M' << 8)))
  { // File not found or not a BMP
    Serial.println(F("BMP not found!"));
    bmpFS.close();
    return false;
  }

  read32(bmpFS);              // filesize in bytes
  read32(bmpFS);              // reserved
  seekOffset = read32(bmpFS); // start of bitmap
  headerSize = read32(bmpFS); // header size
  uint32_t w = read32(bmpFS); // width
  uint32_t h = read32(bmpFS); // height
  read16(bmpFS);              // color planes (must be 1)
  bitDepth = read16(bmpFS);
  if (read32(bmpFS) != 0 || (bitDepth != 24 && bitDepth != 1 && bitDepth != 4 && bitDepth != 8))
  {
    Serial.println(F("BMP format not recognized."));
    bmpFS.close();
    return false;
  }

  uint32_t palette[256];
  if (bitDepth <= 8) // 1,4,8 bit bitmap: read color palette
  {
    read32(bmpFS);
    read32(bmpFS);
    read32(bmpFS); // size, w resolution, h resolution
    paletteSize = read32(bmpFS);
    if (paletteSize == 0)
      paletteSize = bitDepth * bitDepth; // if 0, size is 2^bitDepth
    bmpFS.seek(14 + headerSize);         // start of color palette
    for (uint16_t i = 0; i < paletteSize; i++)
    {
      palette[i] = read32(bmpFS);
    }
  }

  // draw img that is shorter than 240pix into the center
  uint16_t x = (width() - w) / 2;
  uint16_t y = (height() - h) / 2;

  bmpFS.seek(seekOffset);

  uint32_t lineSize = ((bitDepth * w + 31) >> 5) * 4;
  uint8_t lineBuffer[lineSize];

  epd.SendCommand(0x10); // start data frame

  epd.EPD_7IN3F_Draw_Blank(y, width(), EPD_7IN3F_WHITE); // fill area on top of pic white

  // row is decremented as the BMP image is drawn bottom up
  for (row = h - 1; row >= 0; row--)
  {
    epd.EPD_7IN3F_Draw_Blank(1, x, EPD_7IN3F_WHITE); // fill area on the left of pic white
    bmpFS.read(lineBuffer, sizeof(lineBuffer));
    uint8_t *bptr = lineBuffer;

    uint8_t output = 0;
    // Convert 24 to 16 bit colors while copying to output buffer.
    for (uint16_t col = 0; col < w; col++)
    {
      if (bitDepth == 24)
      {
        b = *bptr++;
        g = *bptr++;
        r = *bptr++;
      }
      else
      {
        uint32_t c = 0;
        if (bitDepth == 8)
        {
          c = palette[*bptr++];
        }
        else if (bitDepth == 4)
        {
          c = palette[(*bptr >> ((col & 0x01) ? 0 : 4)) & 0x0F];
          if (col & 0x01)
            bptr++;
        }
        else
        { // bitDepth == 1
          c = palette[(*bptr >> (7 - (col & 0x07))) & 0x01];
          if ((col & 0x07) == 0x07)
            bptr++;
        }
        b = c;
        g = c >> 8;
        r = c >> 16;
      }
      uint8_t color = EPD_7IN3F_WHITE;
      if (r <= 128 && g <= 128 && b <= 128)
      {
        color = EPD_7IN3F_BLACK;
      }
      else if (r > 200 && g > 200 && b > 200)
      {
        color = EPD_7IN3F_WHITE;
      }
      else if (b > 128)
      {
        color = EPD_7IN3F_BLUE;
      }
      else if (g > 128 && r <= 128)
      {
        color = EPD_7IN3F_GREEN;
      }
      else
      {
        color = (g > 140) ? EPD_7IN3F_YELLOW : (g > 64) ? EPD_7IN3F_ORANGE
                                                        : EPD_7IN3F_RED;
      }
      uint32_t buf_location = (row * (width() / 2) + col / 2);
      if (col & 0x01)
      {
        output |= color;
        epd.SendData(output);
      }
      else
      {
        output = color << 4;
      }
    }
    epd.EPD_7IN3F_Draw_Blank(1, x, EPD_7IN3F_WHITE); // fill area on the right of pic white
  }

  epd.EPD_7IN3F_Draw_Blank(y, width(), EPD_7IN3F_WHITE); // fill area below the pic white

  bmpFS.close();
  epd.TurnOnDisplay();
  return true;
}

void displayImage()
{
  if (epd.Init() != 0)
  {
    Serial.print("墨水屏启动失败");
    return;
  }
  if (!downloadImage())
  {
    Serial.println("图片下载失败");
  }
  else
  {
    Serial.print("图片正在生成\r\n ");
    drawBmp(filePath);
    Serial.print("图片显示完成\r\n ");
    epd.Sleep();
  }
}

void displayWiFIinfo()
{
  if (epd.Init() != 0)
  {
    Serial.print("墨水屏启动失败");
    return;
  }
  if (!LittleFS.exists("/bgwificonfig/wifiset.txt"))
  {
    Serial.println("未发现网络配置文件！");
  }
  drawBmp(infoFilePath); 
  Serial.print("Done\r\n ");
  epd.Sleep();
}

void printDetailedRTCInfo()
{
  DateTime now = rtc.now();

  Serial.println("=================== RTC 信息 ===================");
  Serial.printf("日期时间: %04d年%02d月%02d日 %02d:%02d:%02d\n",
                now.year(), now.month(), now.day(),
                now.hour(), now.minute(), now.second());

  const char *weekdays[] = {"日", "一", "二", "三", "四", "五", "六"};
  Serial.printf("星期: %s\n", weekdays[now.dayOfTheWeek()]);

  Serial.printf("时间戳: %lu\n", now.unixtime());
  Serial.println("================================================");
}

void restpasswd()
{
  Serial.println("开始清除配网信息");
  wifipw.clearWiFi();
  Serial.println("配网信息清除成功");
}

void setup()
{

  // put your setup code here, to run once:
  pinMode(48, OUTPUT);
  digitalWrite(48, LOW);

  Serial.begin(115200);
  delay(1000);

  if (!rtc.begin())
  {
    Serial.println("未找到 DS3231 RTC 模块!");
    Serial.flush();
    abort();
  }
  else
  {
    Serial.println("DS3231 RTC 模块已启动");
  }

  if (!LittleFS.begin())
  {
    Serial.println("LittleFS 启动失败");
    Serial.flush();
    abort();
  }
  else
  {
    Serial.println("LittleFS 已启动");
    delay(1000);
  }

  if (!LittleFS.exists("/bgwificonfig/wifiset.txt"))
  {
    Serial.println("######");
    Serial.println("展示配网方法");
    displayWiFIinfo();
    Serial.println("######");
  }

  // 生成名为“咔哒相册”,密码为空的配网WiFi。
  wifipw.setPWWiFi("咔哒相册", "");

  // setConFailReset函数：
  // 可选函数，功能为在指定次数及重连超时时间内，WiFi连接失败，可自动重置配网。
  // 形参：
  // 1.xReConNum:重连次数(总次数不记第1次，如设置为3，将总计连接4次)。
  // 2.xReConTime:超时时间(单位:秒)。
  // 3.xIsReboot:重置配网后是否自动重启进入配网程序(为true开启，为false则需提示重启时，手动重启后自动进入配网程序)。
  wifipw.setConFailReset(2, 10, true);

  // useSpaceWiFi()函数：
  // 可选函数，功能为传入true时，配置的WiFi名称和密码将支持空格。
  wifipw.useSpaceWiFi(true);

  // autoStart()函数：
  //  可选函数，功能为传入true时，开发板在写入配网信息后，将自动重启并连接配置好的WiFi，无需手动复位重启。
  wifipw.autoStart(true);

  // setCompatibleMode()函数：
  // 优化解决部分手机配网WiFi自动断连问题，传入true时启用兼容模式，配网IP将从192.168.22.22变为192.168.22.1。
  wifipw.setCompatibleMode(true);

  wifipw.begin();

  if (wifipw.OK())
  {
    // 同步网络时间并打印
    setRTCTimeFromNTP();
    printDetailedRTCInfo();

    // 清除所有闹钟，确保上次的闹钟不会影响本次
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    // 设置一个固定的闹钟时间：凌晨 00:00:00
    // TODO：联网后获取触发时间，并设置闹钟
    DateTime midnight(2000, 1, 1, 0, 0, 0); // 年月日是占位符，不影响闹钟触发

    // 设置闹钟1，匹配时、分、秒后触发
    // 这样可以确保闹钟在每天的 00:00:00 准时触发
    rtc.setAlarm1(midnight, DS3231_A1_Hour);

    // 打印闹钟时间，便于确认
    Serial.printf("闹钟已设置，将在每天 %02d:%02d:%02d 唤醒\n",
                  midnight.hour(), midnight.minute(), midnight.second());

    // 显示图片
    displayImage();

    // 配置外部唤醒源
    esp_sleep_enable_ext1_wakeup(1ULL << DS3231_INTERRUPT_PIN, ESP_EXT1_WAKEUP_ANY_LOW);

    Serial.println("进入深度睡眠...");
    Serial.flush();
    // 进入深度睡眠模式
    esp_deep_sleep_start();
  };
}

void loop()
{
  wifipw.Loop();
}
