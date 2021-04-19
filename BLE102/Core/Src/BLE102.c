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
#include <stdlib.h>

/*硬件层宏定义*/
/********************************************************************/
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
uint8_t BLE102_TPL[1];//BLE102的模块发射功率
uint8_t BLE102_TPL_dbm[8][6]={{"-14dbm"},{"-11dbm"},{"-8dbm"},{"-5dbm"},{"-2dbm"},{"+2dbm"},{"+4dbm"},{"+8dbm"}};//BLE102模块发射等级所对应的功率
										
uint8_t BLE102_PASS[6];//BLE102模块的密码
int BLE102_PASSEN;//BLE102是否使能连接验证
								 //1：使能
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
int BLE102_UART_Pari;//校验位
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
/*
int BLE102_UART_Baudrate;//波特率
int BLE102_UART_Databit;//数据位
int BLE102_UART_Pari;//校验位
											//0：无校验
											//1：奇校验
											//2：偶校验
int BLE102_UART_Stop;//停止位
											//0：1停止位
											//1：2停止位
*/


			/*重写逻辑*/

/*先将串口获取的数据，处理为57600，8，0，0
然后以逗号（0x2C）为分隔符，创建一个二维数组[4][6],第一维是参数，第二维存放数据，在输出的时候依次输出。
写一个循环，创建一个a，每遇到一个逗号，a+1，存储数据就用a，数组[a]
*/
//查询BLE102模块串口参数
void BLE102_READ_UART(void)
{
	uint8_t DATA_OUT[10]="AT+UART?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,10,0x00ff);

	uint8_t DATA_Baudrate[50];
	uint8_t DATA_Databit[50];
	uint8_t DATA_Pari[50];
	uint8_t DATA_Stop[50];

		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		int i=0;
		int X=8;
		//获取串口波特率
		for(i=0;i<UART2_Rx_cnt-X;i++)
		{
			if(UART2_Rx_Buf[i+X]==0x0A || UART2_Rx_Buf[i+X]==0x0D /*|| UART2_Rx_Buf[i+X]==0x2C*/)
			{
				break;
			}
			DATA_Baudrate[i]=UART2_Rx_Buf[i+X];
		}
		sscanf((char*)DATA_Baudrate,"%d", &BLE102_UART_Baudrate);
		//获取串口数据位
		for(i;i<UART2_Rx_cnt;i++)
		{
			if(UART2_Rx_Buf[i+X]==0x2C)
			{
				break;
			}
			DATA_Databit[i]=UART2_Rx_Buf[i+X];
		}
		sscanf((char*)DATA_Databit,"%d", &BLE102_UART_Databit);
		
		//获取串口校验位

		//获取串口停止位



		printf("BLE102模块串口参数:");
		HAL_UART_Transmit(&huart1,DATA_Baudrate,UART2_Rx_cnt-X,0x10);    //发送接收到的数据
		//HAL_UART_Transmit(&huart1,(uint8_t*)BLE102_UART_Baudrate,10,0x10);    //发送接收到的数据
		printf("\r\n%u",BLE102_UART_Baudrate);
		printf("\r\n%u",BLE102_UART_Databit);
		printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块是否使能连接验证
void BLE102_READ_PASSEN(void)
{
	uint8_t DATA_OUT[12]="AT+PASSEN?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,12,0x00ff);
	uint8_t DATA_IN[1];
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-7;i++)
		{
			if(UART2_Rx_Buf[i+7]==0x0A || UART2_Rx_Buf[i+7]==0x0D)
			{
				break;
			}
			DATA_IN[0]=UART2_Rx_Buf[11];
		}
		printf("BLE102模块连接使能验证:");
		if(DATA_IN[0]==0x4E)
		{
		printf("使能");
		BLE102_PASSEN=1;
		}
		else if(DATA_IN[0]==0x46)
		{
		printf("禁止");
		BLE102_PASSEN=0;
		}
		else
		{
		printf("查询BLE102模块是否使能连接验证出错！！");
		}
		//HAL_UART_Transmit(&huart1,BLE102_CIVER,UART2_Rx_cnt-7,0x10);    //发送接收到的数据
		printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块密码/*ERROR*/
void BLE102_READ_PASS(void)
{
	uint8_t DATA_OUT[10]="AT+PASS?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,10,0x00ff);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-8;i++)
		{
			if(UART2_Rx_Buf[i+8]==0x0A || UART2_Rx_Buf[i+8]==0x0D)
			{
				break;
			}
			BLE102_PASS[i]=UART2_Rx_Buf[i+8];
		}
		printf("BLE102模块密码:");
		HAL_UART_Transmit(&huart1,BLE102_PASS,UART2_Rx_cnt-8,0x10);    //发送接收到的数据
		//printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块发射功率
void BLE102_READ_TPL(void)
{
	uint8_t DATA_OUT[11]="AT+TPL?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,11,0x00ff);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{

		BLE102_TPL[0]=UART2_Rx_Buf[7];
		
		printf("BLE102模块发射功率:");
		HAL_UART_Transmit(&huart1,BLE102_TPL,1,0x10);    //发送接收到的数据
		printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块软件版本号
void BLE102_READ_CIVER(void)
{
	uint8_t DATA_OUT[11]="AT+CIVER?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,11,0x00ff);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-7;i++)
		{
			if(UART2_Rx_Buf[i+7]==0x0A || UART2_Rx_Buf[i+7]==0x0D)
			{
				break;
			}
			BLE102_CIVER[i]=UART2_Rx_Buf[i+7];
		}
		printf("BLE102模块软件版本号:");
		HAL_UART_Transmit(&huart1,BLE102_CIVER,UART2_Rx_cnt-7,0x10);    //发送接收到的数据
		printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
}

//查询BLE102模块名称
void BLE102_READ_NAME(void)
{
	uint8_t DATA_OUT[10]="AT+NAME?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,10,0x00ff);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-7;i++)
		{
			if(UART2_Rx_Buf[i+7]==0x0A || UART2_Rx_Buf[i+7]==0x0D)
			{
				break;
			}
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
			if(UART2_Rx_Buf[i+7]==0x0A || UART2_Rx_Buf[i+7]==0x0D)
				{
					break;
				}
			BLE102_MAC[i]=UART2_Rx_Buf[i+7];
		}
		printf("BLE102模块MAC地址：");
		HAL_UART_Transmit(&huart1,BLE102_MAC,UART2_Rx_cnt-6,0x10);    //发送接收到的数据
		printf("\r\n");
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
	BLE102_READ_CIVER();//查询模块软件版本号
	//BLE102_READ_TPL();//查询模块发射功率/*疑似查询之后需要重新进AT*/
	BLE102_READ_PASS();//查询模块密码
	BLE102_READ_PASSEN();//查询BLE102模块是否使能连接验证
	BLE102_READ_UART();//查询BLE102模块串口参数
	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
