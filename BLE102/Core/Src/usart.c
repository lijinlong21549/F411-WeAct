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


uint8_t UART1_Rx_flg = 0;                   //USART1接收完成标志
uint8_t UART2_Rx_flg = 0;                   //USART2接收完成标志
uint8_t UART1_receive_buff[255];						//串口1的缓存变量
uint8_t UART2_receive_buff[255];						//串口2的缓存变量
int UART1_data_length;											//串口1的数据长度
int UART2_data_length;											//串口2的数据长度

uint8_t UART1_TX_DMA_OVER = 0;							//USART1发送完成标志
uint8_t UART2_TX_DMA_OVER = 0;							//USART2发送完成标志
//extern uint8_t receive_buff[255];

//串口2的DMA发送完成中断函数
void USART2_DMA_OVER()
{
	UART2_TX_DMA_OVER = 0;
}

//串口2的DMA发送函数
void USART2_DMA_printf(char* DATA,int bit)
{
	while(UART2_TX_DMA_OVER == 1)//判断发送完成标志是否为1
	{
		HAL_Delay(10);
	}


	if(UART2_TX_DMA_OVER == 0)
	{
		UART2_TX_DMA_OVER = 1;//将发送标志置为0
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)DATA, strlen(DATA));
	}
	else
	{
		printf("串口2出错 \r\n");
	}
}

//串口1的DMA发送函数
void USART1_DMA_printf(char* DATA)
{
	while(UART1_TX_DMA_OVER == 1)//判断发送完成标志是否为1
	{
		HAL_Delay(10);
	}


	if(UART1_TX_DMA_OVER == 0)
	{
		UART1_TX_DMA_OVER = 1;//将发送标志置为0
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)DATA, strlen(DATA));
		printf("发送 \r\n");
	}
	else
	{
		printf("串口1出错 \r\n");
	}
	
}
//串口1的DMA发送完成中断函数
void USART1_DMA_OVER()
{
	UART1_TX_DMA_OVER = 0;
}

//串口2的中断重载函数
void USAR2_Interrupt_reload()
{  
	  //清零接收缓冲区
    memset(UART2_receive_buff,0,255);                                            
    UART2_data_length = 0;
		//清除接收完成标志位
		UART2_Rx_flg = 0;
    //重启开始DMA传输 每次255字节数据
    HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_receive_buff, 255);       
}

//串口1的中断重载函数
void USAR1_Interrupt_reload()
{
	  //清零接收缓冲区
    memset(UART1_receive_buff,0,255);
    UART1_data_length = 0;
		//清除接收完成标志位
		UART1_Rx_flg = 0;
    //重启开始DMA传输 每次255字节数据
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)UART1_receive_buff, 255);
}

//串口1的中断处理函数
void USAR1_IDLECallback(UART_HandleTypeDef *huart)
{
	//停止本次DMA传输
    HAL_UART_DMAStop(&huart1);  
                                                       
    //计算接收到的数据长度
    UART1_data_length  =255-__HAL_DMA_GET_COUNTER(huart1.hdmarx);
		//将接收完成标志置1
    UART1_Rx_flg=1;
		//测试函数：将接收到的数据打印出去
    printf("Receive Data(length = %d): ",UART1_data_length);
    HAL_UART_Transmit(&huart1,UART1_receive_buff,UART1_data_length,0x200);                     
    printf("\r\n");
		USAR1_Interrupt_reload();
}

//串口2的中断处理函数
void USAR2_IDLECallback(UART_HandleTypeDef *huart)
{
	//停止本次DMA传输
    HAL_UART_DMAStop(&huart2);  
    
    //计算接收到的数据长度
    UART2_data_length  =255-__HAL_DMA_GET_COUNTER(huart2.hdmarx);   
    //将接收完成标志置1
    UART2_Rx_flg=1;
		//测试函数：将接收到的数据打印出去
    //printf("Receive Data(length = %d): ",UART2_data_length);
    //HAL_UART_Transmit(&huart1,UART2_receive_buff,UART2_data_length,0x200);
    //printf("\r\n");      
		//USAR2_Interrupt_reload();
}
//中断判断函数
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //判断是否是串口1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //清楚空闲中断标志（否则会一直不断进入中断）
            //printf("\r\nUART1 Idle IQR Detected\r\n");
            USAR1_IDLECallback(huart);                          //调用中断处理函数
        }
    }
    if(USART2 == huart2.Instance)                                   //判断是否是串口1
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //清楚空闲中断标志（否则会一直不断进入中断）
            //printf("\r\nUART2 Idle IQR Detected\r\n");
            USAR2_IDLECallback(huart);                          //调用中断处理函数
        }
    }
		
}



//串口2中断变量
uint8_t UART2_Rx_Buf[MAX_REC_LENGTH] = {0}; //USART1存储接收数据

unsigned int  UART2_Rx_cnt = 0;                   //USART1接受数据计数器
uint8_t UART2_temp[REC_LENGTH] = {0};       //USART1接收数据缓存

void UART2_INT_REST()//重置中断数据缓存
{
		HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//重新启动中断
		for(int i = 0;i<UART2_Rx_cnt;i++)															//清空缓存区
		{
			UART2_Rx_Buf[i] = 0;
		}
		UART2_Rx_cnt = 0;
		UART2_Rx_flg = 0;
}
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART2)//判断中断函数
  {
		if(BLE102_AT_command_MODE == 1)//判断是否在AT握手模式
		{
			if(UART2_temp[0] == 0x61)
			{
			uint8_t DATA[1]="a";
			HAL_UART_Transmit(&huart2,DATA,1,0x00ff);
			UART2_INT_REST();//重置中断数据缓存
			BLE102_AT_command_MODE=0;
			}
		}
		else if(BLE102_AT_command_MODE == 0)
		{
			
			UART2_Rx_Buf[UART2_Rx_cnt] = UART2_temp[0];//将接收到的数据送到缓存区
			UART2_Rx_cnt++;														 //数据长度+1
			if(0x0A == UART2_temp[0]&&UART2_Rx_cnt>=3)//判断数据是否为换行  &&UART2_Rx_cnt>=3
			{
				UART2_Rx_flg = 1;															//将接收完成标志置1
			}
			HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//重新启动中断
			//


			if(UART2_Rx_flg)
			{
				HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //发送接收到的数据
				for(int i = 0;i<UART2_Rx_cnt;i++)															//清空缓存区
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
			printf("BLE102AT握手变量 BLE102_AT_command_MODE 出现错误！！ \r\n");
		}

  }
	
	if(huart->Instance==USART1)//判断中断函数
  {
			UART2_Rx_Buf[UART2_Rx_cnt] = UART2_temp[0];//将接收到的数据送到缓存区
			UART2_Rx_cnt++;														 //数据长度+1
			if(0x0A == UART2_temp[0]&&UART2_Rx_cnt>=3)//判断数据是否为换行//&&UART2_Rx_cnt>=3
			{
				UART2_Rx_flg = 1;															//将接收完成标志置1
			}
			HAL_UART_Receive_IT(&huart1,(uint8_t *)UART2_temp,REC_LENGTH);//重新启动中断
			if(UART2_Rx_flg)
			{
				HAL_UART_Transmit(&huart1,UART2_Rx_Buf,UART2_Rx_cnt,0x10);    //发送接收到的数据
				for(int i = 0;i<UART2_Rx_cnt;i++)															//清空缓存区
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
/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&huart1,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		
	int ch;
  while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET);
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);	
  
	return (ch);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
