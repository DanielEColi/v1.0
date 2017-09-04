#include "piprocess.h"
#include "ledcurrent.h"
#include "spo2tools.h"
#include "ledcurrent.h"
#include "global.h"
#include "plethwave.h"
/*
PI=(AC/DC)*100
���µļ��㷽���Ǵ����
60mA:
ԭʼ�������8000����,3%����
6%,16000
0.5%���1500����
*/

//��ǰ��עָ��


//��ʹ�ú�������
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


