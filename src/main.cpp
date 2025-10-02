/*
引脚接法

DS3231模块
SDA -> 8
SCL -> 9
SQW -> 10

墨水屏驱动板(epdif.h)
BUSY_PIN  -> 7
RST_PIN   -> 6
DC_PIN    -> 5
CS_PIN    -> 4
HSPI_SCLK -> 12
HSPI_COPI -> 13

按键接法
左 (切换模式)     -> 2
中 (重置系统)     -> 3
右 (立即刷新图片) -> 11
*/

#include <Arduino.h>
#include <SPI.h>
#include "epd7in3f.h"
#include "LittleFS.h"
#include "driver/rtc_io.h"

#define useLittleFS
#include <BGWiFiConfig.h>

#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <RTClib.h>    // 使用 RTClib 库来操作 DS3231
#include <esp_sleep.h> // ESP32 睡眠管理库

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ArduinoJson.h>

#include <Preferences.h> // 使用Preferences库来保存数据
Preferences preferences;

// NTP客户端设置
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com", 8 * 3600, 60000); // UTC+8时区，每60秒更新一次

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO) // 2 ^ GPIO_NUMBER in hex
#define WAKEUP_GPIO_2 GPIO_NUM_2
#define WAKEUP_GPIO_3 GPIO_NUM_3
#define WAKEUP_GPIO_10 GPIO_NUM_10
#define WAKEUP_GPIO_11 GPIO_NUM_11

// Define bitmask for multiple GPIOs
uint64_t bitmask = BUTTON_PIN_BITMASK(WAKEUP_GPIO_2) | BUTTON_PIN_BITMASK(WAKEUP_GPIO_3) | BUTTON_PIN_BITMASK(WAKEUP_GPIO_10) | BUTTON_PIN_BITMASK(WAKEUP_GPIO_11);

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

bool downloadImage(String imageUrl, String filePath)
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
  http.setTimeout(5000);        // 设置整体超时时间为5秒
  http.setConnectTimeout(3000); // 设置连接超时为3秒

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK)
  {
    Serial.printf("HTTP 请求失败，状态码：%d\n", httpCode);
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

  bool success = true;

  while ((len = response->readBytes(buffer, sizeof(buffer))) > 0)
  {
    if (file.write(buffer, len) != len)
    {
      Serial.println("写入文件时出现错误");
      success = false;
      break;
    }
  }

  file.close();
  http.end();

  if (success)
  {
    Serial.println("图片已成功下载并保存！");
  }
  else
  {
    Serial.println("图片下载失败");
  }

  return success;
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

bool drawBmp(String filename)
{
  fs::File bmpFS;

  // Open requested file on LittleFS
  bmpFS = LittleFS.open(filename, "r");
  if (!bmpFS)
  {
    Serial.println("无法打开图片文件！");
    return false;
  }
  Serial.println("读取图片中。。。。");

  uint32_t seekOffset, headerSize, paletteSize = 0;
  int16_t row;
  uint8_t r, g, b;
  uint16_t bitDepth;

  uint16_t magic = read16(bmpFS);
  if (magic != 0x4D42) // 'BM'
  {
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

void displayImage(String imageUrl, String filePath)
{
  if (epd.Init() != 0)
  {
    Serial.print("墨水屏启动失败");
    return;
  }

  bool success = downloadImage(imageUrl, filePath);

  if (!success)
  {
    Serial.println("图片下载失败");
    epd.Sleep();
    return;
  }

  Serial.print("图片加载中。。。。");
  drawBmp(filePath);
  Serial.print("图片显示完成");
  epd.Sleep();
}

void displayWiFIinfo(String filePath)
{
  if (epd.Init() != 0)
  {
    Serial.print("墨水屏启动失败");
    return;
  }
  drawBmp(filePath);
  Serial.print("说明图片显示完成");
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

void print_GPIO_wake_up()
{
  int GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println((log(GPIO_reason)) / log(2), 0);
}

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    print_GPIO_wake_up();
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

String getUUID(String url)
{
  HTTPClient http;
  WiFiClient client;
  String result;

  http.begin(client, url);
  http.setTimeout(5000);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK)
  {
    Serial.println("HTTP GET request succeeded");
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    else
    {
      result = doc["uuid"].as<String>();
    }
  }
  else
  {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    result = String();
  }
  http.end();
  return result;
}

int getPhotoCount(String url)
{
  HTTPClient http;
  WiFiClient client;
  int result = 0;

  http.begin(client, url);
  http.setTimeout(5000);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK)
  {
    Serial.println("HTTP GET request succeeded");
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      http.end();
      return 0;
    }
    result = doc["data"].size();
  }
  else
  {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
  }
  http.end();
  return result;
}

String getPhotoUrl(String url, int index)
{
  HTTPClient http;
  WiFiClient client;
  String result;

  http.begin(client, url);
  http.setTimeout(5000);
  int httpResponseCode = http.GET();
  if (httpResponseCode == HTTP_CODE_OK)
  {
    Serial.println("HTTP GET request succeeded");
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      http.end();
    }
    result = doc["data"][index].as<String>();
  }
  else
  {
    Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    result = String();
  }
  http.end();
  return result;
}

