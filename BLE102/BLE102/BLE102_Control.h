#include "main.h"
#include "usart.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "stdarg.h"
#include "math.h"

//返回值
#define BLE102_OK 0
#define BLE102_Error 1

#define BLE102_ADP_ON 1  //使能
#define BLE102_ADP_OFF 0 //禁止
struct BLE102_ADP
{
    //模块是否进行广播
    int ADP_EN;
    //模块的自定义广播速度 10-1024
    int ADPTIM;
};
//周围从机参数
struct BLE102_SCAN
{
    //周围从机的索引号和MAC地址对照
    uint8_t MAC[20][12];
    //周围从机的索引号和信号强度对照
    uint8_t Size[20][10];
};

//模块连接状态
#define BLE102_LINK_Online 0  //连接成功
#define BLE102_LINK_OFFLine 1 //未连接
//连接设备参数
struct BLE102_Link
{
    //模块连接状态
    int Status;
    //已连接设备的地址
    uint8_t MAC[12];
    //已连接设备的信号强度
    uint8_t Rssi;
};
//波特率
#define BLE102_Baud_Rate_1200 1200
#define BLE102_Baud_Rate_9600 9600
#define BLE102_Baud_Rate_57600 57600
#define BLE102_Baud_Rate_115200 115200
#define BLE102_Baud_Rate_512000 512000
#define BLE102_Baud_Rate_921600 921600
//数据位
#define BLE102_Data_Bit_5 5
#define BLE102_Data_Bit_6 6
#define BLE102_Data_Bit_7 7
#define BLE102_Data_Bit_8 8
//校验位
#define BLE102_Test_Bit_NO 0   //无校验
#define BLE102_Test_Bit_Odd 1  //奇校验
#define BLE102_Test_Bit_Even 2 //偶校验
//停止位
#define BLE102_Stop_Bit_1 0 //1停止位
#define BLE102_Stop_Bit_2 1 //2停止位
struct BLE102_Usart
{

    int Baud_rate; //波特率
    int Data_Bit;  //数据位
    int Test_Bit;  //校验位
    int Stop_Bit;  //停止位
};
//设备模式
#define BLE102_Mode_Mast 0    //主设备模式
#define BLE102_Mode_Slave 1   //从设备模式
#define BLE102_Mode_iBeacon 2 //广播或iBeacon模式
#define BLE102_Mode_Mesh 3    //Mesh组网模式
//设备发送功率
#define BLE102_TPL_1 1
#define BLE102_TPL_2 2
#define BLE102_TPL_3 3
#define BLE102_TPL_4 4
#define BLE102_TPL_5 5
#define BLE102_TPL_6 6
#define BLE102_TPL_7 7
#define BLE102_TPL_8 8
//使能连接验证
#define BLE102_PASSEN_ON 1  //使能
#define BLE102_PASSEN_OFF 0 //禁止
//自动睡眠
#define BLE102_AUTOSLEEP_ON 1  //使能
#define BLE102_AUTOSLEEP_OFF 0 //禁止
//自动重连功能
#define BLE102_AUTOCONN_ON 1  //使能
#define BLE102_AUTOCONN_OFF 0 //禁止
//最大输出
#define BLE102_MAXPUT_ON 1  //使能
#define BLE102_MAXPUT_OFF 0 //禁止
//数据加密传输
#define BLE102_TRENC_ON 1  //使能
#define BLE102_TRENC_OFF 0 //禁止
//AT模式
#define BLE102_AT_Order 1//命令模式
#define BLE102_AT_DATA 0//数据模式
typedef struct __BLE102
{
    //复位
    uint16_t Reset_Pin;
    GPIO_TypeDef *Reset_Prot;
    //唤醒引脚
    uint16_t Wake_Pin;
    GPIO_TypeDef *Wake_Prot;
    //所使用的串口通道
    UART_HandleTypeDef UART_Aisle;
	
    //AT配置模式
    uint8_t AT_Mode;
    //模块名称
    uint8_t NAME[15];
    //设备工作模式
    uint8_t Mode;
    //BLE102的MAC地址
    uint8_t MAC[12];
    //BLE102的软件版本号
    uint8_t CIVER[20];
    //BLE102的模块发射功率
    uint8_t TPL;
    //BLE102模块的密码
    uint8_t PASS[6];
    //BLE102是否使能连接验证
    int PASSEN;
    //BLE102的模块串口打包时间
    int UARTTM;
    //自动睡眠参数
    int BLE102_AUTOSLEEP_Status;
    //自动睡眠等待时间 0-100
    int BLE102_AUTOSLEEP_Time;
    //开机欢迎语
    uint8_t BLE102_HELLO[20];
    //设备上电默认连接模块的MAC地址
    uint8_t BLE102_CONNADD[12];
    //是否使能自动重连功能
    int BLE102_AUTOCONN;
    //是否使能最大输出
    int BLE102_MAXPUT;
    //是否使能数据加密传输
    int BLE102_TRENC;
    //嵌套结构体
		
    struct BLE102_Usart Usart;
    struct BLE102_Link Link;
    struct BLE102_SCAN SCAN;
    struct BLE102_ADP ADP;
} Bluetooth_BLE102;


//函数
extern Bluetooth_BLE102 BLE102_1;

int BLE102_Pretreatment(Bluetooth_BLE102 *BLE102);
int BLE102_UASRT_IRQHandler(Bluetooth_BLE102 *BLE102);
int BLE102_UASRT_Interrupt_reload(Bluetooth_BLE102 *BLE102);
int BLE102_UASRT_DMA_OVER();
int BLE102_AT_In(Bluetooth_BLE102 *BLE102);
int BLE102_HardRest(Bluetooth_BLE102 *BLE102);
int BLE102_AT_Out(Bluetooth_BLE102 *BLE102);
int BLE102_judge_AT_MODE(Bluetooth_BLE102 *BLE102);