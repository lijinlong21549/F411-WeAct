/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int EC20_Jitter_Bit=1;//EC20的按键抖动标志位
int EC20_KEY_Bit=1;//EC20的按钮状态；1是松开，0是按下
int EC20_NUM=0;//EC20旋转值

int EC20_Cycle=0;//EC20周期位
int EC20_Direction=0;//EC20旋转方向；0是顺时针，1是逆时针
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim11;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */
		if(EC20_Jitter_Bit==1)//防抖动判断
	{

	if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==1)//上升沿判断
	{
		if(EC20_Cycle==0)//无等待
		{
			EC20_Cycle=1;//创建等待
			EC20_Direction=0;//顺时针
		}
		else if(EC20_Cycle==1)//有等待
		{
			EC20_Cycle=2;//输出
		}

	}
	else if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==0)//下降沿判断
	{
		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0)
		{
			EC20_Cycle=0;
		}
	}
		/*
		if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==1)//上升沿判断
 		{

		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0&&EC20_Cycle==0)//周期为0时，方向判断
		{
			EC20_Direction=0;
			EC20_Cycle=1;
		}
		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==1&&EC20_Cycle==1)//周期为1时，周期推进
		{
		EC20_Cycle=2;
		}

		}
	else if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0)//下降沿判断
		{
		
		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==1&&EC20_Cycle==2)//周期为2时，周期推进
		{
		EC20_Cycle=3;
		}

		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0&&EC20_Cycle==3)//周期为2时，周期推进
		{
		EC20_Cycle=4;
		}

		}
		*/
	if(EC20_Cycle==2)
	{
		EC20_Cycle=0;
		if(EC20_Direction==1)
		{
			printf("逆时针\r\n");
		}
		else if(EC20_Direction==0)
		{
			printf("顺时针\r\n");
		}
	}

		EC20_Jitter_Bit=0;
		HAL_TIM_Base_Start_IT(&htim11);
	}
  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
		if(EC20_Jitter_Bit==1)//防抖动判断
	{	
	
	if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==1)//上升沿判断
	{
		if(EC20_Cycle==0)//无等待
		{
			EC20_Cycle=1;//创建等待
			EC20_Direction=1;//逆时针
		}
		else if(EC20_Cycle==1)//有等待
		{
			EC20_Cycle=2;//输出
		}

	}
	else if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0)//下降沿判断
	{
		if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==0)
		{
			EC20_Cycle=0;
		}
	}

	if(EC20_Cycle==2)
	{
		EC20_Cycle=0;
		if(EC20_Direction==1)
		{
			printf("逆时针\r\n");
		}
		else if(EC20_Direction==0)
		{
			printf("顺时针\r\n");
		}
	}

		/*
		if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==1)//上升沿判断
 		{
			if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==0&&EC20_Cycle==0)//周期为0时，方向判断
			{
				EC20_Direction=1;
				EC20_Cycle=1;
			}
			
			if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==1&&EC20_Cycle==1)//周期为1时，周期推进
			{
				EC20_Cycle=2;
			}
		}
		else if(HAL_GPIO_ReadPin(EC11_CK_GPIO_Port,EC11_CK_Pin)==0)//下降沿判断
		{
			if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==1&&EC20_Cycle==2)//周期为1时，周期推进
			{
				EC20_Cycle=3;
			}
			
			if(HAL_GPIO_ReadPin(EC11_DK_GPIO_Port,EC11_DK_Pin)==0&&EC20_Cycle==3)//周期为2时，周期推进
			{
				EC20_Cycle=4;
			}
		}
		
		if(EC20_Cycle==4)
		{
			EC20_Cycle=0;
			if(EC20_Direction==0)
			{
				printf("逆时针\r\n");
			}
			else if(EC20_Direction==1)
			{
				printf("顺时针\r\n");
			}
			
		}*/




		EC20_Jitter_Bit=0;
		HAL_TIM_Base_Start_IT(&htim11);
	}

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_IRQn 1 */
	if(EC20_Jitter_Bit==1)
	{
		if(HAL_GPIO_ReadPin(EC11_KEY_GPIO_Port,EC11_KEY_Pin)==0&&EC20_KEY_Bit==1)
		{
			printf("0\r\n");
			EC20_Jitter_Bit=0;
			HAL_TIM_Base_Start_IT(&htim11);
			EC20_KEY_Bit=0;
		}
		else if(HAL_GPIO_ReadPin(EC11_KEY_GPIO_Port,EC11_KEY_Pin)==1&&EC20_KEY_Bit==0)
		{
			printf("1\r\n");
			EC20_Jitter_Bit=0;
			HAL_TIM_Base_Start_IT(&htim11);
			EC20_KEY_Bit=1;
		}
		else
		{
			printf("错误\r\n");
		}

	}
  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles TIM1 trigger and commutation interrupts and TIM11 global interrupt.
  */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 0 */

  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 0 */
  HAL_TIM_IRQHandler(&htim11);
  /* USER CODE BEGIN TIM1_TRG_COM_TIM11_IRQn 1 */
	//printf("定时器中断\r\n");
	HAL_TIM_Base_Stop_IT(&htim11);
	EC20_Jitter_Bit=1;
  /* USER CODE END TIM1_TRG_COM_TIM11_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
