#include "STM32ID.h"

/*��ȡMCU��ΨһID*/
void GetSTM32MCUID(uint32_t *id,MCUTypedef type)
{
	uint32_t idAddr[]={0x1FFFF7AC,  /*STM32F0ΨһID��ʼ��ַ*/
                0x1FFFF7E8,  /*STM32F1ΨһID��ʼ��ַ*/
                0x1FFF7A10,  /*STM32F2ΨһID��ʼ��ַ*/
                0x1FFFF7AC,  /*STM32F3ΨһID��ʼ��ַ*/
                0x1FFF7A10,  /*STM32F4ΨһID��ʼ��ַ*/
                0x1FF0F420,  /*STM32F7ΨһID��ʼ��ַ*/
                0x1FF80050,  /*STM32L0ΨһID��ʼ��ַ*/
                0x1FF80050,  /*STM32L1ΨһID��ʼ��ַ*/
                0x1FFF7590,  /*STM32L4ΨһID��ʼ��ַ*/
                0x1FF0F420}; /*STM32H7ΨһID��ʼ��ַ*/
  if(id!=NULL)
  {
		//��ַ��С����,�ȷŵ��ֽڣ��ٷŸ��ֽڣ�С��ģʽ
    //��ַ��С����,�ȷŸ��ֽڣ��ٷŵ��ֽڣ����ģʽ
    id[0]=*(uint32_t*)(idAddr[type]);    // ���ֽ�
    id[1]=*(uint32_t*)(idAddr[type]+4);
    id[2]=*(uint32_t*)(idAddr[type]+8);  // ���ֽ�
  }
}
