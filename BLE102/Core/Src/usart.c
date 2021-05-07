/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include <string.h>
/* USER CODE BEGIN 0 */
#include "BLE102.h"


uint8_t UART1_Rx_flg = 0;                   //USART1������ɱ�־
uint8_t UART2_Rx_flg = 0;                   //USART2������ɱ�־
uint8_t UART1_receive_buff[255];						//����1�Ļ������
uint8_t UART2_receive_buff[255];						//����2�Ļ������
int UART1_data_length;											//����1�����ݳ���
int UART2_data_length;											//����2�����ݳ���

uint8_t UART1_TX_DMA_OVER = 0;							//USART1������ɱ�־
uint8_t UART2_TX_DMA_OVER = 0;							//USART2������ɱ�־
//extern uint8_t receive_buff[255];

//����2��DMA��������жϺ���
void USART2_DMA_OVER()
{
	UART2_TX_DMA_OVER = 0;
}

//����2��DMA���ͺ���
void USART2_DMA_printf(char* DATA,int bit)
{
	while(UART2_TX_DMA_OVER == 1)//�жϷ�����ɱ�־�Ƿ�Ϊ1
	{
		HAL_Delay(10);
	}


	if(UART2_TX_DMA_OVER == 0)
	{
		UART2_TX_DMA_OVER = 1;//�����ͱ�־��Ϊ0
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)DATA, strlen(DATA));
	}
	else
	{
		printf("����2���� \r\n");
	}
}

//����1��DMA���ͺ���
void USART1_DMA_printf(char* DATA)
{
	while(UART1_TX_DMA_OVER == 1)//�жϷ�����ɱ�־�Ƿ�Ϊ1
	{
		HAL_Delay(10);
	}


	if(UART1_TX_DMA_OVER == 0)
	{
		UART1_TX_DMA_OVER = 1;//�����ͱ�־��Ϊ0
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)DATA, strlen(DATA));
		printf("���� \r\n");
	}
	else
	{
		printf("����1���� \r\n");
	}
	
}
//����1��DMA��������жϺ���
void USART1_DMA_OVER()
{
	UART1_TX_DMA_OVER = 0;
}

//����2���ж����غ���
void USAR2_Interrupt_reload()
{  
	  //������ջ�����
    memset(UART2_receive_buff,0,255);                                            
    UART2_data_length = 0;
		//���������ɱ�־λ
		UART2_Rx_flg = 0;
    //������ʼDMA���� ÿ��255�ֽ�����
    HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_receive_buff, 255);       
}

//����1���ж����غ���
void USAR1_Interrupt_reload()
{
	  //������ջ�����
    memset(UART1_receive_buff,0,255);
    UART1_data_length = 0;
		//���������ɱ�־λ
		UART1_Rx_flg = 0;
    //������ʼDMA���� ÿ��255�ֽ�����
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)UART1_receive_buff, 255);
}

//����1���жϴ�����
void USAR1_IDLECallback(UART_HandleTypeDef *huart)
{
	//ֹͣ����DMA����
    HAL_UART_DMAStop(&huart1);  
                                                       
    //������յ������ݳ���
    UART1_data_length  =255-__HAL_DMA_GET_COUNTER(huart1.hdmarx);
		//��������ɱ�־��1
    UART1_Rx_flg=1;
		//���Ժ����������յ������ݴ�ӡ��ȥ
    printf("Receive Data(length = %d): ",UART1_data_length);
    HAL_UART_Transmit(&huart1,UART1_receive_buff,UART1_data_length,0x200);                     
    printf("\r\n");
		USAR1_Interrupt_reload();
}

