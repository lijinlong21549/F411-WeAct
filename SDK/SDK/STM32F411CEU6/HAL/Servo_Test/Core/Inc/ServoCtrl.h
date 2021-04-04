#ifndef __servoctrl_H
#define __servoctrl_H

#include "main.h"
#include "stm32f4xx_hal.h"
//#include "usart.h"
#include "tim.h"

#define S1_OUT TIM_CHANNEL_1
#define S2_OUT TIM_CHANNEL_2
#define S3_OUT TIM_CHANNEL_3
#define S4_OUT TIM_CHANNEL_4

#define SxTime htim11
#define GetTime() ( HAL_GetTick() ) 

typedef struct 
{
	float ServoSpeed; // ��ÿ��	
	float ServoAngleNow; // ��ǰ�Ƕ�
	uint32_t TimeLast;  // �ϴ����ýǶ�ʱ��
	uint8_t ServoAngleSet; // 0 ~ 180��
	uint8_t MaxAngleSet; // ������ýǶ�
	uint8_t MinAngleSet; // ��С���ýǶ�
} ServoCtrl_t;


void ServoCtrlInit(void);

// ���ö���ٶ� �Ƕ�
void setServo(ServoCtrl_t *ServoCtr,float Speed,uint8_t Angle);

extern ServoCtrl_t ServoCtrlS1;

#endif

