/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stdio.h"

#define REC_LENGTH  1
#define MAX_REC_LENGTH  1024 

/*串口2中断变量*/
extern uint8_t UART2_Rx_Buf[MAX_REC_LENGTH];
extern uint8_t UART2_Rx_flg ;
extern unsigned int  UART2_Rx_cnt ;
extern uint8_t UART2_temp[REC_LENGTH];

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
void UART2_INT_REST();
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void USAR2_Interrupt_reload();
void USAR1_Interrupt_reload();
void USART1_DMA_printf(uint8_t* DATA);
void USART1_DMA_OVER();
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USART2_DMA_OVER();
void USART2_DMA_printf(char* DATA,int bit);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
