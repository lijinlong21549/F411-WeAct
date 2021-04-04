/**
  ******************************************************************************
	WeAct ΢�д��� 
	>> ��׼��ʵ������
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
// ȫ�ֶ��� STM32F411xE ���� STM32F401xx
// ��ǰ���� STM32F411xE

// STM32F411 �ⲿ����25Mhz�����ǵ�USBʹ�ã��ڲ�Ƶ������Ϊ96Mhz
// ��Ҫ100mhz,�����޸�system_stm32f4xx.c

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/* Enable Clock Security System(CSS): this will generate an NMI exception
     when HSE clock fails *****************************************************/
  RCC_ClockSecuritySystemCmd(ENABLE);
	
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */

  /* SysTick end of count event each 1ms */
  SystemCoreClockUpdate();
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
	
  /* Add your application code here */
  /* Insert 50 ms delay */
  Delay(50);
	
  GPIO_Config();
  TIM_Config();   
  /* Infinite loop */
  while (1)
  {
#if soft_pwm
		/* C13 �����Ʋ��� */
		static uint8_t pwmset;
		static uint16_t time;
		static uint8_t timeflag;
		static uint8_t timecount;

		 /* ������ */
		if(timeflag == 0)
		{
			time ++;
			if(time >= 1600) timeflag = 1;
		}
		else
		{
			time --;
			if(time == 0) timeflag = 0;
		}

		/* ռ�ձ����� */
		pwmset = time/80;

		/* 20ms ���� */
		if(timecount > 20) timecount = 0;
		else timecount ++;

		if(timecount >= pwmset ) GPIO_SetBits(LED_C13_PORT,LED_C13_PIN);
		else GPIO_ResetBits(LED_C13_PORT,LED_C13_PIN);
		
		Delay(1);
#endif
	}
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
