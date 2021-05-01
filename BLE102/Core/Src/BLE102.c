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

/*Ӳ����궨��*/
/********************************************************************/
int BLE102_AT_command_MODE=0;//��������ģʽ

/*BLE102״̬����*/
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
uint8_t	BLE102_UARTTM[5];//BLE102��ģ�鴮�ڴ��ʱ��
uint8_t BLE102_PASS[6];//BLE102ģ�������
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
uint8_t BLE102_LINK_MAC;//�������豸�ĵ�ַ
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
/********************************************************************/
#define BLE102_RES_Enable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_RESET)  //�͵�ƽ
#define BLE102_RES_Disable() 			HAL_GPIO_WritePin(BLE102_RES_GPIO_Port, BLE102_RES_Pin, GPIO_PIN_SET)	   //�ߵ�ƽ

//�ж������ַ����Ƿ�һ��
int StringComparison(uint8_t* DATA,uint8_t* pDATA,int NUM)
{
	int RE_NUM = 1;//����ֵ 
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

//��ѯBLE102ģ���Զ�˯��
void BLE102_READ_AUTOSLEEP(void)
{
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
			/*
			if(UART2_Rx_Buf[i+2]==0x0A || UART2_Rx_Buf[i+2]==0x0D)
			{
				break;
			}*/
			DATA[i]=UART2_Rx_Buf[i+0];
		}
		printf("BLE102ģ���Զ�˯�߲���:");
		HAL_UART_Transmit(&huart1,DATA,UART2_Rx_cnt-0,0x10);    //���ͽ��յ�������
		printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ�鴮�ڴ��ʱ��
void BLE102_READ_UARTTM(void)
{
	uint8_t DATA_OUT[12]="AT+UARTTM?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,12,0x00ff);
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-10;i++)
		{
			if(UART2_Rx_Buf[i+10]==0x0A || UART2_Rx_Buf[i+10]==0x0D)
			{
				break;
			}
			BLE102_UARTTM[i]=UART2_Rx_Buf[i+10];
		}
		printf("BLE102ģ�鴮�ڴ��ʱ��:");
		HAL_UART_Transmit(&huart1,BLE102_UARTTM,5,0x10);    //���ͽ��յ�������
		printf("0(����)\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ�鴮�ڲ���
void BLE102_READ_UART(void)
{
	uint8_t DATA_OUT[10]="AT+UART?\r\n";
	HAL_UART_Transmit(&huart2,DATA_OUT,10,0x00ff);
	uint8_t DATA_long[50];//���ڴ��57600��8��0��0����
	int X=8;
	while(UART2_Rx_flg == 0)//�ȴ�����
	{
	HAL_Delay(1);
	}
	if(UART2_Rx_flg)
	{
		for(int i=0;i<UART2_Rx_cnt-X;i++)
		{
			if(UART2_Rx_Buf[i+X]==0x0A || UART2_Rx_Buf[i+X]==0x0D)
			{
				break;
			}
			DATA_long[i]=UART2_Rx_Buf[i+X];
		}
		/*���ˣ��Ѿ��õ�57600��8��0��0����*/
	//BLE102_UART_DATA
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
	
	printf("BLE102ģ�鴮�ڲ���:\r\n");
	printf("		������:");
	HAL_UART_Transmit(&huart1,BLE102_UART_DATA[0],10,0x10);
	printf("\r\n");
	printf("		����λ:");
	HAL_UART_Transmit(&huart1,BLE102_UART_DATA[1],10,0x10);
	printf("\r\n");
	printf("		У��λ:");
	HAL_UART_Transmit(&huart1,BLE102_UART_DATA[2],10,0x10);
	printf("\r\n");
	printf("		ֹͣλ:");
	HAL_UART_Transmit(&huart1,BLE102_UART_DATA[3],10,0x10);
	printf("\r\n");
	UART2_INT_REST();//�����ж����ݻ���
	}
}

//��ѯBLE102ģ���Ƿ�ʹ��������֤
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
		printf("BLE102ģ������ʹ����֤:");
		if(DATA_IN[0]==0x4E)
		{
		printf("ʹ��");
		BLE102_PASSEN=1;
		}
		else if(DATA_IN[0]==0x46)
		{
		printf("��ֹ");
		BLE102_PASSEN=0;
		}
		else
		{
		printf("��ѯBLE102ģ���Ƿ�ʹ��������֤������");
		}
		//HAL_UART_Transmit(&huart1,BLE102_CIVER,UART2_Rx_cnt-7,0x10);    //���ͽ��յ�������
		printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ������/*ERROR*/
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
		printf("BLE102ģ������:");
		HAL_UART_Transmit(&huart1,BLE102_PASS,UART2_Rx_cnt-8,0x10);    //���ͽ��յ�������
		//printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ�鷢�书��
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
		
		printf("BLE102ģ�鷢�书��:");
		HAL_UART_Transmit(&huart1,BLE102_TPL,1,0x10);    //���ͽ��յ�������
		printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ������汾��
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
		printf("BLE102ģ������汾��:");
		HAL_UART_Transmit(&huart1,BLE102_CIVER,UART2_Rx_cnt-7,0x10);    //���ͽ��յ�������
		printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ������
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
		printf("BLE102ģ�����ƣ�");
		HAL_UART_Transmit(&huart1,BLE102_NAME,UART2_Rx_cnt-7,0x10);    //���ͽ��յ�������
		UART2_INT_REST();//�����ж����ݻ���
	} 
}