void blinktest()
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(RGB_BUILTIN, HIGH);
    delay(300);
    digitalWrite(RGB_BUILTIN, LOW);
    delay(300);
  }
}

void calendar_modle()
{
  Serial.println("当前模式为：日历模式");
  // 同步网络时间并打印
  setRTCTimeFromNTP();
  printDetailedRTCInfo();

  // 清除所有闹钟，确保上次的闹钟不会影响本次
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  // 设置一个固定的闹钟时间：凌晨 00:02:00
  // TODO：联网后获取触发时间，并设置闹钟
  DateTime midnight(2000, 1, 1, 0, 2, 0); // 年月日是占位符，不影响闹钟触发

  // 设置闹钟1，匹配时、分、秒后触发
  // 这样可以确保闹钟在每天的 00:00:00 准时触发
  rtc.setAlarm1(midnight, DS3231_A1_Hour);

  // 打印闹钟时间，便于确认
  Serial.printf("闹钟已设置，将在每天 %02d:%02d:%02d 唤醒\n",
                midnight.hour(), midnight.minute(), midnight.second());

  // 图片 URL 和本地保存路径
  String imageUrl = "http://statics.biogeeker.com/demo.bmp";
  String filePath = "/display.bmp";

  displayImage(imageUrl, filePath);
}

