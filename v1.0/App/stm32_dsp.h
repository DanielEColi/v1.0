/**
  ******************************************************************************
  * @file STM32F10x_DSP_Lib/inc/stm32_dsp.h 
  * @author  MCD Application Team
  * @version  V2.0.0
  * @date  04/27/2009
  * @brief  This source file contains prototypes of DSP functions
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_DSP_H
#define __STM32F10x_DSP_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_lib.h"


/* Exported types ------------------------------------------------------------*/
/* for block FIR module */
typedef struct {
  short* h;
//  uint16_t *h;
  u32 nh;
} COEFS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* FIR 16-bit filter in assembly */
void fir_16by16_stm32(void *y, void *x, COEFS *c, u32 N);

#endif /* __STM32F10x_DSP_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
