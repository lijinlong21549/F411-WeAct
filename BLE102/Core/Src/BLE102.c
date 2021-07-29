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

//���������жϱ���
extern uint8_t UART1_TX_DMA_OVER;							//USART1������ɱ�־
extern uint8_t UART2_TX_DMA_OVER;							//USART2������ɱ�־
extern uint8_t UART1_Rx_flg;                   //USART1������ɱ�־
extern uint8_t UART2_Rx_flg;                   //USART2������ɱ�־
extern uint8_t UART1_receive_buff[255];						//����1�Ļ������
extern uint8_t UART2_receive_buff[255];						//����2�Ļ������
extern int UART1_data_length;											//����1�����ݳ���
extern int UART2_data_length;											//����2�����ݳ���
/*�ַ�����ȡ��������*/
uint8_t String_Intercept_DATA_return[255]={0};//��������
int String_Intercept_DATA_long=0;//���ݳ���
/*DMA���ͻ���*/
uint8_t DATA_Printf[255]={0};
/*Ӳ����궨��*/
/********************************************************************/
int BLE102_AT_command_MODE=0;//��������ģʽ

/*BLE102״̬����*/
int BLE102_AT_MODE=0;//BLE102��ATָ��ģʽ״̬
					//0������ATָ��ģʽ
					//1����ATָ��ģʽ
uint8_t BLE102_NAME[15];//BLE102������
int BLE102_MODE=1;//BLE102��ģʽ
								//0�����豸ģʽ
								//1�����豸ģʽ
								//2���㲥��iBeaconģʽ
								//3��Mesh����ģʽ
uint8_t BLE102_MAC[12];//BLE102��MAC��ַ
uint8_t BLE102_CIVER[20];//BLE102������汾��
uint8_t BLE102_TPL[1];//BLE102��ģ�鷢�书��
uint8_t BLE102_TPL_dbm[8][6]={{"-14dbm"},{"-11dbm"},{"-8dbm"},{"-5dbm"},{"-2dbm"},{"+2dbm"},{"+4dbm"},{"+8dbm"}};//BLE102ģ�鷢��ȼ�����Ӧ�Ĺ���
int	BLE102_UARTTM;//BLE102��ģ�鴮�ڴ��ʱ��
uint8_t BLE102_PASS[6+1]={0};//BLE102ģ�������
int BLE102_PASSEN;//BLE102�Ƿ�ʹ��������֤
								 //1��ʹ��
								 //0����ֹ
uint8_t BLE102_HELLO[20];//������ӭ��
uint8_t BLE102_CONNADD[12];//�豸�ϵ�Ĭ������ģ���MAC��ַ
int BLE102_ADP;//ģ���Ƿ���й㲥
								//0���رչ㲥
								//1�������㲥
int BLE102_ADPTIM;//ģ����Զ���㲥�ٶ� 10-1024
int BLE102_AUTOCONN;//�Ƿ�ʹ���Զ���������
										//0����ֹ
										//1��ʹ��
int BLE102_MAXPUT=1;//�Ƿ�ʹ��������
										//0����ֹ
										//1��ʹ��
int BLE102_TRENC;//�Ƿ�ʹ�����ݼ��ܴ���
										//0����ֹ
										//1��ʹ��
/*ģ���Զ�˯�߲���*/
int BLE102_AUTOSLEEP_Status;//�Ƿ��Զ�˯��
														//0����ֹ
														//1��ʹ��
int BLE102_AUTOSLEEP_Time=5;//�Զ�˯�ߵȴ�ʱ�� 0-100

/*ģ�鴮�ڲ���*/
uint8_t BLE102_UART_DATA[4][10]={0};
//[0]������
//[1]����λ
//[2]У��λ
	//0����У��
	//1����У��
	//2��żУ��
//[3]ֹͣλ
	//0��1ֹͣλ
	//1��2ֹͣλ
/*ģ������״̬*/
uint8_t BLE102_LINK_MAC[12];//�������豸�ĵ�ַ
uint8_t BLE102_LINK_Rssi;//�������豸���ź�ǿ��
int BLE102_LINK_Status;//ģ������״̬
														//1��Online�����ӳɹ�
														//0��OFFLine��ģ��δ����
/*��Χ�ӻ�����*/
uint8_t BLE102_SCAN_MAC[20][12];//��Χ�ӻ��������ź�MAC��ַ����
uint8_t BLE102_SCAN_Size[20][10];//��Χ�ӻ��������ź��ź�ǿ�ȶ���
/*iBeacon���ܲ��������������ݶ�20��*/
uint8_t BLE102_IBEACON_Uuid[20];//iBEACON�豸ʶ����
uint8_t BLE102_IBEACON_Major[20];//major���
uint8_t BLE102_IBEACON_Minor[20];//minor���
uint8_t BLE102_IBEACON_Rssi[20];//�ź�ǿ��

/**GPIO����ָ��*/
#define BLE102_RES_Enable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_RESET)  //�͵�ƽ
#define BLE102_RES_Disable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_SET)	   //�ߵ�ƽ


/*BLE102ģʽ��������*/

//��λBLE102ģ��
void BLE102_REST(void)
{
	printf("��λBLE102ģ�飺");
	BLE102_RES_Enable();
	HAL_Delay(300);
	BLE102_RES_Disable();
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //���ͽ��յ�������
		USAR2_Interrupt_reload();//�����ж����ݻ���
	} 
	HAL_Delay(500);
	printf("\r\n");
	
}

//����ָ��ģʽ
void BLE102_AT_instruction(void)
{
	printf("��ʼ����ATָ��ģʽ\r\n");
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
		USAR2_Interrupt_reload();//�����ж����ݻ���
		HAL_UART_Transmit(&huart2,pDATA,1,0x10);
	}
	else
	{
		BLE102_AT_MODE=0;
		printf("����ATָ��ģʽ���ִ��󣡣�\r\n");
	}

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	uint8_t nDATA[5]="+ok\r\n";
	if(UART2_Rx_flg && StringComparison(UART2_receive_buff,nDATA,5)==1)
	{

		USAR2_Interrupt_reload();//�����ж����ݻ���
		BLE102_AT_MODE=1;
		printf("����ATָ��ģʽ\r\n");
	}
	else
	{
		BLE102_AT_MODE=0;
		printf("����ATָ��ģʽ���ִ��󣡣�\r\n");
	}
}

//�ж��Ƿ���ָ��ģʽ������ָ��ִ��ǰ���
void BLE102_judge_AT_MODE(void)
{
	if(BLE102_AT_MODE==0)
	{
		BLE102_AT_instruction();
	}
}

/**
* @brief  ����BLE102ģ��Ĺ���ģʽ
* @param	DATA:����ģʽ
				M:���豸ģʽ
				S:���豸ģʽ
				B:�㲥��iBeaconģʽ
				F:Mesh����ģʽ
* @retval DATA_Return��
											�ɹ�����1
											ʧ�ܷ���0
*/
int BLE102_write_MODE(uint8_t* DATA)
{
	BLE102_judge_AT_MODE();

	uint8_t Compared_DATA[4][1]={{"M"},{"S"},{"B"},{"F"}};//�����Աȱ���
	
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
		USART1_DMA_printf((char*)"����");
	}

	//�ȴ�BLE102������ɣ���������
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	uint8_t DATA_MODE[1]={0};
	DATA_MODE[0]=UART2_receive_buff[8];
	int ret =0;//���ر���

	//����������
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

	//�ж���Ҫ���õĲ�������״̬�Ƿ�һ��
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
	
	USAR2_Interrupt_reload();//�����ж����ݻ���

	return ret;
}

/*BLE102ָ���������*/

