# HAL库示例程序

* [English Version](./README.md)

* `01-GPIO` GPIO 测试，按下KEY，LED快闪，KEY未按下，LED慢闪
* `02-ADC_RTC_Test` ADC和RTC测试，使用USB CDC虚拟串口显示数据
* `03-CDC_Standalone` USB虚拟串口例程，PA9,PA10为串口
* `04-USBDisk+FATFS` 模拟U盘和FATFS文件系统例程，需要外接FLASH
* `05-SPIFlash_Erase` SPI Flash 擦除例程，需要外接FLASH
* `06-F401_ST7735` ST7735 LCD显示例程，需要外接屏幕

## 重要说明

8Mhz晶振的板子，打开工程前先运行工程目录里的`CrystalsChangeTo8Mhz.bat`,脚本会自动修改工程的晶振配置,由25Mhz改为8Mhz

如果板子背面没有`WeAct`和`版本号`，芯片批次是`537`或旧的，插针和板子是装在一个袋子里，`丝印印刷是错误的`(`3V3`印成了`V3V`)，肯定是盗版。可以issues我们或通过电子邮件(WeAct_TC@163.com)告知我们。