//��ѯBLE102ģ��MAC��ַ
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
		printf("BLE102ģ��MAC��ַ��");
		HAL_UART_Transmit(&huart1,BLE102_MAC,UART2_Rx_cnt-6,0x10);    //���ͽ��յ�������
		printf("\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	} 
}
//��λBLE102ģ��
void BLE102_REST(void)
{
	printf("��λBLE102ģ�飺");
	BLE102_RES_Enable();
	HAL_Delay(300);
	BLE102_RES_Disable();
	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//�����жϣ���ATָ���������֮��
	while(UART2_Rx_flg == 0)
	{
	HAL_Delay(1);
	}
		if(UART2_Rx_flg)
	{
		HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //���ͽ��յ�������
		UART2_INT_REST();//�����ж����ݻ���
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

	uint8_t pDATA[5]="+ok\r\n";
	if(UART2_Rx_flg && StringComparison(UART2_Rx_Buf,pDATA,5)== 1)
	{
		printf("����ATָ��ģʽ\r\n");
		UART2_INT_REST();//�����ж����ݻ���
	}
	else
	{
		printf("����ATָ��ģʽ���ִ��󣡣�\r\n");
	}
}

void BLE102_Init(void)
{
	printf("BLE102��ʼ���� \r\n");
	


	BLE102_REST();//��λBLE102ģ��
	BLE102_AT_instruction();//����ָ��ģʽ
	BLE102_READ_NAME();//��ѯģ������
	BLE102_READ_MAC();//��ѯģ��MAC��ַ
	BLE102_READ_CIVER();//��ѯģ������汾��
	//BLE102_READ_TPL();//��ѯģ�鷢�书��/*���Ʋ�ѯ֮����Ҫ���½�AT*/
	BLE102_READ_PASS();//��ѯģ������
	BLE102_READ_PASSEN();//��ѯBLE102ģ���Ƿ�ʹ��������֤
	BLE102_READ_UART();//��ѯBLE102ģ�鴮�ڲ���
	BLE102_READ_UARTTM();//��ѯBLE102ģ�鴮�ڴ��ʱ��
	BLE102_READ_AUTOSLEEP();//��ѯBLE102ģ���Զ�˯�߲���

	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//�����жϣ���ATָ���������֮��

}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
