#ifndef __BLE102_H__
#define __BLE102_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BLE102.h"
#include "BLE102_text.h"
#include "usart.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "stdarg.h"
#include "math.h"

//声明串口中断变量
extern uint8_t UART1_TX_DMA_OVER;							//USART1发送完成标志
extern uint8_t UART2_TX_DMA_OVER;							//USART2发送完成标志
extern uint8_t UART1_Rx_flg;                   //USART1接收完成标志
extern uint8_t UART2_Rx_flg;                   //USART2接收完成标志
extern uint8_t UART1_receive_buff[255];						//串口1的缓存变量
extern uint8_t UART2_receive_buff[255];						//串口2的缓存变量
extern int UART1_data_length;											//串口1的数据长度
extern int UART2_data_length;											//串口2的数据长度
/*字符串截取函数变量*/
uint8_t String_Intercept_DATA_return[255]={0};//返回数据
int String_Intercept_DATA_long=0;//数据长度
/*DMA发送缓存*/
uint8_t DATA_Printf[255]={0};
/*硬件层宏定义*/
/********************************************************************/
int BLE102_AT_command_MODE=0;//串口命令模式

/*BLE102状态变量*/
int BLE102_AT_MODE=0;//BLE102的AT指令模式状态
					//0：不在AT指令模式
					//1：在AT指令模式
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
int	BLE102_UARTTM;//BLE102的模块串口打包时间
uint8_t BLE102_PASS[6+1]={0};//BLE102模块的密码
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
														//0：禁止
														//1：使能
int BLE102_AUTOSLEEP_Time=5;//自动睡眠等待时间 0-100

/*模块串口参数*/
uint8_t BLE102_UART_DATA[4][10]={0};
//[0]波特率
//[1]数据位
//[2]校验位
	//0：无校验
	//1：奇校验
	//2：偶校验
//[3]停止位
	//0：1停止位
	//1：2停止位
/*模块连接状态*/
uint8_t BLE102_LINK_MAC[12];//已连接设备的地址
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

/**GPIO操作指令*/
#define BLE102_RES_Enable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_RESET)  //低电平
#define BLE102_RES_Disable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_SET)	   //高电平


/*BLE102模式操作函数*/

//复位BLE102模块
void BLE102_REST(void)
{
	printf("复位BLE102模块：");
	BLE102_RES_Enable();
	HAL_Delay(300);
	BLE102_RES_Disable();
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //发送接收到的数据
		USAR2_Interrupt_reload();//重置中断数据缓存
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
	
	uint8_t pDATA[1]="a";
	if(UART2_Rx_flg && StringComparison(UART2_receive_buff,pDATA,1)==1)
	{
		USAR2_Interrupt_reload();//重置中断数据缓存
		HAL_UART_Transmit(&huart2,pDATA,1,0x10);
	}
	else
	{
		BLE102_AT_MODE=0;
		printf("进入AT指令模式出现错误！！\r\n");
	}

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	uint8_t nDATA[5]="+ok\r\n";
	if(UART2_Rx_flg && StringComparison(UART2_receive_buff,nDATA,5)==1)
	{

		USAR2_Interrupt_reload();//重置中断数据缓存
		BLE102_AT_MODE=1;
		printf("进入AT指令模式\r\n");
	}
	else
	{
		BLE102_AT_MODE=0;
		printf("进入AT指令模式出现错误！！\r\n");
	}
}

//判断是否在指令模式，用于指令执行前检查
void BLE102_judge_AT_MODE(void)
{
	if(BLE102_AT_MODE==0)
	{
		BLE102_AT_instruction();
	}
}

