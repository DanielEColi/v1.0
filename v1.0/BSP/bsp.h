#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f2xx_conf.h"

#define I2C_SPEED              	10000
#define I2C_SLAVE_ADDRESS	    0xA0


//以下地址请参考参考手册
#define ADC1_DR_ADDRESS    ((u32)0x4001204C)
#define ADC2_DR_ADDRESS    ((u32)0x4001214C)
#define ADC3_DR_ADDRESS    ((u32)0x4001224C)

#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)

void BSP_Init(void);


#endif

