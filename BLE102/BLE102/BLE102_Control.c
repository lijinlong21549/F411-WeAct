#include "BLE102_Control.h"
#include "BLE102_text.h"
#include <string.h>
Bluetooth_BLE102 BLE102_1;
/*********************初始化结构体*********************/
int BLE102_Pretreatment(Bluetooth_BLE102 *BLE102)
{
    BLE102->Wake_Pin = BLE102_WARK_Pin;
    BLE102->Wake_Prot = BLE102_WARK_GPIO_Port;
    BLE102->Reset_Pin = BLE102_RES_Pin;
    BLE102->Reset_Prot = BLE102_RES_GPIO_Port;
    BLE102->UART_Aisle = huart2;
    BLE102->Mode=BLE102_Mode_Slave;
}
/*********************DMA操作*********************/
uint8_t BLE102_UART_Rx_flg = 0;        //USART1接收完成标志
uint8_t BLE102_UART_receive_buff[255]; //串口的缓存变量
int BLE102_UART_data_length;           //串口1的数据长度
uint8_t BLE102_UART_TX_DMA_OVER = 0;   //USART1发送完成标志

//串口2的DMA发送完成中断函数
int BLE102_UASRT_DMA_OVER()
{
    BLE102_UART_TX_DMA_OVER = 0;
    return BLE102_OK;
}
//串口2的DMA发送函数
int BLE102_UASRT_DMA_printf(Bluetooth_BLE102 *BLE102, char *DATA)
{
    while (BLE102_UART_TX_DMA_OVER == 1) //判断发送完成标志是否为1
    {
        HAL_Delay(10);
    }

    if (BLE102_UART_TX_DMA_OVER == 0)
    {
        BLE102_UART_TX_DMA_OVER = 1; //将发送标志置为0
        HAL_UART_Transmit_DMA(&(BLE102->UART_Aisle), (uint8_t *)DATA, strlen(DATA));
    }
    else
    {
        return BLE102_Error;
    }
    return BLE102_OK;
}
//串口2的中断重载函数
int BLE102_UASRT_Interrupt_reload(Bluetooth_BLE102 *BLE102)
{
    //清零接收缓冲区
    memset(BLE102_UART_receive_buff, 0, 255);
    BLE102_UART_data_length = 0;
    //清除接收完成标志位
    BLE102_UART_Rx_flg = 0;
    //重启开始DMA传输 每次255字节数据
    HAL_UART_Receive_DMA(&(BLE102->UART_Aisle), (uint8_t *)BLE102_UART_receive_buff, 255);
}
//串口2的中断处理函数
int BLE102_UASRT_IDLECallback(Bluetooth_BLE102 *BLE102)
{
    //停止本次DMA传输
    HAL_UART_DMAStop(&(BLE102->UART_Aisle));

    //计算接收到的数据长度
    BLE102_UART_data_length = 255 - __HAL_DMA_GET_COUNTER((BLE102->UART_Aisle).hdmarx);
    //将接收完成标志置1
    BLE102_UART_Rx_flg = 1;
    //测试函数：将接收到的数据打印出去
    //printf("Receive Data(length = %d): ",UART2_data_length);
    //HAL_UART_Transmit(&huart1,UART2_receive_buff,UART2_data_length,0x200);
    //printf("\r\n");
    //USAR2_Interrupt_reload();
}
int BLE102_UASRT_IRQHandler(Bluetooth_BLE102 *BLE102)
{
    //判断是否是空闲中断
    if (RESET != __HAL_UART_GET_FLAG(&(BLE102->UART_Aisle), UART_FLAG_IDLE))
    {
        //清楚空闲中断标志（否则会一直不断进入中断）
        __HAL_UART_CLEAR_IDLEFLAG(&(BLE102->UART_Aisle));
        //printf("\r\nUART1 Idle IQR Detected\r\n");
        BLE102_UASRT_IDLECallback(&*BLE102); //调用中断处理函数
    }
}

/*********************串口操作*********************/
/**
  * @brief 串口读取
  * @param BLE102：目标设备
  * @param DATA：读取的数据
  * @param NUM：读取数据长度
  * @retval 读取是否成功
  */
