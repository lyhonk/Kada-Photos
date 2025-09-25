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

## Parts List
1. [七色墨水屏，带驱动板](https://m.tb.cn/h.SeE8QkH?tk=PNn34DUXIfi)（闲鱼购买，卖家：记得带马扎）
2. [ESP32 SuperMini开发板](https://item.taobao.com/item.htm?id=757262224552)（淘宝店铺为无名科技，他们家应该是原版，其他的家的板子可能会有点问题，遇到过板载LED灯无法完全熄灭的问题，可以购买向下焊接排针版）
3. [1800mAH 锂电池](https://item.taobao.com/item.htm?id=756027058909)（淘宝店铺为无名科技，也可以购买其他家的，安全、便宜的就可以，不需要带充电模块）
4. [DS3231 高精度时钟模块](https://item.taobao.com/item.htm?id=551150126785)（购买带电池版本）
5. [5V锂电池充放电模块 输出常开](https://item.taobao.com/item.htm?id=737971031309)（购买焊线款就可以，充放电模块一定要买输出常开的这种，避免开发版进入休眠后因电流过小导致的断电）
6. [双面万能板 喷锡 6 * 8cm](https://detail.tmall.com/item.htm?id=607841294118)（6 * 8CM的尺寸正好合适相框，最大不要超过10 * 15CM，可以适当多买几块备用）
7. [2PIN磁吸线](https://item.taobao.com/item.htm?_u=qiptnth32b2&id=650683179648&pisk=gW94G8bzzxHVdtaShUBa4c6qlk6AHOuISd_1jhxGcZbm6iMgQh-hfm3tDQSM5UntDttG7F85yRssMVewzEtHGsT6GNjGrUxbhNMA_F-BJFwsHIsgQUtOsF9wXRSMjFnA5mhWDnBOI2gC7vtvDYSmRL9VSgVk2Gwgmb10gY-jV2gIdAl0qOuqRFs-cxTlYMXGm1V0q0SAvSXGsixo4Gj_o52DS3mPyG_gsNVcrYjNbGXGjixuZiIOmrVcjgqlyGXGmZYMq0S1rNXGIFxoKs8Mk87Pi0rf5dhYhk1c-nbzIJrdAsmkD5w_CXsAiwflSRqWUi5V-n9EzzDNqL9PO_Zs_TxpNEjPLYNcK3RM8BYtO7B2jIYOaE38aaOkHhbGgl2p4U-Mva-rLWLF3F5Hn_rgCn6VrTSkKcNdPL7XSK5oAJ51Ep1hnQG-l16VYFvvulyM-3tBH_pZ8zb9GMBlAnH4U9xcjgPUWgDi1dd4S55c2g7I40STleoPyLdH-5FOabIPRDnL65Cc2RN2CmVT6_JV4wimx&skuId=5021346002204&spm=a1z09.2.0.0.14852e8diFvSPq)和[2PIN2.8mm间距磁吸母座](https://item.taobao.com/item.htm?_u=qiptnth32b2&id=650683179648&pisk=gW94G8bzzxHVdtaShUBa4c6qlk6AHOuISd_1jhxGcZbm6iMgQh-hfm3tDQSM5UntDttG7F85yRssMVewzEtHGsT6GNjGrUxbhNMA_F-BJFwsHIsgQUtOsF9wXRSMjFnA5mhWDnBOI2gC7vtvDYSmRL9VSgVk2Gwgmb10gY-jV2gIdAl0qOuqRFs-cxTlYMXGm1V0q0SAvSXGsixo4Gj_o52DS3mPyG_gsNVcrYjNbGXGjixuZiIOmrVcjgqlyGXGmZYMq0S1rNXGIFxoKs8Mk87Pi0rf5dhYhk1c-nbzIJrdAsmkD5w_CXsAiwflSRqWUi5V-n9EzzDNqL9PO_Zs_TxpNEjPLYNcK3RM8BYtO7B2jIYOaE38aaOkHhbGgl2p4U-Mva-rLWLF3F5Hn_rgCn6VrTSkKcNdPL7XSK5oAJ51Ep1hnQG-l16VYFvvulyM-3tBH_pZ8zb9GMBlAnH4U9xcjgPUWgDi1dd4S55c2g7I40STleoPyLdH-5FOabIPRDnL65Cc2RN2CmVT6_JV4wimx&skuId=5408860679026&spm=a1z09.2.0.0.14852e8diFvSPq) （带USB的磁吸线和磁吸母座需要分开购买）
8. [8寸相框](https://detail.tmall.com/item.htm?_u=qiptnth3450&id=729576054896&pisk=g0N0Gmwun-kXTvdXnNcfH1dGXy6RhjG__ldtXfnNU0o55KpAlAuaSlVTHPotslqgjmHNcjFgqyaOl5Qjg10avucTkmnTqNqY5CIfCjdwjoaFcrnOl5mZqoriCZitbcqT7-QR96UblfGwsMCd9oH8SVZD_CoN7_ujoa3VP-5aLfGNvGvJ_x1n1oJ7K6f2U4oSSV82_llr42gs_VowgamrS29w0llNra0tRKuZbVuzzVgi_nkw3Q-r-VJ2_CJ2Ua0s4clZ_llPrVirbflZpdpqv5yUaGWj7E-_66FqoxmUumzYk7SjEKaoqETYZ8ViYXnk_CPolPVmohAhBDyIDvHb4_dmszugD0EFTQcrKJatSofDgjezFSGYwMYInlmoL7MlLUD84SqiDXYNgzcmmvPxHFR3Yz4apYVR86GmgmytGPLhDzV0DrNuWFJrijeoKScFOnoTeyViS5s1Zk4QNu0zY1jV4mpyLMfX147Tad9s34gov82UeJcYppkdravL5xuSRDQlrd96ACR9Uabkp-DqPqoA.&spm=a1z09.2.0.0.1e8b2e8dWy9zrX) （卡纸开孔一定要备注为 16 * 9.5 厘米，不备注默认开孔为6寸，会遮挡屏幕）
9. [无反光玻璃](https://item.taobao.com/item.htm?_u=qiptnth9168&id=665807301515&pisk=g7aQGPm-bpvCSzrxA45wfglUZFgSV17VyQG8i7Lew23KVUFUhb8r4McWPANAtvSlx8Z_eRmEpYorPbwqIW8PtW55P7Fxp4rELbeaFRxULzkzQaN0hD8UwzR3j-Pv8ySnzUgnr4BV3Z7qTW0oy9Go4vpnBXllzUKKe2093vOFDZ74t5OE9sPOuz82EzhM2Ue-vcQttAhpJYHd1chqw4LK2eK9BAc-y4npJFntij8-e4nd1hhjGLHJJvFtWjlWv438e5CsZAhKy4e861GoKKvsaMG8OsUz0FOcGKFmCUL85fBoHW6JuXajO6kbTA6vDPamAxFKCUWEeWKjFA4AQGNgYkeEs8_fC42g28h7BdBrx7E_CXwFhLDz-WzSLWsdB5gZd7n7_iTjJJcSD4n6wUFs-AoY5qQvCfqTTuU3dQT-tyzqVxm1wUmrWroYDJOli53Kw8m0ztYnp7F0oouf7H3Y1uebVgJwuxgRM3OsmUGs3116q3AKgzBAIE8FrDhi9Z511dtovfcs3z562hiKsXER11t2I&skuId=4796095864111&spm=a1z09.2.0.0.1e8b2e8dWy9zrX) (非必需，只是把相框自带的亚克力换为无反光玻璃后，效果更清晰，尺寸选择8寸 15.2X20.3cm 厚度2mm)
10. [2P 2.54mm弯针端子和2P 2.54mm胶壳](https://detail.tmall.com/item.htm?_u=qiptntha9f4&id=608827349017&pisk=gsL8GaABixDu64ezwpmm-MSZRGl0ymAygLR_-9XuAKpvpCFkqMXhAwpwBMan4ajdJK6DrTAlP6sBKdtlF3X3Jep2ez4hPp0ddC-qTTfkrWCBcLUHqQXkDWQyo34hEY7pOdbKIA0iSQRPbwMiI4IFAt_11yZ7KuwbGwfpmQZu1QRP89N0dcxXaWEiPWk5O9GAl6C8NwaCdtGA91BCRTsQho11h9_Bdgwbl6C_Ay_CRZGA66wQdMa5hs1lO_wWdwGAl66fdwsWdIGATt6ILxcR3718J3aVH0eurD2QdECR29UhByIM7PjhCjB4JyB8Wg61StUQRECy9UC8pPnhChWljgJjozQpG6plndg_kZs9bBfpe4U5rnt2zaYmB5SJptI6uin_MivXypBlWWgWMaBRfUjuOPp6MBtGDEcs7aQ5FHbyt5HkMUpkaEd3Oo_RrnI9yNgaF9xD6eCp8vuvBC82Hs9sJYszQFYOmNyGB6qSMjEUYg14DtzB-RLu9Z5AIbs7YkShgsBiMjEQARzOMOc7okrFD65..&spm=a1z09.2.0.0.1e8b2e8dWy9zrX&skuId=4450022483177) （非必需，用来链接电池）
11. 轻触开关、导线、排针等

## Tools
1. [电烙铁](https://detail.tmall.com/item.htm?_u=qiptnth8597&id=744595862328&pisk=gacLTC0SShx3RpjFJ0vMZ8y2N-8Gyd0E7DufZuqhFcnt2qQnK8quF7nqD81kLW28wcZitk03OzwSqmM3AvqlwbnZv6fuO0A8Vqkwzk4ntaUSCDCoK2qnBaFEsvfuxHP-PmVJmnADi2uU37tDmBezFlNbl915q_s6174-I26hl2uUau_GVKDs8aBD9Wv7PuTT1zUdR71CNOTTyrE7Nkw5fOabfuNSVJs11zUfF_N7N5TTlr15d9aC5Oa8zzsSV7TT1lz7Nyi7NF3_bzMF-eENRyC-d1r9pMfy-aQ-6yeL2_4AwllRi-rfCl1RwkLYLoN0X_1S6yu-9yIRMLqj3k20LcOG973_yDqicI1T1-Hqrznp96NnCAlUtfxAuJn8X-GaBK_TIcGK2kVAPBgT9kejtJb5MmgtPx2tsEO49WZIEWulkH0t9D0E64b5CWFnB-hLGISQqA0xwzhy4CnxufoT5mOJAg8ZijeggiqYr9T9WTWzdPWTb_GlDXb8_PEDRJ6PU84aWoY9WT650sUTme1GUTyTz&spm=a1z09.2.0.0.1e8b2e8dWy9zrX) （推荐正点原子T80/T80P智能电烙铁，升温快，温度准，不会烫坏元器件，对比原先买的电烙铁，就是降维打击 ）
2. 焊锡 （含铅的焊锡更好用）
3. [助焊笔](https://item.taobao.com/item.htm?_u=qiptnth8b7f&id=556541766052&pisk=gDK0ToTkoE7bfmqboAjjkcEi6pgRZis6bCEO6GCZz_5SCrUxhNWwICx9kd5OjCAMssQZGiKMZp9thhHf0cXw9Qj9HsC9ZAAvClhj5iEasI9EGKCthhfNZIRG5x1O_1A9QEH-vDpXhGsajXnKvIQJIOO0b1WqQaWAdYBqFEow8GsZ9f48bZihfIzWKSwazT55IOy4uC5zU9CPQrSw__7PL9FVbGRZEgWFKOrNQ1JyUOCzurWN3TSPL9UN_t7ZEg55QG5w_ZkkU_6Nb1khGr5pbe-r_q6a35TSLdBco6JVgKLD4j_LlIfuH-tkr2fp3FZab3Xcl37HagSckFJCRnK-jusvKUSPpKi0qMJVKICJTDrl4Lpy6wTS1kQWZhbw0ZeZT6Rv4pWkFSzVKifc8n77dk9V7UxlDnl_RLplgwjJy4GAWiADR6_qPbOMEsdwmaPmw1Lpen7kTbEk1NYBBOAmmoRF4nUz8XmbfTk94Pa1uT6l9UYeyejvJy7KEY4pCZW5d6HoEPa_OlrTzYDuJEbVFt5A.&skuId=3608176382515&spm=a1z09.2.0.0.1e8b2e8dWy9zrX)或[助焊剂](https://item.taobao.com/item.htm?_u=qiptntha79d&id=657207515317&pisk=gkdLTXTWScmhnuZUJ3DiZTWqNtuMNAYe7H8bZgj3FhK92EUHKTjlF_K2DTNoL6XRwhsMtMYhOa6Wqn9hApjuwQKwvWVlO3cRVEJZzMbHtzQWCHFkKejHBzCespVlxD5JPnf-mmcmie8F3_immXBPFG1flJw7qa__f_bJIew3le8FagagVxv68zemtfM5PggO1aQLR_s5NPgOyZI5NM67fR_ffg1WV9Z_1aQbF715N1gOkZECP_NQ11_czwZBV_g95G75Nwt5NVL1baMUK2IaRwFJd5Stpkcz8jhv6wBd27fbwQpai6jjrGP8wM3ALi1cX7NW6w8J9wE8MYjX3MXcLhGg9_L1yHjMcjNO1tp2raKK9W1HCdRFtCmYu9KRXtONBxaOIhOp2MfYPXTO9MBXt947MnT9PKX9sqGV96sBE68ukDY99HYe6U47C6CHBtddGjrCqdYvwadr4fKvuC-O5nG-Ag8wiIBGgojArJgtW8yPdNyOb7OuDB4R_NImR9wzUTbNWi0tW8w70SQOm2NgU8WOz&spm=a1z09.2.0.0.1e8b2e8dWy9zrX) （以前不知道有这个东西，焊接电源线的时候把电路板烧透了都焊不上，涂点助焊剂焊接更轻松）
4. 美纹纸 （用来固定屏幕）
5. 美工刀 （给相册背板开孔）
6. 热熔胶 （固定洞洞板、电池）


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