void photo_modle()
{
  Serial.println("当前模式为：相册模式");
  Serial.println("当前为相册模式");
  // 同步网络时间并打印
  setRTCTimeFromNTP();
  printDetailedRTCInfo();

  // 清除所有闹钟，确保上次的闹钟不会影响本次
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // 获取当前时间
  DateTime now = rtc.now();

  // 设置下一个整点时间（分钟和秒数都为0）
  DateTime nextHour(now.year(), now.month(), now.day(), now.hour() + 1, 0, 0);

  // 设置闹钟1，每小时整点触发
  rtc.setAlarm1(nextHour, DS3231_A1_Hour);

  // 打印闹钟时间，便于确认
  Serial.printf("闹钟已设置，将在每小时 %02d:%02d 整点唤醒\n",
                nextHour.hour(), nextHour.minute());
  
  String photolistUrl = "http://statics.biogeeker.com/kada/photolist.json";
  String uuid = getUUID(photolistUrl);
  Serial.println("相册队列uuid为:");
  Serial.println(uuid);
  String currentuuid = preferences.getString("uuid", "inituuid");

  int photoIndex = preferences.getUInt("index", 0);

  if (currentuuid != uuid)
  {
    Serial.println("数据更新了");
    preferences.putString("uuid", uuid);
    preferences.putUInt("index", 0);

    // 图片 URL 和本地保存路径
    String photoimageUrl = getPhotoUrl(photolistUrl, photoIndex);
    String photpdisplay = "/photpdisplay.bmp";
    displayImage(photoimageUrl, photpdisplay);

    photoIndex = photoIndex + 1;
    preferences.putUInt("index", photoIndex);
  }
  else
  {
    Serial.println("数据没有更新");
    photoIndex = preferences.getUInt("index", 0);
    Serial.println("当前图片索引为:");
    Serial.println(photoIndex);
    if (photoIndex >= getPhotoCount(photolistUrl))
    {
      photoIndex = 0;
      preferences.putUInt("index", photoIndex);
    }
    // 图片 URL 和本地保存路径
    String photoimageUrl = getPhotoUrl(photolistUrl, 0);
    String photpdisplay = "/photpdisplay.bmp";
    displayImage(photoimageUrl, photpdisplay);
  }

}
void check_EXT_info()
{
  uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  int modle = preferences.getUInt("modle", 0);

  // 检查是否有 GPIO 被触发
  if (GPIO_reason == 0)
  {
    Serial.println("No GPIO triggered the wake up.");
    if (modle == 0)
    {
      calendar_modle();
    }
    else
    {
      photo_modle();
    }
    return;
  }

  // 检查是否只有一个 GPIO 被触发（即是否为 2 的幂）
  if ((GPIO_reason & (GPIO_reason - 1)) != 0)
  {
    Serial.println("Multiple GPIOs triggered the wake up.");
    return;
  }

  // 使用位运算获取最低置位的 GPIO 编号
  int GPIO_number = __builtin_ctzll(GPIO_reason);

  Serial.print("GPIO that triggered the wake up: GPIO ");
  Serial.println(GPIO_number);

  switch (GPIO_number)
  {
  case 2:
    Serial.println("切换模式");
    rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); // Red

    if (modle == 0)
    {
      photo_modle();
      bool success = preferences.putUInt("modle", 1);
      if (!success)
      {
        Serial.println("状态保存失败");
      }
    }
    else
    {
      calendar_modle();
      bool success = preferences.putUInt("modle", 0);
      if (!success)
      {
        Serial.println("状态保存失败");
      }
    }
    break;
  case 3:
    // 重置相册无线密码
    Serial.println("重置相册无线密码");
    rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); // Green
    delay(2000);
    restpasswd();
    ESP.restart();
    break;
  case 10:
    // 接收DS3231定时重启信号
    Serial.println("定时重启");
    break;
  case 11:
    // 强制刷新
    digitalWrite(RGB_BUILTIN, HIGH); // Turn the RGB LED white
    delay(2000);
    Serial.println("强制刷新");
    ESP.restart();
  default:
    Serial.println("其他原因重启");
    break;
  }

  delay(5000); // 可考虑替换为非阻塞方式，如 millis()
}
void init_system()
{
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
}
void setup()
{
  // put your setup code here, to run once:
  pinMode(48, OUTPUT);
  digitalWrite(48, LOW);

  init_system();

  preferences.begin("kada-photos", false);
  int modle = preferences.getUInt("modle", 0);

  if (!LittleFS.exists("/bgwificonfig/wifiset.txt"))
  {
    Serial.println("######");
    Serial.println("展示配网方法");
    // 配网说明图片
    String infoFilePath = "/demo.bmp";
    displayWiFIinfo(infoFilePath);
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
    check_EXT_info();
  }
  // Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Use ext1 as a wake-up source
  esp_sleep_enable_ext1_wakeup_io(bitmask, ESP_EXT1_WAKEUP_ANY_LOW);
  // enable pull-down resistors and disable pull-up resistors
  rtc_gpio_pulldown_dis(WAKEUP_GPIO_2);
  rtc_gpio_pullup_en(WAKEUP_GPIO_2);
  rtc_gpio_pulldown_dis(WAKEUP_GPIO_3);
  rtc_gpio_pullup_en(WAKEUP_GPIO_3);
  rtc_gpio_pulldown_dis(WAKEUP_GPIO_10);
  rtc_gpio_pullup_en(WAKEUP_GPIO_10);
  rtc_gpio_pulldown_dis(WAKEUP_GPIO_11);
  rtc_gpio_pullup_en(WAKEUP_GPIO_11);

  // Go to sleep now
  Serial.println("Going to sleep now");
  digitalWrite(RGB_BUILTIN, LOW);
  esp_deep_sleep_start();
}

void loop()
{
  wifipw.Loop();
}