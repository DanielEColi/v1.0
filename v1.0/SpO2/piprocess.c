#include "piprocess.h"
#include "ledcurrent.h"
#include "spo2tools.h"
#include "ledcurrent.h"
#include "global.h"
#include "plethwave.h"
/*
PI=(AC/DC)*100
以下的计算方法是错误的
60mA:
原始数据相差8000左右,3%灌柱
6%,16000
0.5%相差1500左右
*/

//当前灌注指数


//仅使用红外数据
void AddPIData(s32 ir_adc,s32 r_adc)
{
	s32 adcvalue[2];
	adcvalue[0]=ir_adc;
	adcvalue[1]=r_adc;
#if(LED_CURRENT_ADJUST==1)	
	LedCurrentControl(adcvalue);
#endif
}

void ClearPIData(void)
{
	g_CurrentPILevel=15;	
}


