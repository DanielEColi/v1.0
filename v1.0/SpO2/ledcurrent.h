#ifndef __LED_CURRENT_H__
#define __LED_CURRENT_H__

//LED电流计算
#include "datatype.h"

#define CURRENT_ADJUST_STYLE		1	//0:10ma-100mA 10个档调节方式 1:1-255轮循方式

#define JUDGE_FINGER_OFF_NUM		10	//判断是否手指脱落，方法：如果JUDGE_FINGER_OFF_NUM个值变化小于COMPARE_VALUE，则还是手指脱落
#define COMPARE_VALUE						500


#if(CURRENT_ADJUST_STYLE==0)
#define MAX_CURRENT_VALUE	9
#define MIN_CURRENT_VALUE	2  

#define DEAULT_LED1_CURRENT_LEVEL	9//136	
#define DEAULT_LED2_CURRENT_LEVEL 9//136	
#else
#define MAX_CURRENT_VALUE	255
#define MIN_CURRENT_VALUE	100  //20		//~2mA 5/255*100

#define DEAULT_LED1_CURRENT_LEVEL	255	
#define DEAULT_LED2_CURRENT_LEVEL 255	
#endif

#define DEAULT_LED1_GAIN_LEVEL		0
#define DEAULT_LED2_GAIN_LEVEL		0

//增益设置与RF成正比
#define MAX_GAIN_VALUE		6
#define MIN_GAIN_VALUE		0

#define MIN_ADC_VALUE	400000
#define MAX_ADC_VALUE	1800000

#define MID_ADC_VALUE	1000000

extern u8 g_LedCurrentLevel[2];	//0:10mA 1:20mA 2:30mA 3:40mA 4:50mA 5:60mA 6:70mA 7:80mA 8:90mA 9:100mA
extern u8 g_LedGain[2];

void SetGain(u8 LedType,u8 gain);
void SetLedCurrent(u8 type,u8 indexcurrent);
void LedCurrentControl(s32* min);
void SetMaxCurrentGain(void);
u8 CheckFingerOff2(void);
void AddLedCurrentSampleData(s32* adcvalue);
#endif

