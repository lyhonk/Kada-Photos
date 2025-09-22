# Introducing the all-new Kada Photos — Your memories, perfected. 

```
目前已更新硬件代码，组装教程本周更新
```

介绍一下最近做的小项目——咔哒相册
当你的显示器已经达到了144HZ的时候，我做了一个电子相册，刷新一次需要38秒
相册使用了一块墨水屏，能够最大程度还原纸张的质感
这样我就可以把日历、油画、照片全部装进去
当相册中照片像胶卷冲印一样缓慢展现的时候
回忆也会慢慢浮现
# 先来看效果
## 1. 照片模式
![1. 照片模式](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C-4-1152x1536.jpg)
## 2. 横向照片展示
![2. 横向照片展示](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C-2-1536x1152.jpg)
## 3. 日历模式
![3. 日历模式](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C.jpg)
## 桌面效果展示
![](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C-7-1536x1152.jpg)
![](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C-6-1536x1152.jpg)
![](https://statics.biogeeker.com/2025/07/%E5%92%94%E5%93%92%E7%9B%B8%E5%86%8C-3-1536x1152.jpg)
## 咔哒相册
起这个名称的原因是因为在组装相册的时候会有一声清脆的咔哒声，相册后背板使用压入式弹珠，装配的时候声音很好听，另一个原因就是这个相册使用磁吸充电，充电线吸附的时候，就会发出咔哒声，也很有特色，昨晚焊完磁吸接口的时候，充电器咔哒一下准确到位，就感觉这个项目就应该叫``“咔哒”``。
![](https://statics.biogeeker.com/2025/07/image.png)
![](https://statics.biogeeker.com/2025/07/cgi-bin_mmwebwx-bin_webwxgetmsgimg__MsgID2178765207799436223skey@crypt_60cd12b4_e593167bae09a05a89ee64859c1bfa4emmweb_appidwx_webfilehelper-1536x864.jpg)
![](https://statics.biogeeker.com/2025/07/cgi-bin_mmwebwx-bin_webwxgetmsgimg__MsgID9187682236043910517skey@crypt_60cd12b4_e593167bae09a05a89ee64859c1bfa4emmweb_appidwx_webfilehelper.jpg)
# 刷新效果展示
![](https://statics.biogeeker.com/2025/07/1702486767.mp4)
# 简要介绍一下特性
## 1. 省电
采用墨水屏和esp32处理器，每小时查询数据判断是否更新图片，一块1000mAh的电池我估计可以用半个月，这就可以让这个电子相册和普通相册一样，挂墙或者放在桌面，不用拖着一条长长的充电线。
## 2. 灵活度高
采用C/S架构，相册只负责处理图片，图像处理、数据获取等都在服务器端执行，自由度更高，现在日历的数据来源于韩寒开发的《ONE》APP，后期会添加油画和电影剧照模式，这样更有意思一些。
## 3. 组装简单
材料主要有
1. 7.3寸彩色墨水屏
2. 淘宝购买的成品相册
3. 墨水屏转接板
4. Esp32S3 super mini控制器
5. 锂电池充放模块及锂电池
6. 洞洞板、磁吸充电模块等
7. 热熔胶

简单焊接以后，用热熔胶将电池粘在洞洞板上，再将洞洞板粘在相册背板上，干净又卫生。
## 4. AI编程深度参与
说实话，我是对硬件确实懂得不多，对嵌入式开发也是一知半解，这次项目开发主要用到了通义灵码进行辅助编程，极大的提高了效率。基本上就是一分钟搭框架，10秒钟给出bug解决方案，1秒钟就能猜到你下一步要写的内容，甚至连变量名都给想好了。
![通义灵码网站](https://statics.biogeeker.com/2025/07/Snipaste_2025-07-20_00-12-48-1536x1309.jpg)
![这是AI提示的Bug可能原因](https://statics.biogeeker.com/2025/07/%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE-2025-07-16-000342.png)
![](https://statics.biogeeker.com/2025/07/%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE-2025-07-16-000229-e1752941264723.png)
![给出的方案完美解决bug](https://statics.biogeeker.com/2025/07/%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE-2025-07-16-000250-1-e1752941308391.png)
# Todo
1. 代码整理后进行开源
2. 整理组装步骤
# 参考资料
1. [**我在数字时代做了一个电子日历，让油画和照片可以被装进去**](https://sspai.com/post/82704)
2. **[eInkPhotoAlbum](https://github.com/Debatrix/eInkPhotoAlbum)**
3. [**eInk-calendar**](https://github.com/westqzy/eInk-calendar)
4. [**eInkCalendarOfToxicSoul**](https://github.com/breakstring/eInkCalendarOfToxicSoul)
5. [**WINK**](https://github.com/Aircoookie/WINK)
6. [示例图片及题图来源（出镜：@五颗桃子 @Yvestiny 摄影：@陆想陆 ）](https://weibo.com/2066540507/H1BGpixxv)
