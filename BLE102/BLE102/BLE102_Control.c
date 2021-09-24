#include "BLE102_Control.h"
#include "BLE102_text.h"
#include <string.h>
Bluetooth_BLE102 BLE102_1;
/*********************��ʼ���ṹ��*********************/
int BLE102_Pretreatment(Bluetooth_BLE102 *BLE102)
{
    BLE102->Wake_Pin = BLE102_WARK_Pin;
    BLE102->Wake_Prot = BLE102_WARK_GPIO_Port;
    BLE102->Reset_Pin = BLE102_RES_Pin;
    BLE102->Reset_Prot = BLE102_RES_GPIO_Port;
    BLE102->UART_Aisle = huart2;
    BLE102->Mode=BLE102_Mode_Slave;
}
/*********************DMA����*********************/
uint8_t BLE102_UART_Rx_flg = 0;        //USART1������ɱ�־
uint8_t BLE102_UART_receive_buff[255]; //���ڵĻ������
int BLE102_UART_data_length;           //����1�����ݳ���
uint8_t BLE102_UART_TX_DMA_OVER = 0;   //USART1������ɱ�־

//����2��DMA��������жϺ���
int BLE102_UASRT_DMA_OVER()
{
    BLE102_UART_TX_DMA_OVER = 0;
    return BLE102_OK;
}
//����2��DMA���ͺ���
int BLE102_UASRT_DMA_printf(Bluetooth_BLE102 *BLE102, char *DATA)
{
    while (BLE102_UART_TX_DMA_OVER == 1) //�жϷ�����ɱ�־�Ƿ�Ϊ1
    {
        HAL_Delay(10);
    }

    if (BLE102_UART_TX_DMA_OVER == 0)
    {
        BLE102_UART_TX_DMA_OVER = 1; //�����ͱ�־��Ϊ0
        HAL_UART_Transmit_DMA(&(BLE102->UART_Aisle), (uint8_t *)DATA, strlen(DATA));
    }
    else
    {
        return BLE102_Error;
    }
    return BLE102_OK;
}
//����2���ж����غ���
int BLE102_UASRT_Interrupt_reload(Bluetooth_BLE102 *BLE102)
{
    //������ջ�����
    memset(BLE102_UART_receive_buff, 0, 255);
    BLE102_UART_data_length = 0;
    //���������ɱ�־λ
    BLE102_UART_Rx_flg = 0;
    //������ʼDMA���� ÿ��255�ֽ�����
    HAL_UART_Receive_DMA(&(BLE102->UART_Aisle), (uint8_t *)BLE102_UART_receive_buff, 255);
}
//����2���жϴ�����
int BLE102_UASRT_IDLECallback(Bluetooth_BLE102 *BLE102)
{
    //ֹͣ����DMA����
    HAL_UART_DMAStop(&(BLE102->UART_Aisle));

    //������յ������ݳ���
    BLE102_UART_data_length = 255 - __HAL_DMA_GET_COUNTER((BLE102->UART_Aisle).hdmarx);
    //��������ɱ�־��1
    BLE102_UART_Rx_flg = 1;
    //���Ժ����������յ������ݴ�ӡ��ȥ
    //printf("Receive Data(length = %d): ",UART2_data_length);
    //HAL_UART_Transmit(&huart1,UART2_receive_buff,UART2_data_length,0x200);
    //printf("\r\n");
    //USAR2_Interrupt_reload();
}
int BLE102_UASRT_IRQHandler(Bluetooth_BLE102 *BLE102)
{
    //�ж��Ƿ��ǿ����ж�
    if (RESET != __HAL_UART_GET_FLAG(&(BLE102->UART_Aisle), UART_FLAG_IDLE))
    {
        //��������жϱ�־�������һֱ���Ͻ����жϣ�
        __HAL_UART_CLEAR_IDLEFLAG(&(BLE102->UART_Aisle));
        //printf("\r\nUART1 Idle IQR Detected\r\n");
        BLE102_UASRT_IDLECallback(&*BLE102); //�����жϴ�����
    }
}

/*********************���ڲ���*********************/
/**
  * @brief ���ڶ�ȡ
  * @param BLE102��Ŀ���豸
  * @param DATA����ȡ������
  * @param NUM����ȡ���ݳ���
  * @retval ��ȡ�Ƿ�ɹ�
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
  * @brief ����д
  * @param BLE102��Ŀ���豸
  * @param DATA��д�������
  * @retval д���Ƿ�ɹ�
  */
int BLE102_UART_Write(Bluetooth_BLE102 *BLE102, uint8_t *DATA)
{
    while (BLE102_UART_TX_DMA_OVER == 1) //�жϷ�����ɱ�־�Ƿ�Ϊ1
    {
        HAL_Delay(10);
    }
    if (BLE102_UART_TX_DMA_OVER == 0)
    {
        BLE102_UART_TX_DMA_OVER = 1; //�����ͱ�־��Ϊ0
        HAL_UART_Transmit_DMA(&huart2, (uint8_t *)DATA, strlen((char *)DATA));
    }
}
/*********************ATָ��ṹ��*********************/
/**
  * @brief ����ATָ��ģʽ
  * @param BLE102��Ŀ���豸
  * @retval �Ƿ�ɹ�
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
        printf("����ATģʽ\r\n");
        BLE102->AT_Mode = BLE102_AT_Order;
        return BLE102_OK;
    }
    else
    {
        return BLE102_Error;
    }
}
/**
  * @brief �˳�ATָ��ģʽ
  * @param BLE102��Ŀ���豸
  * @retval �Ƿ�ɹ�
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
        printf("�˳�ATģʽ\r\n");
        BLE102->AT_Mode = BLE102_AT_DATA;
    }
}
/**
  * @brief ATģʽ���飬�鿴�Ƿ�������ģʽ��������������ATģʽ
  * @param BLE102��Ŀ���豸
  * @retval �Ƿ�ɹ�
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
  * @brief Ӳ����λBLE102
  * @param BLE102��Ŀ���豸
  * @retval �Ƿ�ɹ�
  */
int BLE102_HardRest(Bluetooth_BLE102 *BLE102)
{
    BLE102_UASRT_Interrupt_reload(&*BLE102); //�����ж����ݻ���
    //Ӳ����λ
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(300);
    HAL_GPIO_WritePin(BLE102->Reset_Prot, BLE102->Reset_Pin, GPIO_PIN_SET);

    //���߻�������
    HAL_GPIO_WritePin(BLE102->Wake_Prot, BLE102->Wake_Pin, GPIO_PIN_RESET);
    while (BLE102_UART_Rx_flg == 0)
    {
        HAL_Delay(1);
    }
    if (BLE102_UART_Rx_flg)
    {
        BLE102_UASRT_Interrupt_reload(&*BLE102); //�����ж����ݻ���
    }
    HAL_Delay(500);
}