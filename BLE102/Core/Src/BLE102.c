#ifndef __BLE102_H__
#define __BLE102_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BLE102.h"
#include "usart.h"
#include "stdio.h"
#include <string.h>

int BLE102_AT_command_MODE=0;//串口命令模式

/*BLE102状态变量*/
uint8_t BLE102_NAME[15];//BLE102的名称
int BLE102_MODE=1;//BLE102的模式
								//0：主设备模式
								//1：从设备模式
								//2：广播或iBeacon模式
								//3：Mesh组网模式
uint8_t BLE102_MAC[12];//BLE102的MAC地址
uint8_t BLE102_CIVER[20];//BLE102的软件版本号
int BLE102_TPL;//BLE102的模块发射功率
uint8_t BLE102_PASS[6];//BLE102模块的密码
int BLE102_PASSEN;//BLE102是否使能连接验证
								 //1：使能S
								 //0：禁止
uint8_t BLE102_HELLO[20];//开机欢迎语
uint8_t BLE102_CONNADD[12];//设备上电默认连接模块的MAC地址
int BLE102_ADP;//模块是否进行广播
								//0：关闭广播
								//1：开启广播
int BLE102_ADPTIM;//模块的自定义广播速度 10-1024
int BLE102_AUTOCONN;//是否使能自动重连功能
										//0：禁止
										//1：使能
int BLE102_MAXPUT=1;//是否使能最大输出
										//0：禁止
										//1：使能
int BLE102_TRENC;//是否使能数据加密传输
										//0：禁止
										//1：使能
/*模块自动睡眠参数*/
int BLE102_AUTOSLEEP_Status;//是否自动睡眠
int BLE102_AUTOSLEEP_Time=5;//自动睡眠等待时间 0-100
/*模块串口参数*/
int BLE102_UART_Baudrate;//波特率
int BLE102_UART_Databit;//数据位
int BLE102_UART_Pari;//校验
											//0：无校验
											//1：奇校验
											//2：偶校验
int BLE102_UART_Stop;//停止位
											//0：1停止位
											//1：2停止位
/*模块连接状态*/
uint8_t BLE102_LINK_MAC;//已连接设备的地址
uint8_t BLE102_LINK_Rssi;//已连接设备的信号强度
int BLE102_LINK_Status;//模块连接状态
														//1：Online：连接成功
														//0：OFFLine：模块未连接
/*周围从机数组*/
uint8_t BLE102_SCAN_MAC[20][12];//周围从机的索引号和MAC地址对照
uint8_t BLE102_SCAN_Size[20][10];//周围从机的索引号和信号强度对照
/*iBeacon功能参数（参数长度暂定20）*/
uint8_t BLE102_IBEACON_Uuid[20];//iBEACON设备识别码
uint8_t BLE102_IBEACON_Major[20];//major编号
uint8_t BLE102_IBEACON_Minor[20];//minor标号
uint8_t BLE102_IBEACON_Rssi[20];//信号强度
/********************************************************************/
#define BLE102_RES_Enable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_RESET)  //低电平
#define BLE102_RES_Disable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_SET)	   //高电平

//判断两个字符串是否一致
int StringComparison(uint8_t* DATA,uint8_t* pDATA,int NUM)
{
	int RE_NUM = 1;//返回值 
	for(int i=0;i<NUM;i++)
	{
		if(DATA[i+1]==pDATA[i+1])
		{
		RE_NUM=1;
		}
		else 
		{
		RE_NUM=0;
		}
	}
	return RE_NUM;
}

//查询BLE102模块名称
void BLE102_READ_NAME(void)
{
	uint8_t DATA_OUT[10]="AT+NAME?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,10,0x00ff);
	uint8_t DATA_INT[15]={0};
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-7;i++)
		{
			BLE102_NAME[i+1]=UART2_Rx_Buf[i+8];
		}
		printf("BLE102模块名称：");
		HAL_UART_Transmit(&huart1,BLE102_NAME,UART2_Rx_cnt-7,0x10);    //发送接收到的数据
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块MAC地址
void BLE102_READ_MAC(void)
{
	uint8_t DATA_OUT[9]="AT+MAC?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,9,0x00ff);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-6;i++)
		{
			BLE102_MAC[i+1]=UART2_Rx_Buf[i+7];
		}
		printf("BLE102模块MAC地址：");
		HAL_UART_Transmit(&huart1,BLE102_MAC,UART2_Rx_cnt-6,0x10);    //发送接收到的数据
		UART2_INT_REST();//重置中断数据缓存
	} 
}
//复位BLE102模块
void BLE102_REST(void)
{
	printf("复位BLE102模块：");
	BLE102_RES_Enable();
	HAL_Delay(300);
	BLE102_RES_Disable();
	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //发送接收到的数据
		UART2_INT_REST();//重置中断数据缓存
	} 
	HAL_Delay(500);
	printf("\r\n");
	
}

//进入指令模式
void BLE102_AT_instruction(void)
{
	printf("开始进入AT指令模式\r\n");
	BLE102_AT_command_MODE=1;
	uint8_t DATA[3]="+++";
	HAL_UART_Transmit(&huart2,DATA,3,0x00ff);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	uint8_t pDATA[5]="+ok\r\n";
	if(UART2_Rx_flg && StringComparison(UART2_Rx_Buf,pDATA,5)== 1)
	{
		printf("进入AT指令模式\r\n");
		UART2_INT_REST();//重置中断数据缓存
	}
	else
	{
		printf("进入AT指令模式出现错误！！\r\n");
	}
}

void BLE102_Init(void)
{
	printf("BLE102初始化： \r\n");
	


	BLE102_REST();//复位BLE102模块
	BLE102_AT_instruction();//进入指令模式
	BLE102_READ_NAME();//查询模块名称
	BLE102_READ_MAC();//查询模块MAC地址
	
	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
