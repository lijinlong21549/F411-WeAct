#include "BLE102_Use.h"
#include "BLE102_text.h"
#include <string.h>
/**
  * @brief ����ģ������
  * @param BLE102��Ŀ���豸
  * @retval �����Ƿ�ɹ�
  */
/**
  * @brief ����ģ������
  * @param BLE102��Ŀ���豸
  * @retval �����Ƿ�ɹ�
  */
int BLE102_Witer_Pass(Bluetooth_BLE102 *BLE102)
{
    //�����Ƿ���ATģʽ
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
  * @brief ��ȡ�豸MAC��ַ
  * @param BLE102��Ŀ���豸
  * @retval �����Ƿ�ɹ�
  */
int BLE102_Read_MAC(Bluetooth_BLE102 *BLE102)
{
    //�����Ƿ���ATģʽ
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
  * @brief ����ģ������
  * @param BLE102��Ŀ���豸
  * @param BLE102_NAME������
  * @retval �����Ƿ�ɹ�
  */
int BLE102_Witer_Name(Bluetooth_BLE102 *BLE102)
{
    //�����Ƿ���ATģʽ
    BLE102_judge_AT_MODE(&*BLE102);
    //��������
    uint8_t DATA[10 + 15] = "AT+NAME=";
    strcat((char *)DATA, (char *)BLE102->NAME);
    strcat((char *)DATA, (char *)"\r\n");
    BLE102_UART_Write(&*BLE102, DATA);
    //��ȡ��Ӧ
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
  * @brief ����ģ�鹤��ģʽ
  * @param BLE102��Ŀ���豸
  * @retval �����Ƿ�ɹ�
  */
int BLE102_Witer_MODE(Bluetooth_BLE102 *BLE102)
{
    //�����Ƿ���ATģʽ
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
    //У���Ӧ
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
        //printf("ģʽ���óɹ� \r\n");
        
    }
    else
    {
        return BLE102_Error;
    }
    //�ȴ�������ȡ������ӭ��
		HAL_Delay(200);
    uint8_t DATA_Boot_up[20];
    BLE102_UART_Read(&*BLE102, DATA_Boot_up, 20);
    BLE102->AT_Mode = BLE102_AT_DATA;
		HAL_Delay(500);
		return BLE102_OK;

}
int BLE102_Init(Bluetooth_BLE102 *BLE102)
{
    printf("BLE102��ʼ���� \r\n");
    //��ʼ���ṹ��
    BLE102_Pretreatment(&*BLE102);
    //���������ж�
    __HAL_UART_ENABLE_IT(&(BLE102->UART_Aisle), UART_IT_IDLE);
    //Ӳ����λBLE102
    BLE102_HardRest(&*BLE102);
    //����ATָ��ģʽ
    printf("��ʼ����ATָ��ģʽ\r\n");
    BLE102_AT_In(&*BLE102);
    BLE102_Witer_Name(&*BLE102);
    BLE102_Witer_MODE(&*BLE102);
    BLE102_Read_MAC(&*BLE102);
		BLE102_Witer_Pass(&*BLE102);
    //BLE102_AT_Out(&*BLE102);
}