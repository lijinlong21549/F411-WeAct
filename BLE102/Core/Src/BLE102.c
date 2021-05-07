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


/**
  * @brief  通过关键字开始截取一段文本的内容
  * @param  参数：
								DATA:原始数据
								pDATA：被复制字符串
								Keyword：关键字
								DATA_STOP：数据停止字符
  * @retval 空值
  */
void Keyword_String_Intercept(uint8_t* DATA,uint8_t* PDATA,uint8_t* Keyword,uint8_t* DATA_STOP)
{
	int DATA_Zero=0;//数据开始截取起点
	
	/*开始寻找关键字，定位起电*/
	int Keyword_long=strlen((char*)Keyword);//定义关键字长度
	int DATA_long=strlen((char*)DATA);			//定义原始数据长度
	int K_IF_LONG=0;//已经判断成功的字符

	for(int i=0;i<DATA_long;i++)//历遍整个原始数据
	{
	if(Keyword[0]==DATA[i])			//判断关键字的第一个字符是否与原始数据对应
	{
		for(int B=0;B<Keyword_long;B++)
	{
		if(Keyword[B]==DATA[i+B])
		{
			K_IF_LONG++;
		}//关键字单个判断结束
		else
		{
		K_IF_LONG=0;
		}
	}//全部关键字判断结束

	}//第一关键字判断结束
		if(K_IF_LONG==Keyword_long)
		{
		DATA_Zero=i+Keyword_long+1;
		break;
		}
	}//历遍结束
	//到此已经拿到数据起点
	
	int DATA_STOP_bit=0;//数据的截取的截止点
	for(int i=DATA_long-DATA_Zero;i<DATA_long;i++)
	{
		if(DATA[i]==DATA_STOP[0])
		{
			DATA_STOP_bit=i;
			break;
		}
	}
	//至此，已经拿到数据的起始和截止点
	
	/*将这个区间内的数据赋值给pDATA*/
	for(int i=0;i<DATA_long;i++)
	{
	PDATA[i]=0x00;
	}

	for(int i=0;i<DATA_STOP_bit-DATA_Zero+1;i++)
	{
		PDATA[i]=DATA[DATA_Zero+i-1];
	}


}

//将uint8_t转为int
int Characters_converted_numbers(uint8_t* DATA)
{
	int DATA_LONG=strlen((char*)DATA);
	int DATA_Return=0;

	for(int i=0;i<DATA_LONG;i++)
	{
	for(int B=0;B<9;B++)
	{

			if(DATA[i]==(0x30+B))//0
	{
		DATA_Return=DATA_Return+B*(pow(10,DATA_LONG-i-1));
	}

	}
	}
return DATA_Return;
}

/**
  * @brief  复制字符串内容
  * @param  参数：
								DATA:原始数据
								pDATA：被复制字符串
								Bit_long：复制字符串长度
  * @retval 空值
  */
void Transfer_character(uint8_t* DATA,uint8_t* PDATA,int Bit_long)
{
	for(int i=0;i<Bit_long;i++)
	{
	PDATA[i]=DATA[i];
	}
}

//判断两个字符串是否一致
int StringComparison(uint8_t* DATA,uint8_t* pDATA,int NUM)
{
	int RE_NUM = 1;//返回值 
	for(int i=0;i<NUM;i++)
	{
		if(DATA[i]==pDATA[i])
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

//截取字符串内容函数
/**
  * @brief  截取字符串内容函数
  * @param  参数：
								DATA:原始数据
								DATA_begin_long：数据起始位置
								DATA_STOP：数据停止字符
  * @retval 空值
  */
void String_Intercept(uint8_t DATA[255],int DATA_begin_long,uint8_t DATA_STOP[1])
{	
	
	//将数据转移到pDATA
	uint8_t pDATA[255];
	for(int i=1;i<255;i++)
	{
	pDATA[i] = DATA[i];
	}

	for(int i=1;i<255;i++)
	{
	String_Intercept_DATA_return[i] =0x00;
	}
	String_Intercept_DATA_long=0;

	for(int B=DATA_begin_long;B<255;B++)
	{
		if((pDATA[B]==DATA_STOP[0])==0)
		{
			String_Intercept_DATA_return[String_Intercept_DATA_long] = pDATA[B];
			String_Intercept_DATA_long++;
		}
		else{break;}
	}
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
	uint8_t DATA_Keyword[5]="AUTO:";
	uint8_t Stop_bit[1]="\r";		//定义截止符
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword,Stop_bit);
	/*int BLE102_AUTOSLEEP_Status;//是否自动睡眠
														//0：禁止
														//1：使能
		int BLE102_AUTOSLEEP_Time=5;//自动睡眠等待时间 0-100*/
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
	
	strcat((char*)DATA_Printf,(char*)"\r\n");
	
	//代码续：已经写完是否自动休眠，之后要判断自动休眠时间，注意，AT指令返回有些奇葩，截止位是“ * ”


	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//重置中断数据缓存



	/*
	uint8_t DATA_OUT[15]="AT+AUTOSLEEP?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,15,0x00ff);
	uint8_t DATA[50];
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-0;i++)
		{
			
			//if(UART2_Rx_Buf[i+2]==0x0A || UART2_Rx_Buf[i+2]==0x0D)
			//{
			//	break;
			//}
			DATA[i]=UART2_Rx_Buf[i+0];
		}
		printf("BLE102模块自动睡眠参数:");
		HAL_UART_Transmit(&huart1,DATA,UART2_Rx_cnt-0,0x10);    //发送接收到的数据
		printf("\r\n");
		UART2_INT_REST();//重置中断数据缓存
	} 
	*/
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
		printf("进入AT指令模式\r\n");
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
	BLE102_READ_TPL();//查询模块发射功率//疑似查询之后需要重新进AT
	BLE102_READ_PASS();//查询模块密码
	//BLE102_READ_PASSEN();//查询BLE102模块是否使能连接验证
	BLE102_READ_UART();//查询BLE102模块串口参数
	//BLE102_READ_UARTTM();//查询BLE102模块串口打包时间
	BLE102_READ_AUTOSLEEP();//查询BLE102模块自动睡眠参数

	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
