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
#ifndef __W25Q16_H__
#define __W25Q16_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stdio.h"
#include "W25Q16.h"

#define delay(ms) osDelay(ms)
#define get_tick() HAL_GetTick()

W25Qx_Parameter W25Qx_Para;

/**********************************************************
* @brief  ��ʼ��W25QXXXX�ӿڡ�
* @retval None
**********************************************************/
uint8_t W25Qx_Init(void)
{ 
	uint8_t state;
	/* ���� W25Qxxx */
	W25Qx_Reset();
	
	state = W25Qx_Get_Parameter(&W25Qx_Para);
	
	return state;
}

/**
  * @brief  �˹�������W25Qx��
  * @retval None
  */
static void	W25Qx_Reset(void)
{
	uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};
	
	W25Qx_Enable();
	/* Send the reset command */
	HAL_SPI_Transmit(&hspi1, cmd, 2, W25QXXXX_TIMEOUT_VALUE);	
	W25Qx_Disable();

}

/**
  * @brief  ��ȡW25QXXXX�ĵ�ǰ״̬��
  * @retval W25QXXXX�ڴ�״̬
  */
static uint8_t W25Qx_GetStatus(void)
{
	uint8_t cmd[] = {READ_STATUS_REG1_CMD};
	uint8_t status;
	
	W25Qx_Enable();
	
	/* Send the read status command */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);	
	/* Reception of the data */
	HAL_SPI_Receive(&hspi1,&status, 1, W25QXXXX_TIMEOUT_VALUE);
	W25Qx_Disable();
	
	/* Check the value of the register */
  if((status & W25QXXXX_FSR_BUSY) != 0)
  {
    return W25Qx_BUSY;
  }
	else
	{
		return W25Qx_OK;
	}		
}

/**
  * @brief  �ù��ܷ���һ��дʹ�ܲ��ȴ�����Ч��
  * @retval None
  */
uint8_t W25Qx_WriteEnable(void)
{
	uint8_t cmd[] = {WRITE_ENABLE_CMD};
	uint32_t tickstart = get_tick();

	/*ѡ��FLASH��Ƭѡ�� */
	W25Qx_Enable();
	/* ���Ͷ�ȡID���� */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);	
	/*ȡ��ѡ��FLASH��оƬѡ��ߵ�ƽ */
	W25Qx_Disable();
	
	/* �ȴ�Flashд����� */
	while(W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((get_tick() - tickstart) > W25QXXXX_TIMEOUT_VALUE)
    {        
			return W25Qx_TIMEOUT;
    }
		HAL_Delay(1);
	}
	
	return W25Qx_OK;
}

/**
  * @brief  �Ķ�������/�豸ID��
	* @param  ����ֵ��ַ
/   ����ֵ����:
/   0XEF13,��ʾоƬ�ͺ�ΪW25Q80   
/   0XEF14,��ʾоƬ�ͺ�ΪW25Q16     
/   0XEF15,��ʾоƬ�ͺ�ΪW25Q32   
/   0XEF16,��ʾоƬ�ͺ�ΪW25Q64  
  * @retval None
  */
void W25Qx_Read_ID(uint16_t *ID)
{
	uint8_t idt[2];
	idt[0]=0x11;
	idt[1]=0x11;
	uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
	
	W25Qx_Enable();
	/* ���Ͷ�ȡID���� */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);	
	/* ���ݽ��� */
	HAL_SPI_Receive(&hspi1,idt, 2, W25QXXXX_TIMEOUT_VALUE);

	printf("Flash�ͺŲ�����");
	printf("%X",idt[0]);
	printf("%X\r\n",idt[1]);
	*ID = (idt[0] << 8) + idt[1]; 
	
	W25Qx_Disable();
		
}

#include <math.h>
/**
  * @brief  ��ȡW25QX������
  * @param  ������W25Qx����
  * @retval ��ֵ
  */
uint8_t W25Qx_Get_Parameter(W25Qx_Parameter *Para)
{
	uint16_t id;
	uint32_t size;
	
	Para->PAGE_SIZE = 256;
	Para->SUBSECTOR_SIZE = 4096;
	Para->SECTOR_SIZE = 0x10000;
	
	W25Qx_Read_ID(&id);
	if(id < W25Q80 || id > W25Q128) return W25Qx_ERROR;
	
	size = (uint32_t) powf(2,(id - 0xEF13)) * 1024 * 1024;
	
	Para->FLASH_ID = id;
	Para->FLASH_SIZE = size;
	Para->SUBSECTOR_COUNT = Para->FLASH_SIZE / Para->SUBSECTOR_SIZE;
	Para->SECTOR_COUNT = Para->FLASH_SIZE / Para->SECTOR_SIZE;
	
	return W25Qx_OK;
}
/**
  * @brief  ��QSPI�洢����ȡ�������ݡ�
  * @param  pData��ָ��Ҫ��ȡ�����ݵ�ָ��
  * @param  ReadAddr����ȡ��ʼ��ַ
  * @param  ��С��Ҫ��ȡ�����ݴ�С
  * @retval QSPI�ڴ�״̬
  */
