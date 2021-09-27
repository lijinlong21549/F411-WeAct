#include "BLE102_Use.h"
#include "BLE102_text.h"
#include <string.h>
/**
  * @brief 设置模块密码
  * @param BLE102：目标设备
  * @retval 设置是否成功
  */
/**
  * @brief 设置模块密码
  * @param BLE102：目标设备
  * @retval 设置是否成功
  */
int BLE102_Witer_Pass(Bluetooth_BLE102 *BLE102)
{
    //检验是否在AT模式
    BLE102_judge_AT_MODE(&*BLE102);
    uint8_t DATA_OUT[10 + 6] = "AT+PASS=";
    strcat((char *)DATA_OUT, (char *)BLE102->PASS);
    strcat((char *)DATA_OUT, (char *)"\r\n");
    BLE102_UART_Write(&*BLE102, DATA_OUT);
    uint8_t DATA_CRC[16 + 6] = "\r\n+PASS:";
    uint8_t DATA_IN[16 + 6] = {0};
    strcat((char *)DATA_CRC, (char *)BLE102->PASS);
    strcat((char *)DATA_CRC, (char *)"\r\nOK\r\n");
    BLE102_UART_Read(&*BLE102, DATA_IN, 16 + 6);
    if (StringComparison(DATA_IN, DATA_CRC, 16 + 6) == 1)
    {
        return BLE102_OK;
    }
    else
    {
        return BLE102_Error;
    }
}
/**
  * @brief 读取设备MAC地址
  * @param BLE102：目标设备
  * @retval 设置是否成功
  */
int BLE102_Read_MAC(Bluetooth_BLE102 *BLE102)
{
    //检验是否在AT模式
    BLE102_judge_AT_MODE(&*BLE102);
    uint8_t DATA_OUT[9] = "AT+MAC?\r\n";
    BLE102_UART_Write(&*BLE102, DATA_OUT);
    uint8_t DATA_IN[13 + 12] = {0};
    BLE102_UART_Read(&*BLE102, DATA_IN, 13 + 12);
    for (int i = 0; i < 12; i++)
    {
        BLE102->MAC[i] = DATA_IN[i + 7];
    }
}
/**
  * @brief 设置模块名称
  * @param BLE102：目标设备
  * @param BLE102_NAME：名称
  * @retval 设置是否成功
  */
int BLE102_Witer_Name(Bluetooth_BLE102 *BLE102)
{
    //检验是否在AT模式
    BLE102_judge_AT_MODE(&*BLE102);
    //发送名称
    uint8_t DATA[10 + 15] = "AT+NAME=";
    strcat((char *)DATA, (char *)BLE102->NAME);
    strcat((char *)DATA, (char *)"\r\n");
    BLE102_UART_Write(&*BLE102, DATA);
    //读取回应
    uint8_t DATA_CRC[13 + 15] = "\r\n+NAME:";
    uint8_t DATA_IN[13 + 15] = {0};
    strcat((char *)DATA_CRC, (char *)BLE102->NAME);
    strcat((char *)DATA_CRC, (char *)"\r\nOK\r\n");

    BLE102_UART_Read(&*BLE102, DATA_IN, 13 + 15);
    if (StringComparison(DATA_IN, DATA_CRC, 13 + 15) == 1)
    {
        return BLE102_OK;
    }
    else
    {
        return BLE102_Error;
    }
}
/**
  * @brief 设置模块工作模式
  * @param BLE102：目标设备
  * @retval 设置是否成功
  */
int BLE102_Witer_MODE(Bluetooth_BLE102 *BLE102)
{
    //检验是否在AT模式
    BLE102_judge_AT_MODE(&*BLE102);
    uint8_t DATA[11] = "AT+MODE=";
    if (BLE102->Mode == BLE102_Mode_Mast)
    {
        strcat((char *)DATA, (char *)"M\r\n");
    }
    else if (BLE102->Mode == BLE102_Mode_Slave)
    {
        strcat((char *)DATA, (char *)"S\r\n");
    }
    else if (BLE102->Mode == BLE102_Mode_iBeacon)
    {
        strcat((char *)DATA, (char *)"B\r\n");
    }
    else if (BLE102->Mode == BLE102_Mode_Mesh)
    {
        strcat((char *)DATA, (char *)"F\r\n");
    }
    else
    {
        return BLE102_Error;
    }
    BLE102_UART_Write(&*BLE102, DATA);
    //校验回应
    uint8_t DATA_CRC[14 + 7] = "\r\n+MODE:";
    if (BLE102->Mode == BLE102_Mode_Mast)
    {
        strcat((char *)DATA_CRC, (char *)"Mast\r\nOK/r/n");
    }
    else if (BLE102->Mode == BLE102_Mode_Slave)
    {
        strcat((char *)DATA, (char *)"Slave\r\nOK/r/n");
    }
    else if (BLE102->Mode == BLE102_Mode_iBeacon)
    {
        strcat((char *)DATA, (char *)"iBeacon\r\nOK/r/n");
    }
    else if (BLE102->Mode == BLE102_Mode_Mesh)
    {
        strcat((char *)DATA, (char *)"Mesh\r\nOK/r/n");
    }
    else
    {
        return BLE102_Error;
    }
    uint8_t DATA_IN[14 + 7] = {0};
    BLE102_UART_Read(&*BLE102, DATA_IN, 14 + 7);
    if (StringComparison(DATA_IN, DATA_CRC, 14 + 7) == 1)
    {
        //printf("模式设置成功 \r\n");
        
    }
    else
    {
        return BLE102_Error;
    }
    //等待开机读取开机欢迎语
		HAL_Delay(200);
    uint8_t DATA_Boot_up[20];
    BLE102_UART_Read(&*BLE102, DATA_Boot_up, 20);
    BLE102->AT_Mode = BLE102_AT_DATA;
		HAL_Delay(500);
		return BLE102_OK;

}
int BLE102_Init(Bluetooth_BLE102 *BLE102)
{
    printf("BLE102初始化： \r\n");
    //初始化结构体
    BLE102_Pretreatment(&*BLE102);
    //开启串口中断
    __HAL_UART_ENABLE_IT(&(BLE102->UART_Aisle), UART_IT_IDLE);
    //硬件复位BLE102
    BLE102_HardRest(&*BLE102);
    //进入AT指令模式
    printf("开始进入AT指令模式\r\n");
    BLE102_AT_In(&*BLE102);
    BLE102_Witer_Name(&*BLE102);
    BLE102_Witer_MODE(&*BLE102);
    BLE102_Read_MAC(&*BLE102);
		BLE102_Witer_Pass(&*BLE102);
    //BLE102_AT_Out(&*BLE102);
}