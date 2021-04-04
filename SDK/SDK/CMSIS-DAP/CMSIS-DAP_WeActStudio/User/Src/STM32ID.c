#include "STM32ID.h"

/*获取MCU的唯一ID*/
void GetSTM32MCUID(uint32_t *id,MCUTypedef type)
{
	uint32_t idAddr[]={0x1FFFF7AC,  /*STM32F0唯一ID起始地址*/
                0x1FFFF7E8,  /*STM32F1唯一ID起始地址*/
                0x1FFF7A10,  /*STM32F2唯一ID起始地址*/
                0x1FFFF7AC,  /*STM32F3唯一ID起始地址*/
                0x1FFF7A10,  /*STM32F4唯一ID起始地址*/
                0x1FF0F420,  /*STM32F7唯一ID起始地址*/
                0x1FF80050,  /*STM32L0唯一ID起始地址*/
                0x1FF80050,  /*STM32L1唯一ID起始地址*/
                0x1FFF7590,  /*STM32L4唯一ID起始地址*/
                0x1FF0F420}; /*STM32H7唯一ID起始地址*/
  if(id!=NULL)
  {
		//地址从小到大,先放低字节，再放高字节：小端模式
    //地址从小到大,先放高字节，再放低字节：大端模式
    id[0]=*(uint32_t*)(idAddr[type]);    // 低字节
    id[1]=*(uint32_t*)(idAddr[type]+4);
    id[2]=*(uint32_t*)(idAddr[type]+8);  // 高字节
  }
}
