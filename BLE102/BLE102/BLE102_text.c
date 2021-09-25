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
  * @brief  ͨ���ؼ��ֿ�ʼ��ȡһ���ı�������
  * @param	DATA:ԭʼ����
  * @param	pDATA���������ַ���
  * @param	Keyword���ؼ���
  * @param	DATA_STOP������ֹͣ�ַ�
  * @retval ��ֵ
  */
void Keyword_String_Intercept(uint8_t* DATA,uint8_t* PDATA,uint8_t* Keyword,uint8_t* DATA_STOP)
{
	int DATA_Zero=0;//���ݿ�ʼ��ȡ���
	
	/*��ʼѰ�ҹؼ��֣���λ���*/
	int Keyword_long=strlen((char*)Keyword);//����ؼ��ֳ���
	int DATA_long=strlen((char*)DATA);			//����ԭʼ���ݳ���
	int K_IF_LONG=0;//�Ѿ��жϳɹ����ַ�

	for(int i=0;i<DATA_long;i++)//��������ԭʼ����
	{
	if(Keyword[0]==DATA[i])			//�жϹؼ��ֵĵ�һ���ַ��Ƿ���ԭʼ���ݶ�Ӧ
	{
		for(int B=0;B<Keyword_long;B++)
	{
		if(Keyword[B]==DATA[i+B])
		{
			K_IF_LONG++;
		}//�ؼ��ֵ����жϽ���
		else
		{
		K_IF_LONG=0;
		}
	}//ȫ���ؼ����жϽ���

	}//��һ�ؼ����жϽ���
		if(K_IF_LONG==Keyword_long)
		{
		DATA_Zero=i+Keyword_long+1;
		break;
		}
	}//�������
	//�����Ѿ��õ��������
	
	int DATA_STOP_bit=0;//���ݵĽ�ȡ�Ľ�ֹ��
	for(int i=DATA_long-DATA_Zero;i<DATA_long;i++)
	{
		if(DATA[i]==DATA_STOP[0])
		{
			DATA_STOP_bit=i;
			break;
		}
	}
	//���ˣ��Ѿ��õ����ݵ���ʼ�ͽ�ֹ��
	
	/*����������ڵ����ݸ�ֵ��pDATA*/
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
* @brief  ��uint8_tתΪint
* @param	DATA:ԭʼ����
* @retval DATA_Return��ת��Ϊint���͵�����
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
* @brief  �����ַ�������
* @param	DATA:ԭʼ����
* @param	pDATA:����ֵ������
* @param	Bit_long����ֵ����
* @retval ��ֵ
*/
void Transfer_character(uint8_t* DATA,uint8_t* PDATA,int Bit_long)
{
	for(int i=0;i<Bit_long;i++)
	{
	PDATA[i]=DATA[i];
	}
}

/**
* @brief  �ж������ַ����Ƿ�һ��
* @param	DATA:ԭʼ����
* @param	pDATA:�ж�����
* @param	NUM����Ҫ�ж����ݵĳ���
* @retval ��ֵ
*/
int StringComparison(uint8_t* DATA,uint8_t* pDATA,int NUM)
{
	int RE_NUM = 1;//����ֵ 
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
* @brief  ��ȡ�ַ������ݺ���
* @param	DATA:ԭʼ����
* @param	DATA_begin_long:������ʼλ��
* @param	DATA_STOP������ֹͣ�ַ�
* @retval ��ֵ
*/
//extern uint8_t String_Intercept_DATA_return[255];//��������
//extern int String_Intercept_DATA_long;//���ݳ���
/*
void String_Intercept(uint8_t DATA[255],int DATA_begin_long,uint8_t DATA_STOP[1])
{	
	
	//������ת�Ƶ�pDATA
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