//��ѯBLE102ģ��Ĺ���ģʽ
void BLE102_READ_MODE(void)
{
	BLE102_judge_AT_MODE();
	char DATA_OUT[10]="AT+MODE?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
	
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
//int BLE102_MODE=1;//BLE102��ģʽ
								//0�����豸ģʽ
								//1�����豸ģʽ
								//2���㲥��iBeaconģʽ
								//3��Mesh����ģʽ


	uint8_t DATA_MODE[1]={0};
	DATA_MODE[0]=UART2_receive_buff[8];

	uint8_t Compared_DATA[4][1]={{"M"},{"S"},{"B"},{"F"}};//�����Աȱ���

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ�鹤��ģʽ:");

	if(StringComparison(DATA_MODE,Compared_DATA[0],1))
	{
		BLE102_MODE=0;
		strcat((char*)DATA_Printf,(char*)"���豸ģʽ\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[1],1))
	{
		BLE102_MODE=1;
		strcat((char*)DATA_Printf,(char*)"���豸ģʽ\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[2],1))
	{
		BLE102_MODE=2;
		strcat((char*)DATA_Printf,(char*)"�㲥ģʽ\r\n");
	}
	else if (StringComparison(DATA_MODE,Compared_DATA[3],1))
	{
		BLE102_MODE=3;
		strcat((char*)DATA_Printf,(char*)"Mesh����ģʽ\r\n");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"����\r\n");
	}
	
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
	
}

//��ѯBLE102ģ����Χ�ӻ�
void BLE102_READ_SCAN(void)
{
	char DATA_OUT[9]="AT+SCAN\r\n";
	USART2_DMA_printf(DATA_OUT,9);
}

//��ѯBLE102ģ������״̬
void BLE102_READ_LINK(void)
{
	/*
	uint8_t BLE102_LINK_MAC;//�������豸�ĵ�ַ
	uint8_t BLE102_LINK_Rssi;//�������豸���ź�ǿ��
	int BLE102_LINK_Status;//ģ������״̬
														//1��Online�����ӳɹ�
														//0��OFFLine��ģ��δ����
	*/
	char DATA_OUT[10]="AT+LINK?\r\n";
	USART2_DMA_printf(DATA_OUT,10);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	uint8_t DATA_Keyword_Link[6]="Link:";
	uint8_t Stop_bit[1]="\r";		//�����ֹ��
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_Link,Stop_bit);//��ȡ�ַ�

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	uint8_t Compared_DATA[2][7]={{"OffLine"},{"OnLine"}};//�����Աȱ���
	strcat((char*)DATA_Printf,(char*)"BLE102ģ������״̬:\r\n	�Ƿ����ӣ�");

	//�жϻ�ȡ�Ľ������תΪint����д��BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],String_Intercept_DATA_return,7)==1)
	{
		BLE102_LINK_Status=0;//д���ֹ
		strcat((char*)DATA_Printf,(char*)"OFF\r\n");
	}
	else if(StringComparison(Compared_DATA[1],String_Intercept_DATA_return,6)==1)
	{
		BLE102_LINK_Status=1;//д��ʹ��
		strcat((char*)DATA_Printf,(char*)"ON\r\n");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"����\r\n");
	}
	/*���ӳɹ����������ȴ�������ɺ��߼�������ץ������д
	if(BLE102_LINK_Status==1)//�����ӳɹ�����ȡMAC��ַ���ź�ǿ��Rssi
	{
		uint8_t DATA_Keyword_MAC[6]="MAC:";
		Keyword_String_Intercept(UART2_receive_buff,BLE102_LINK_MAC,DATA_Keyword_Link,Stop_bit);//��ȡ�ַ�
	}*/
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ�鿪����ӭ��
void BLE102_READ_HELLO(void)
{
	char DATA_OUT[11]="AT+HELLO?\r\n";
	USART2_DMA_printf(DATA_OUT,11);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+7;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_HELLO,20);//����ȡ���ַ��洢��ָ������

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ�鿪����ӭ��:");
	strcat((char*)DATA_Printf,(char*)BLE102_HELLO);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���

}

//��ѯBLE102ģ���Զ�˯��
void BLE102_READ_AUTOSLEEP(void)
{
	char DATA_OUT[15]="AT+AUTOSLEEP?\r\n";
	USART2_DMA_printf(DATA_OUT,15);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[4]={0};

	//�Ȼ�ȡ�Ƿ��Զ����ߵĲ���
	uint8_t DATA_Keyword_AUTO[5]="AUTO:";
	uint8_t Stop_bit[1]="\r";		//�����ֹ��
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_AUTO,Stop_bit);//��ȡ�ַ�

		//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	uint8_t Compared_DATA[2][3]={{"OFF"},{"ON"}};//�����Աȱ���

	strcat((char*)DATA_Printf,(char*)"BLE102ģ���Զ����߲���:\r\n	�Ƿ��Զ����ߣ�");

	//�жϻ�ȡ�Ľ������תΪint����д��BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],String_Intercept_DATA_return,3)==1)
	{
		BLE102_AUTOSLEEP_Status=0;//д���ֹ
		strcat((char*)DATA_Printf,(char*)"OFF");
	}
	else if(StringComparison(Compared_DATA[1],String_Intercept_DATA_return,2)==1)
	{
		BLE102_AUTOSLEEP_Status=1;//д��ʹ��
		strcat((char*)DATA_Printf,(char*)"ON");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"����:");
	}
	
	strcat((char*)DATA_Printf,(char*)"\r\n	����ʱ�䣺");
	
	uint8_t DATA_Keyword_TIME[5]="Time:";
	Stop_bit[0]='*';
	
	Keyword_String_Intercept(UART2_receive_buff,String_Intercept_DATA_return,DATA_Keyword_TIME,Stop_bit);//��ȡ�ַ�

	BLE102_AUTOSLEEP_Time=(Characters_converted_numbers(String_Intercept_DATA_return))*5;
	char X[3]={0};
	sprintf(X, "%d", BLE102_AUTOSLEEP_Time); 
	strcat((char*)DATA_Printf,(char*)X);
	strcat((char*)DATA_Printf,(char*)" S\r\n");
	
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ�鴮�ڴ��ʱ��
void BLE102_READ_UARTTM(void)
{
	char DATA_OUT[12]="AT+UARTTM?\r\n";
	USART2_DMA_printf(DATA_OUT,12);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[4]={0};

	int Begin_bit=2+8;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,PDATA,4);//����ȡ���ַ��洢��ָ������

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	
	BLE102_UARTTM=Characters_converted_numbers(PDATA);
	
	strcat((char*)DATA_Printf,(char*)"BLE102ģ�鴮�ڴ��ʱ��:");
	strcat((char*)DATA_Printf,(char*)PDATA);
	strcat((char*)DATA_Printf,(char*)"0ms\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ�鴮�ڲ���
void BLE102_READ_UART(void)
{
	char DATA_OUT[10]="AT+UART?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
	
	uint8_t DATA_long[50]={0};//���ڴ��57600��8��0��0����

	while(UART2_Rx_flg == 0)//�ȴ�����
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+6;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,DATA_long,50);//����ȡ���ַ��洢��ָ������
	
	int UART_DATA_SET=0;
	int UART_DATA_BIT=0;

	for(int i=0;i<50;i++)
	{
		if(DATA_long[i] == 0x2C)//����⵽���ţ�0x2c����SET���飩+1
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

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ�鴮�ڲ���:\r\n");
	strcat((char*)DATA_Printf,(char*)"	������:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[0]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	����λ:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[1]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	У��λ:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[2]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	strcat((char*)DATA_Printf,(char*)"	ֹͣλ:");
	strcat((char*)DATA_Printf,(char*)BLE102_UART_DATA[3]);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ���Ƿ�ʹ��������֤
void BLE102_READ_PASSEN(void)
{
	char DATA_OUT[12]="AT+PASSEN?\r\n";
	USART2_DMA_printf(DATA_OUT,12);

		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	uint8_t PDATA[3]={0};

	int Begin_bit=2+8;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,PDATA,3);//����ȡ���ַ��洢��ָ������

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	uint8_t Compared_DATA[2][3]={{"OFF"},{"ON"}};//�����Աȱ���

	strcat((char*)DATA_Printf,(char*)"BLE102ģ���Ƿ�ʹ��������֤:");

	//�жϻ�ȡ�Ľ������תΪint����д��BLE102_PASSEN
	if(StringComparison(Compared_DATA[0],PDATA,3)==1)
	{
		BLE102_PASSEN=0;//д���ֹ
		strcat((char*)DATA_Printf,(char*)"OFF");
	}
	else if(StringComparison(Compared_DATA[1],PDATA,2)==1)
	{
		BLE102_PASSEN=1;//д���ֹ
		strcat((char*)DATA_Printf,(char*)"ON");
	}
	else
	{
		strcat((char*)DATA_Printf,(char*)"����:");
	}
	
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
	
}

//��ѯBLE102ģ������
void BLE102_READ_PASS(void)
{
	char DATA_OUT[10]="AT+PASS?\r\n";
	USART2_DMA_printf(DATA_OUT,10);
		while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+6;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_PASS,6);//����ȡ���ַ��洢��ָ������

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ������:");
	strcat((char*)DATA_Printf,(char*)BLE102_PASS);
	strcat((char*)DATA_Printf,(char*)"\r\n");

	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ�鷢�书��
void BLE102_READ_TPL(void)
{
	char DATA_OUT[11]="AT+TPL?\r\n";
	USART2_DMA_printf(DATA_OUT,11);
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	int Begin_bit=2+10;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_TPL,1);//����ȡ���ַ��洢��ָ������
	
	int X=(int)BLE102_TPL[0]-48;
	
	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}
	Transfer_character(BLE102_TPL_dbm[X-1],String_Intercept_DATA_return,6);//����ȡ���ַ��洢��ָ������
	
	strcat((char*)DATA_Printf,(char*)"BLE102ģ�鷢�书��:");
	strcat((char*)DATA_Printf,(char*)String_Intercept_DATA_return);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ������汾��
void BLE102_READ_CIVER(void)
{
	char DATA_OUT[11]="AT+CIVER?\r\n";
	USART2_DMA_printf(DATA_OUT,11);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+5;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��

	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_CIVER,20);//����ȡ���ַ��洢��ָ������

	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ������汾��:");
	strcat((char*)DATA_Printf,(char*)BLE102_CIVER);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);

	USAR2_Interrupt_reload();//�����ж����ݻ���
}