//����2���жϴ�����
void USAR2_IDLECallback(UART_HandleTypeDef *huart)
{
	//ֹͣ����DMA����
    HAL_UART_DMAStop(&huart2);  
    
    //������յ������ݳ���
    UART2_data_length  =255-__HAL_DMA_GET_COUNTER(huart2.hdmarx);   
    //��������ɱ�־��1
    UART2_Rx_flg=1;
		//���Ժ����������յ������ݴ�ӡ��ȥ
    //printf("Receive Data(length = %d): ",UART2_data_length);
    //HAL_UART_Transmit(&huart1,UART2_receive_buff,UART2_data_length,0x200);
    //printf("\r\n");      
		//USAR2_Interrupt_reload();
}
//�ж��жϺ���
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //�ж��Ƿ��Ǵ���1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
            //printf("\r\nUART1 Idle IQR Detected\r\n");
            USAR1_IDLECallback(huart);                          //�����жϴ�����
        }
    }
    if(USART2 == huart2.Instance)                                   //�ж��Ƿ��Ǵ���1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
            //printf("\r\nUART2 Idle IQR Detected\r\n");
            USAR2_IDLECallback(huart);                          //�����жϴ�����
        }
    }
		
}



//����2�жϱ���
uint8_t UART2_Rx_Buf[MAX_REC_LENGTH] = {0}; //USART1�洢��������

unsigned int  UART2_Rx_cnt = 0;                   //USART1�������ݼ�����
uint8_t UART2_temp[REC_LENGTH] = {0};       //USART1�������ݻ���

void UART2_INT_REST()//�����ж����ݻ���
{
		HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//���������ж�
		for(int i = 0;i<UART2_Rx_cnt;i++)															//��ջ�����
		{
			UART2_Rx_Buf[i] = 0;
		}
		UART2_Rx_cnt = 0;
		UART2_Rx_flg = 0;
}
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART2)//�ж��жϺ���
  {
		if(BLE102_AT_command_MODE == 1)//�ж��Ƿ���AT����ģʽ
		{
			if(UART2_temp[0] == 0x61)
			{
			uint8_t DATA[1]="a";
			HAL_UART_Transmit(&huart2,DATA,1,0x00ff);
			UART2_INT_REST();//�����ж����ݻ���
			BLE102_AT_command_MODE=0;
			}
		}
		else if(BLE102_AT_command_MODE == 0)
		{
			
			UART2_Rx_Buf[UART2_Rx_cnt] = UART2_temp[0];//�����յ��������͵�������
			UART2_Rx_cnt++;														 //���ݳ���+1
			if(0x0A == UART2_temp[0]&&UART2_Rx_cnt>=3)//�ж������Ƿ�Ϊ����  &&UART2_Rx_cnt>=3
			{
				UART2_Rx_flg = 1;															//��������ɱ�־��1
			}
			HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//���������ж�
			//


			if(UART2_Rx_flg)
			{
				HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //���ͽ��յ�������
				for(int i = 0;i<UART2_Rx_cnt;i++)															//��ջ�����
				{
					UART2_Rx_Buf[i] = 0;
				}
				UART2_Rx_cnt = 0;
				UART2_Rx_flg = 0;
			} 
			//
		}
		else
		{
			printf("BLE102AT���ֱ��� BLE102_AT_command_MODE ���ִ��󣡣� \r\n");
		}

  }
	
	if(huart->Instance==USART1)//�ж��жϺ���
  {
			UART2_Rx_Buf[UART2_Rx_cnt] = UART2_temp[0];//�����յ��������͵�������
			UART2_Rx_cnt++;														 //���ݳ���+1
			if(0x0A == UART2_temp[0]&&UART2_Rx_cnt>=3)//�ж������Ƿ�Ϊ����//&&UART2_Rx_cnt>=3
			{
				UART2_Rx_flg = 1;															//��������ɱ�־��1
			}
			HAL_UART_Receive_IT(&huart1,(uint8_t *)UART2_temp,REC_LENGTH);//���������ж�
			if(UART2_Rx_flg)
			{
				HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //���ͽ��յ�������
				for(int i = 0;i<UART2_Rx_cnt;i++)															//��ջ�����
				{
					UART2_Rx_Buf[i] = 0;
				}
				UART2_Rx_cnt = 0;
				UART2_Rx_flg = 0;
			} 
		
	}
	
}
*/





/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)UART1_receive_buff, 255);
}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_receive_buff, 255);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*****************  �����ַ��� **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&huart1,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		
	int ch;
  while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET);
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);	
  
	return (ch);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