int BLE102_UART_Read(Bluetooth_BLE102 *BLE102, uint8_t *DATA, int NUM)
{
    while (BLE102_UART_Rx_flg == 0)
    {
        HAL_Delay(1);
    }
    for (int i = 0; i < NUM; i++)
    {
        DATA[i] = BLE102_UART_receive_buff[i];
    }
    BLE102_UASRT_Interrupt_reload(&*BLE102);
}
/**
  * @brief 串口写
  * @param BLE102：目标设备
  * @param DATA：写入的数据
  * @retval 写入是否成功
  */
int BLE102_UART_Write(Bluetooth_BLE102 *BLE102, uint8_t *DATA)
{
    while (BLE102_UART_TX_DMA_OVER == 1) //判断发送完成标志是否为1
    {
        HAL_Delay(10);
    }
    if (BLE102_UART_TX_DMA_OVER == 0)
    {
        BLE102_UART_TX_DMA_OVER = 1; //将发送标志置为0
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *)DATA, strlen((char *)DATA));
    }
}
/*********************AT指令结构体*********************/
/**
  * @brief 进入AT指令模式
  * @param BLE102：目标设备
  * @retval 是否成功
  */
int BLE102_AT_In(Bluetooth_BLE102 *BLE102)
{
    uint8_t DATA[3] = "+++";
    BLE102_UART_Write(&*BLE102, DATA);
    uint8_t DATA_IN[1] = {0};
    BLE102_UART_Read(&*BLE102, DATA_IN, 1);
    if (DATA_IN[0] == 'a')
    {
        uint8_t DATAOUT[1] = "a";
        BLE102_UART_Write(&*BLE102, DATAOUT);
    }
    else
    {
        return BLE102_Error;
    }
    uint8_t DATA_AT_OUT[5] = "+ok\r\n";
    uint8_t DATA_AT[5] = {0};
    BLE102_UART_Read(&*BLE102, DATA_AT, 5);
    if (StringComparison(DATA_AT_OUT, DATA_AT, 5) == 1)
    {
        printf("进入AT模式\r\n");
        BLE102->AT_Mode = BLE102_AT_Order;
        return BLE102_OK;
    }
    else
    {
        return BLE102_Error;
    }
}
/**
  * @brief 退出AT指令模式
  * @param BLE102：目标设备
  * @retval 是否成功
  */
int BLE102_AT_Out(Bluetooth_BLE102 *BLE102)
{
    uint8_t DATA_OUT[9] = "AT+ENTM\r\n";
    BLE102_UART_Write(&*BLE102, DATA_OUT);

    uint8_t DATA_AT_OUT[16] = "\r\n+ENTM:OK\r\nOK\r\n";
    uint8_t DATA_AT[16] = {0};
    BLE102_UART_Read(&*BLE102, DATA_AT, 16);
    if (StringComparison(DATA_AT_OUT, DATA_AT, 16) == 1)
    {
        printf("退出AT模式\r\n");
        BLE102->AT_Mode = BLE102_AT_DATA;
    }
}
/**
  * @brief AT模式检验，查看是否在命令模式，如果不在则进入AT模式
  * @param BLE102：目标设备
  * @retval 是否成功
  */
int BLE102_judge_AT_MODE(Bluetooth_BLE102 *BLE102)
{
    if (BLE102->AT_Mode == BLE102_AT_DATA)
    {
        if(BLE102_AT_In(&*BLE102)!=BLE102_OK)
        {
            return BLE102_Error;
        }
        else
        {
            return BLE102_OK;
        }
    }
}
/**
  * @brief 硬件复位BLE102
  * @param BLE102：目标设备
  * @retval 是否成功
  */
int BLE102_HardRest(Bluetooth_BLE102 *BLE102)
{
    BLE102_UASRT_Interrupt_reload(&*BLE102); //重置中断数据缓存
    //硬件复位
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_SET);

    //拉高唤醒引脚
    HAL_GPIO_WritePin(BLE102->Wake_Prot, BLE102->Wake_Pin, GPIO_PIN_RESET);
    while (BLE102_UART_Rx_flg == 0)
    {
        HAL_Delay(1);
    }
    if (BLE102_UART_Rx_flg)
    {
        BLE102_UASRT_Interrupt_reload(&*BLE102); //重置中断数据缓存
    }
    HAL_Delay(500);
}