//��ѯBLE102ģ������
void BLE102_READ_NAME(void)
{
	char DATA_OUT[10]="AT+NAME?\r\n";
	USART2_DMA_printf(DATA_OUT,9);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
	
	int Begin_bit=2+6;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��
	
	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_NAME,15);//����ȡ���ַ��洢��ָ������
	
	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ������ƣ�");
	strcat((char*)DATA_Printf,(char*)BLE102_NAME);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���
	
}

//��ѯBLE102ģ��MAC��ַ
void BLE102_READ_MAC(void)
{
	BLE102_judge_AT_MODE();//���ATָ��ģʽ
	char DATA_OUT[9]="AT+MAC?\r\n";
	USART2_DMA_printf(DATA_OUT,9);

	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}

	int Begin_bit=2+5;					//���忪ʼλ��
	uint8_t Stop_bit[1]="\r";		//�����ֹ��
	
	String_Intercept(UART2_receive_buff,Begin_bit,Stop_bit);//��ȡ�ַ�
	Transfer_character(String_Intercept_DATA_return,BLE102_MAC,12);//����ȡ���ַ��洢��ָ������
	
	//����������
	for(int i=0;i<255;i++)
	{
	DATA_Printf[i]=0x00;
	}

	strcat((char*)DATA_Printf,(char*)"BLE102ģ��MAC��ַ��");
	strcat((char*)DATA_Printf,(char*)BLE102_MAC);
	strcat((char*)DATA_Printf,(char*)"\r\n");
	USART1_DMA_printf((char*)DATA_Printf);
	USAR2_Interrupt_reload();//�����ж����ݻ���

}

