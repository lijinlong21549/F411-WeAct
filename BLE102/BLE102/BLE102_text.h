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
#ifndef __BLE102_text_H__
#define __BLE102_text_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "stdio.h"

void Keyword_String_Intercept(uint8_t* DATA,uint8_t* PDATA,uint8_t* Keyword,uint8_t* DATA_STOP);
int Characters_converted_numbers(uint8_t* DATA);
void Transfer_character(uint8_t* DATA,uint8_t* PDATA,int Bit_long);
int StringComparison(uint8_t* DATA,uint8_t* pDATA,int NUM);
void String_Intercept(uint8_t DATA[255],int DATA_begin_long,uint8_t DATA_STOP[1]);


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_text_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
