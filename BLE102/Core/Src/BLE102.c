#ifndef __BLE102_H__
#define __BLE102_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "BLE102.h"


int BLE102_AT_command_MODE=0;//串口命令模式

void BLE102_Init(void)
{
	BLE102_AT_command_MODE=1;
	uint8_t DATA[3]="+++";
	HAL_UART_Transmit(&huart2,DATA,3,0x00ff);

	
	HAL_UART_Receive_IT(&huart2,(uint8_t *)UART2_temp,REC_LENGTH);//开启中断（在AT指令握手完成之后）
	//HAL_Delay(1);
	//uint8_t DATA2[9]="AT+ENTM\r";
	//HAL_UART_Transmit(&huart2,DATA2,9,0x00ff);
}


#ifdef __cplusplus
}
#endif

#endif /* __BLE102_H__ */