void BLE102_Init(void)
{
	printf("BLE102��ʼ���� \r\n");
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

	BLE102_REST();//��λBLE102ģ��
	BLE102_AT_instruction();//����ָ��ģʽ
	BLE102_READ_NAME();//��ѯģ������
	BLE102_READ_MAC();//��ѯģ��MAC��ַ
	BLE102_READ_CIVER();//��ѯģ������汾��
	BLE102_READ_TPL();//��ѯģ�鷢�书��//���Ʋ�ѯ֮����Ҫ���½�AT
	BLE102_READ_PASS();//��ѯģ������
	//BLE102_READ_PASSEN();//��ѯBLE102ģ���Ƿ�ʹ��������֤
	BLE102_READ_UART();//��ѯBLE102ģ�鴮�ڲ���
	//BLE102_READ_UARTTM();//��ѯBLE102ģ�鴮�ڴ��ʱ��
	BLE102_READ_AUTOSLEEP();//��ѯBLE102ģ���Զ�˯�߲���
	BLE102_READ_HELLO();////��ѯBLE102ģ�鿪����ӭ��
	BLE102_READ_LINK();//��ѯBLE102ģ������״̬
	//BLE102_READ_SCAN();//��ѯBLE102ģ����Χ�ӻ�
	BLE102_READ_MODE();//��ѯBLE102ģ��Ĺ���ģʽ



	uint8_t MODE_bit[1]="S";
	BLE102_write_MODE(MODE_bit);//����BLE102ģ��Ĺ���ģʽ
	BLE102_READ_MODE();//��ѯBLE102ģ��Ĺ���ģʽ

	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//�����жϣ���ATָ���������֮��

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
