#ifndef __STM32ID_H
#define __STM32ID_H


#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <string.h>

/*定义STM32 MCU的类型*/
typedef enum {
  STM32F0,
  STM32F1,
  STM32F2,
  STM32F3,
  STM32F4,
  STM32F7,
  STM32L0,
  STM32L1,
  STM32L4,
  STM32H7,
}MCUTypedef;

void GetSTM32MCUID(uint32_t *id,MCUTypedef type);

#ifdef __cplusplus
}
#endif

#endif
