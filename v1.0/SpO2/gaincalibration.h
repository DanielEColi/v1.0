//增益校准
#ifndef __GAINCALIBRATION_H__
#define __GAINCALIBRATION_H__

#include "datatype.h"

#define LED_CURR_MAX		50			//mA
#define LED_CURR_MIN		5				//mA
#define TARGET_THR			1/2			//50%
#define LOW_THR					1/10		//10%
#define HIGH_THR				9/10		//90%
#define HYS_PERCENT			1/100		//1%
#define BLOCK_SIZE			16			//16 samples
#define TIME_WAIT				20			//20ms
#define TFREEZE					1000 		//1000ms

#define LOW_THR_HYS_LOW	

/*
ILED_CURR_MAX = 3 mA
ILED_CURR_MIN = 1 mA
TARGET_THR = 50%
LOW_THR = 10%
HYS_PERCENT = 1%
HIGH_THR = 90%
BLOCK_SIZE = 16 samples
TIME_WAIT = 20 ms
TFREEZE = 1 sec

Derived Variables:
LOW_THR_HYS_LOW = LOW_THR – HYS_PERCENT = 9%
LOW_THR_HYS_HIGH = LOW_THR + HYS_PERCENT = 11%
HIGH_THR_HYS_LOW = HIGH_THR – HYS_PERCENT = 89%
HIGH_THR_HYS_HIGH = HIGH_THR + HYS_PERCENT = 91%
*/

#endif