uint8_t W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	uint8_t cmd[4];

	/* Configure the command */
	cmd[0] = READ_CMD;
	cmd[1] = (uint8_t)(ReadAddr >> 16);
	cmd[2] = (uint8_t)(ReadAddr >> 8);
	cmd[3] = (uint8_t)(ReadAddr);
	
	W25Qx_Enable();
	/* ���Ͷ�ȡID���� */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);	
	/* ���ݽ��� */
	if (HAL_SPI_Receive(&hspi1, pData,Size,W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
  {
    return W25Qx_ERROR;
  }
	W25Qx_Disable();
	return W25Qx_OK;
}

/**
  * @brief  ����������д��QSPI�洢����
  * @param  pData��ָ��Ҫд������ݵ�ָ��
  * @param  WriteAddr��д����ʼ��ַ
  * @param  ��С��Ҫд������ݴ�С��������256byte��
  * @retval QSPI�ڴ�״̬
  */
uint8_t W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
	uint8_t cmd[4];
	uint32_t end_addr, current_size, current_addr;
	uint32_t tickstart = get_tick();
	
	/* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)
  {
    current_addr += W25QXXXX_PAGE_SIZE;
  }
  current_size = current_addr - WriteAddr;

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;
	
  /* Perform the write page by page */
  do
  {
		/* Configure the command */
		cmd[0] = PAGE_PROG_CMD;
		cmd[1] = (uint8_t)(current_addr >> 16);
		cmd[2] = (uint8_t)(current_addr >> 8);
		cmd[3] = (uint8_t)(current_addr);

		/* ����д���� */
		W25Qx_WriteEnable();
	
		W25Qx_Enable();
    /* �������� */
    if (HAL_SPI_Transmit(&hspi1,cmd, 4, W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
    
    /* ���ݴ��� */
    if (HAL_SPI_Transmit(&hspi1, pData,current_size, W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
		W25Qx_Disable();
    	/* �ȴ�Flashд����� */
		while(W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* ��鳬ʱ */
			if((get_tick() - tickstart) > W25QXXXX_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
			//delay(1);
		}
    
    /* ���µ�ַ�ʹ�С�����Խ�����һҳ��� */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25QXXXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXXXX_PAGE_SIZE;
  } while (current_addr < end_addr);

	
	return W25Qx_OK;
}

/**
  * @brief  ����QSPI�洢����ָ���顣
  * @param  BlockAddress��Ҫ�����Ŀ��ַ
  * @retval QSPI�ڴ�״̬
  */
uint8_t W25Qx_Erase_Block(uint32_t Address)
{
	uint8_t cmd[4];
	uint32_t tickstart = get_tick();
	cmd[0] = SECTOR_ERASE_CMD;
	cmd[1] = (uint8_t)(Address >> 16);
	cmd[2] = (uint8_t)(Address >> 8);
	cmd[3] = (uint8_t)(Address);
	
	/* Enable write operations */
	W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
	
	/* Wait the end of Flash writing */
	while(W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((get_tick() - tickstart) > W25QXXXX_SECTOR_ERASE_MAX_TIME)
    {        
			return W25Qx_TIMEOUT;
    }
	  //delay(1);
	}
	return W25Qx_OK;
}

/**
  * @brief  ��������QSPI�洢�����˹��ܽ����Ѻܳ�ʱ�䡣
  * @retval QSPI�ڴ�״̬
  */
uint8_t W25Qx_Erase_Chip(void)
{
	uint8_t cmd[4];
	uint32_t tickstart = get_tick();
	cmd[0] = CHIP_ERASE_CMD;
	
	/* Enable write operations */
	W25Qx_WriteEnable();
	
	/*Select the FLASH: Chip Select low */
	W25Qx_Enable();
	/* Send the read ID command */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);	
	/*Deselect the FLASH: Chip Select high */
	W25Qx_Disable();
	
	/* Wait the end of Flash writing */
	while(W25Qx_GetStatus() == W25Qx_BUSY)
	{
		/* Check for the Timeout */
    if((get_tick() - tickstart) > W25QXXXX_BULK_ERASE_MAX_TIME)
    {        
			return W25Qx_TIMEOUT;
    }
	  //delay(1);
	}
	return W25Qx_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* __W25Q16_H__ */
