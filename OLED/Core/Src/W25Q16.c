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
* @brief  初始化W25QXXXX接口。
* @retval None
**********************************************************/
uint8_t W25Qx_Init(void)
{ 
	uint8_t state;
	/* 重置 W25Qxxx */
	W25Qx_Reset();
	
	state = W25Qx_Get_Parameter(&W25Qx_Para);
	
	return state;
}

/**
  * @brief  此功能重置W25Qx。
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
  * @brief  读取W25QXXXX的当前状态。
  * @retval W25QXXXX内存状态
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
  * @brief  该功能发送一个写使能并等待它生效。
  * @retval None
  */
uint8_t W25Qx_WriteEnable(void)
{
	uint8_t cmd[] = {WRITE_ENABLE_CMD};
	uint32_t tickstart = get_tick();

	/*选择FLASH：片选低 */
	W25Qx_Enable();
	/* 发送读取ID命令 */
	HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXXXX_TIMEOUT_VALUE);	
	/*取消选择FLASH：芯片选择高电平 */
	W25Qx_Disable();
	
	/* 等待Flash写入结束 */
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
  * @brief  阅读制造商/设备ID。
	* @param  返回值地址
/   返回值如下:
/   0XEF13,表示芯片型号为W25Q80   
/   0XEF14,表示芯片型号为W25Q16     
/   0XEF15,表示芯片型号为W25Q32   
/   0XEF16,表示芯片型号为W25Q64  
  * @retval None
  */
void W25Qx_Read_ID(uint16_t *ID)
{
	uint8_t idt[2];
	idt[0]=0x11;
	idt[1]=0x11;
	uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
	
	W25Qx_Enable();
	/* 发送读取ID命令 */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);	
	/* 数据接收 */
	HAL_SPI_Receive(&hspi1,idt, 2, W25QXXXX_TIMEOUT_VALUE);

	printf("Flash型号参数：");
	printf("%X",idt[0]);
	printf("%X\r\n",idt[1]);
	*ID = (idt[0] << 8) + idt[1]; 
	
	W25Qx_Disable();
		
}

#include <math.h>
/**
  * @brief  获取W25QX参数。
  * @param  参数：W25Qx参数
  * @retval 空值
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
  * @brief  从QSPI存储器读取大量数据。
  * @param  pData：指向要读取的数据的指针
  * @param  ReadAddr：读取起始地址
  * @param  大小：要读取的数据大小
  * @retval QSPI内存状态
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
	/* 发送读取ID命令 */
	HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXXXX_TIMEOUT_VALUE);	
	/* 数据接收 */
	if (HAL_SPI_Receive(&hspi1, pData,Size,W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
  {
    return W25Qx_ERROR;
  }
	W25Qx_Disable();
	return W25Qx_OK;
}

/**
  * @brief  将大量数据写入QSPI存储器。
  * @param  pData：指向要写入的数据的指针
  * @param  WriteAddr：写入起始地址
  * @param  大小：要写入的数据大小，不超过256byte。
  * @retval QSPI内存状态
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

		/* 启用写操作 */
		W25Qx_WriteEnable();
	
		W25Qx_Enable();
    /* 发送命令 */
    if (HAL_SPI_Transmit(&hspi1,cmd, 4, W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
    
    /* 数据传输 */
    if (HAL_SPI_Transmit(&hspi1, pData,current_size, W25QXXXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Qx_ERROR;
    }
		W25Qx_Disable();
    	/* 等待Flash写入结束 */
		while(W25Qx_GetStatus() == W25Qx_BUSY)
		{
			/* 检查超时 */
			if((get_tick() - tickstart) > W25QXXXX_TIMEOUT_VALUE)
			{        
				return W25Qx_TIMEOUT;
			}
			//delay(1);
		}
    
    /* 更新地址和大小变量以进行下一页编程 */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25QXXXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXXXX_PAGE_SIZE;
  } while (current_addr < end_addr);

	
	return W25Qx_OK;
}

/**
  * @brief  擦除QSPI存储器的指定块。
  * @param  BlockAddress：要擦除的块地址
  * @retval QSPI内存状态
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
  * @brief  擦除整个QSPI存储器。此功能将花费很长时间。
  * @retval QSPI内存状态
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
