#ifndef __BNO055_H__
#define __BNO055_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "BNO055.h"
#include "I2C.h"
#include "stdio.h"

static void BNO055_Read_Dat(uint8_t dat)
{
	uint8_t buf[2];
	buf[0] = BNO055_I2C_ADDR1;
	buf[1] = dat;
	//HAL_I2C_Master_Transmit(&hi2c1, BNO055_I2C_ADDR1<<1, buf, 2, 100);
	HAL_I2C_Master_Transmit(&hi2c1,BNO055_I2C_ADDR1,(uint8_t*)buf,2,1000);
	//HAL_Delay(1);
	//uint8_t DATA[1];
	//HAL_I2C_Master_Receive(&hi2c1, BNO055_I2C_ADDR1<<1, DATA, 1, 100);
	//printf("%s\r\n",DATA);
}
/*
static void OLED_Write_Cmd(uint8_t cmd)
{
	uint8_t buf[2];
	buf[0] = 0x00;	//control byte
	buf[1] = cmd;
	
	//使用HAL库的API实现
	HAL_I2C_Master_Transmit(&hi2c2, 0x78, buf, 2, 0xFFFF);
}
*/
void BNO055_Init(void)
{
BNO055_RES_Enable();
HAL_Delay(10);
BNO055_RES_Disable();
HAL_Delay(50);


BNO055_Read_Dat(BNO055_CHIP_ID_ADDR);

}


#ifdef __cplusplus
}
#endif

#endif /* __BNO055_H__ */