/**
* @brief  设置BLE102模块的工作模式
* @param	DATA:工作模式
				M:主设备模式
				S:从设备模式
				B:广播或iBeacon模式
				F:Mesh组网模式
* @retval DATA_Return：
											成功返回1
											失败返回0
*/
int BLE102_write_MODE(uint8_t* DATA)
{
	BLE102_judge_AT_MODE();

	uint8_t Compared_DATA[4][1]={{"M"},{"S"},{"B"},{"F"}};//创建对比变量
	
	char DATA_OUT[11]="AT+MODE=";

	if(StringComparison(DATA,Compared_DATA[0],1))
	{
		strcat((char*)DATA_OUT,(char*)"M\r\n");
		USART2_DMA_printf(DATA_OUT,11);
	}
	else if (StringComparison(DATA,Compared_DATA[1],1))
	{
		strcat((char*)DATA_OUT,(char*)"S\r\n");
		USART2_DMA_printf(DATA_OUT,11);
	}
	else if (StringComparison(DATA,Compared_DATA[2],1))
	{
		strcat((char*)DATA_OUT,(char*)"B\r\n");
		USART2_DMA_printf(DATA_OUT,11);
	}
	else if (StringComparison(DATA,Compared_DATA[3],1))
	{
		strcat((char*)DATA_OUT,(char*)"F\r\n");
		USART2_DMA_printf(DATA_OUT,11);
	}
	else
	{
		USART1_DMA_printf((char*)"错误");
	}

	//等待BLE102设置完成，返回数据
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	uint8_t DATA_MODE[1]={0};
	DATA_MODE[0]=UART2_receive_buff[8];
	int ret =0;//返回变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}


	if(StringComparison(DATA_MODE,Compared_DATA[0],1))
	{
		BLE102_MODE=0;
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[1],1))
	{
		BLE102_MODE=1;
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[2],1))
	{
		BLE102_MODE=2;
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[3],1))
	{
		BLE102_MODE=3;
	}

	//判断需要设置的参数和现状态是否一致
	if(StringComparison(DATA,Compared_DATA[0],1)&&BLE102_MODE==0)
	{
	ret=1;
	}
	else
	{
	ret=0;
	}

	if(StringComparison(DATA,Compared_DATA[1],1)&&BLE102_MODE==1)
	{
	ret=1;
	}
	else
	{
	ret=0;
	}

	if(StringComparison(DATA,Compared_DATA[2],1)&&BLE102_MODE==2)
	{
	ret=1;
	}
	else
	{
	ret=0;
	}
	
	if(StringComparison(DATA,Compared_DATA[3],1)&&BLE102_MODE==3)
	{
	ret=1;
	}
	else
	{
	ret=0;
	}

	HAL_Delay(500);
	
	USAR2_Interrupt_reload();//重置中断数据缓存

	return ret;
}

/*BLE102指令操作函数*/

