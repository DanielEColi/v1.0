#include "timer.h"
#include "circlequeue.h"
#include "digitaloxiprocess.h"
#include "afe4490.h"
#include "spo2tools.h"
#include "bciprotocol.h"

void TimerHandle(void)	//600hz¶¨Ê±Æ÷
{
//	static u8 flag=0;
	if(g_T1Flag%6)
	{
		return;
	}
/*	static u8 spo2_counter=0;
	static u16 second_counter=0;
  if(!g_T1Flag) 
  {
		return;
  }
	g_T1Flag=0;
	spo2_counter++;
	second_counter++;
	switch(spo2_counter)
	{
	case 3:						//200hz
		break;
	case 6:						//100hz
		spo2_counter=0;
		break;
	default:
			break;
	}
	if(second_counter>=600)
	{
		WorkLed();
		second_counter=0;
	}*/
}




