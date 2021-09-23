#include "BLE102_Control.h"
#include "BLE102_Use.h"

int BLE102_Init(Bluetooth_BLE102 *BLE102)
{
    printf("BLE102初始化： \r\n");
    //开启串口中断
    __HAL_UART_ENABLE_IT(&(BLE102->UART_Aisle), UART_IT_IDLE);
    //硬件复位
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_SET);
    //拉高唤醒引脚
    HAL_GPIO_WritePin(BLE102->Wake_Prot, BLE102->Wake_Pin, GPIO_PIN_SET);
    //进入AT指令模式
    
}