//查询BLE102模块的工作模式
void BLE102_READ_MODE(void)
{
	BLE102_judge_AT_MODE();
	char DATA_OUT[10]="AT+MODE?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
	
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
//int BLE102_MODE=1;//BLE102的模式
								//0：主设备模式
								//1：从设备模式
								//2：广播或iBeacon模式
								//3：Mesh组网模式


	uint8_t DATA_MODE[1]={0};
	DATA_MODE[0]=UART2_receive_buff[8];

	uint8_t Compared_DATA[4][1]={{"M"},{"S"},{"B"},{"F"}};//创建对比变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块工作模式:");

	if(StringComparison(DATA_MODE,Compared_DATA[0],1))
	{
		BLE102_MODE=0;
		strcat((char*)DATA_Printf,(char*)"主设备模式\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[1],1))
	{
		BLE102_MODE=1;
		strcat((char*)DATA_Printf,(char*)"从设备模式\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[2],1))
	{
		BLE102_MODE=2;
		strcat((char*)DATA_Printf,(char*)"广播模式\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[3],1))
	{
		BLE102_MODE=3;
		strcat((char*)DATA_Printf,(char*)"Mesh组网模式\r\n");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"错误\r\n");
	}
	
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
	
}

//查询BLE102模块周围从机
void BLE102_READ_SCAN(void)
{
	char DATA_OUT[9]="AT+SCAN\r\n";
	USART2_DMA_printf(DATA_OUT,9);
}

//查询BLE102模块连接状态
void BLE102_READ_LINK(void)
{
	/*
	uint8_t BLE102_LINK_MAC;//已连接设备的地址
	uint8_t BLE102_LINK_Rssi;//已连接设备的信号强度
	int BLE102_LINK_Status;//模块连接状态
														//1：Online：连接成功
														//0：OFFLine：模块未连接
	*/
	char DATA_OUT[10]="AT+LINK?\r\n";
	USART2_DMA_printf(DATA_OUT,10);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	uint8_t DATA_Keyword_Link[6]="Link:";
	uint8_t Stop_bit[1]="\r";		//定义截止符
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_Link,Stop_bit);//截取字符

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	uint8_t Compared_DATA[2][7]={{"OffLine"},{"OnLine"}};//创建对比变量
	strcat((char*)DATA_Printf,(char*)"BLE102模块连接状态:\r\n	是否连接：");

	//判断获取的结果，并转为int类型写入BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],String_Intercept_DATA_return,7)==1)
	{
		BLE102_LINK_Status=0;//写入禁止
		strcat((char*)DATA_Printf,(char*)"OFF\r\n");
	}
	else if(StringComparison(Compared_DATA[1],String_Intercept_DATA_return,6)==1)
	{
		BLE102_LINK_Status=1;//写入使能
		strcat((char*)DATA_Printf,(char*)"ON\r\n");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"错误\r\n");
	}
	/*连接成功后续参数等待连接完成后，逻辑分析仪抓出来再写
	if(BLE102_LINK_Status==1)//若连接成功，获取MAC地址和信号强度Rssi
	{
		uint8_t DATA_Keyword_MAC[6]="MAC:";
		Keyword_String_Intercept(UART2_receive_buff,BLE102_LINK_MAC,DATA_Keyword_Link,Stop_bit);//截取字符
	}*/
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块开机欢迎语
void BLE102_READ_HELLO(void)
{
	char DATA_OUT[11]="AT+HELLO?\r\n";
	USART2_DMA_printf(DATA_OUT,11);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+7;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_HELLO,20);//将截取的字符存储入指定变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块开机欢迎语:");
	strcat((char*)DATA_Printf,(char*)BLE102_HELLO);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存

}

//查询BLE102模块自动睡眠
void BLE102_READ_AUTOSLEEP(void)
{
	char DATA_OUT[15]="AT+AUTOSLEEP?\r\n";
	USART2_DMA_printf(DATA_OUT,15);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[4]={0};

	//先获取是否自动休眠的参数
	uint8_t DATA_Keyword_AUTO[5]="AUTO:";
	uint8_t Stop_bit[1]="\r";		//定义截止符
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_AUTO,Stop_bit);//截取字符

		//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	uint8_t Compared_DATA[2][3]={{"OFF"},{"ON"}};//创建对比变量

	strcat((char*)DATA_Printf,(char*)"BLE102模块自动休眠参数:\r\n	是否自动休眠：");

	//判断获取的结果，并转为int类型写入BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],String_Intercept_DATA_return,3)==1)
	{
		BLE102_AUTOSLEEP_Status=0;//写入禁止
		strcat((char*)DATA_Printf,(char*)"OFF");
	}
	else if(StringComparison(Compared_DATA[1],String_Intercept_DATA_return,2)==1)
	{
		BLE102_AUTOSLEEP_Status=1;//写入使能
		strcat((char*)DATA_Printf,(char*)"ON");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"错误:");
	}
	
	strcat((char*)DATA_Printf,(char*)"\r\n	休眠时间：");
	
	uint8_t DATA_Keyword_TIME[5]="Time:";
	Stop_bit[0]='*';
	
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_TIME,Stop_bit);//截取字符

	BLE102_AUTOSLEEP_Time=(Characters_converted_numbers(String_Intercept_DATA_return))*5;
	char X[3]={0};
	sprintf(X, "%d", BLE102_AUTOSLEEP_Time); 
	strcat((char*)DATA_Printf,(char*)X);
	strcat((char*)DATA_Printf,(char*)" S\r\n");
	
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块串口打包时间
void BLE102_READ_UARTTM(void)
{
	char DATA_OUT[12]="AT+UARTTM?\r\n";
	USART2_DMA_printf(DATA_OUT,12);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[4]={0};

	int Begin_bit=2+8;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,PDATA,4);//将截取的字符存储入指定变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	
	BLE102_UARTTM=Characters_converted_numbers(PDATA);
	
	strcat((char*)DATA_Printf,(char*)"BLE102模块串口打包时间:");
	strcat((char*)DATA_Printf,(char*)PDATA);
	strcat((char*)DATA_Printf,(char*)"0ms\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块串口参数
void BLE102_READ_UART(void)
{
	char DATA_OUT[10]="AT+UART?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
	
	uint8_t DATA_long[50]={0};//用于存放57600，8，0，0数据

	while(UART2_Rx_flg == 0)//等待数据
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+6;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,DATA_long,50);//将截取的字符存储入指定变量
	
	int UART_DATA_SET=0;
	int UART_DATA_BIT=0;

	for(int i=0;i<50;i++)
	{
		if(DATA_long[i] == 0x2C)//若检测到逗号（0x2c）则SET（组）+1
		{
			UART_DATA_SET++;
			UART_DATA_BIT=0;
		}
		else
		{
		BLE102_UART_DATA[UART_DATA_SET][UART_DATA_BIT]=DATA_long[i];
		UART_DATA_BIT++;
		}
		if(DATA_long[i] == 0x00)
		{
			break;
		}
	}

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块串口参数:\r\n");
	strcat((char*)DATA_Printf,(char*)"	波特率:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[0]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	数据位:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[1]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	校验位:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[2]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	停止位:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[3]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块是否使能连接验证
void BLE102_READ_PASSEN(void)
{
	char DATA_OUT[12]="AT+PASSEN?\r\n";
	USART2_DMA_printf(DATA_OUT,12);

		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[3]={0};

	int Begin_bit=2+8;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,PDATA,3);//将截取的字符存储入指定变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	uint8_t Compared_DATA[2][3]={{"OFF"},{"ON"}};//创建对比变量

	strcat((char*)DATA_Printf,(char*)"BLE102模块是否使能连接验证:");

	//判断获取的结果，并转为int类型写入BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],PDATA,3)==1)
	{
		BLE102_PASSEN=0;//写入禁止
		strcat((char*)DATA_Printf,(char*)"OFF");
	}
	else if(StringComparison(Compared_DATA[1],PDATA,2)==1)
	{
		BLE102_PASSEN=1;//写入禁止
		strcat((char*)DATA_Printf,(char*)"ON");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"错误:");
	}
	
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
	
}

//查询BLE102模块密码
void BLE102_READ_PASS(void)
{
	char DATA_OUT[10]="AT+PASS?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+6;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_PASS,6);//将截取的字符存储入指定变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块密码:");
	strcat((char*)DATA_Printf,(char*)BLE102_PASS);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块发射功率
void BLE102_READ_TPL(void)
{
	char DATA_OUT[11]="AT+TPL?\r\n";
	USART2_DMA_printf(DATA_OUT,11);
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	int Begin_bit=2+10;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_TPL,1);//将截取的字符存储入指定变量
	
	int X=(int)BLE102_TPL[0]-48;
	
	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	Transfer_character(BLE102_TPL_dbm[X-1],String_Intercept_DATA_return,6);//将截取的字符存储入指定变量
	
	strcat((char*)DATA_Printf,(char*)"BLE102模块发射功率:");
	strcat((char*)DATA_Printf,(char*)String_Intercept_DATA_return);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块软件版本号
void BLE102_READ_CIVER(void)
{
	char DATA_OUT[11]="AT+CIVER?\r\n";
	USART2_DMA_printf(DATA_OUT,11);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+5;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_CIVER,20);//将截取的字符存储入指定变量

	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块软件版本号:");
	strcat((char*)DATA_Printf,(char*)BLE102_CIVER);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//重置中断数据缓存
}

