#ifndef __BLE102_text_H__
#define __BLE102_text_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BLE102_text.h"
#include "usart.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "stdarg.h"
#include "math.h"



/**
  * @brief  通过关键字开始截取一段文本的内容
  * @param	DATA:原始数据
  * @param	pDATA：被复制字符串
  * @param	Keyword：关键字
  * @param	DATA_STOP：数据停止字符
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

 /**
* @brief  将uint8_t转为int
* @param	DATA:原始数据
* @retval DATA_Return：转化为int类型的数据
*/
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
* @param	DATA:原始数据
* @param	pDATA:被赋值的数据
* @param	Bit_long：赋值长度
* @retval 空值
*/
void Transfer_character(uint8_t* DATA,uint8_t* PDATA,int Bit_long)
{
	for(int i=0;i<Bit_long;i++)
	{
	PDATA[i]=DATA[i];
	}
}

/**
* @brief  判断两个字符串是否一致
* @param	DATA:原始数据
* @param	pDATA:判断数据
* @param	NUM：需要判断数据的长度
* @retval 空值
*/
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


/**
* @brief  截取字符串内容函数
* @param	DATA:原始数据
* @param	DATA_begin_long:数据起始位置
* @param	DATA_STOP：数据停止字符
* @retval 空值
*/
//extern uint8_t String_Intercept_DATA_return[255];//返回数据
//extern int String_Intercept_DATA_long;//数据长度
/*
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
*/



#ifdef __cplusplus
}
#endif

#endif /* __BLE102_text_H__ */