//查询BLE102模块名称
void BLE102_READ_NAME(void)
{
	char DATA_OUT[10]="AT+NAME?\r\n";
	USART2_DMA_printf(DATA_OUT,9);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	int Begin_bit=2+6;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符
	
	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_NAME,15);//将截取的字符存储入指定变量
	
	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块的名称：");
	strcat((char*)DATA_Printf,(char*)BLE102_NAME);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存
	
}

//查询BLE102模块MAC地址
void BLE102_READ_MAC(void)
{
	BLE102_judge_AT_MODE();//检查AT指令模式
	char DATA_OUT[9]="AT+MAC?\r\n";
	USART2_DMA_printf(DATA_OUT,9);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+5;					//定义开始位置
	uint8_t Stop_bit[1]="\r";		//定义截止符
	
	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//截取字符
	Transfer_character(String_Intercept_DATA_return,BLE102_MAC,12);//将截取的字符存储入指定变量
	
	//清空输出变量
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102模块MAC地址：");
	strcat((char*)DATA_Printf,(char*)BLE102_MAC);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存

}

void BLE102_Init(void)
{
	printf("BLE102初始化： \r\n");
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

	BLE102_REST();//复位BLE102模块
	BLE102_AT_instruction();//进入指令模式
	BLE102_READ_NAME();//查询模块名称
	BLE102_READ_MAC();//查询模块MAC地址
	BLE102_READ_CIVER();//查询模块软件版本号
	BLE102_READ_TPL();//查询模块发射功率//疑似查询之后需要重新进AT
	BLE102_READ_PASS();//查询模块密码
	//BLE102_READ_PASSEN();//查询BLE102模块是否使能连接验证
	BLE102_READ_UART();//查询BLE102模块串口参数
	//BLE102_READ_UARTTM();//查询BLE102模块串口打包时间
	BLE102_READ_AUTOSLEEP();//查询BLE102模块自动睡眠参数
	BLE102_READ_HELLO();////查询BLE102模块开机欢迎语
	BLE102_READ_LINK();//查询BLE102模块连接状态
	//BLE102_READ_SCAN();//查询BLE102模块周围从机
	BLE102_READ_MODE();//查询BLE102模块的工作模式



	uint8_t MODE_bit[1]="S";
	BLE102_write_MODE(MODE_bit);//设置BLE102模块的工作模式
	BLE102_READ_MODE();//查询BLE102模块的工作模式

	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
