#include "digitaloxiprocess.h"
#include "circlequeue.h"
#include "oxisample.h"
#include "stm32_dsp.h"
#include "global.h"
#include "spo2tools.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

u16 DigitalOxiSignalBuf[2][TIMER_SECOND*SIZE_SAMPLE_FRQ+(FIR_CALC_NUM-1)];
u16 DigitalOxiSignalBuf_Idx=0;
u8 g_DigitalOxiSampleTime=0; 
s32 FirDigitalOxiSignalBuf[2][TIMER_SECOND*SIZE_SAMPLE_FRQ];

void Digital_Oxi_Handle(void)
{
/*	u16 firdatalen;
	u16 i;
	
	if(g_DigitalOxiHalfSecFullFlag)			
  {
		g_DigitalOxiHalfSecFullFlag=0;		
		if(g_DigitalOxiSampleTime<20)
		{
  		if(g_DigitalOxiSampleTime==18)
			{
				g_DigitalOxiSampleTime=17;
			}
			else
			{
				g_DigitalOxiSampleTime++; 			
			}
		}
		if(g_DigitalOxiSampleTime==(TIMER_SECOND<<1)+1)	//6.5s
		{
			memcpy(DigitalOxiSignalBuf[0],&(DigitalOxiSignalBuf[0][HALF_SIZE_SAMPLE_FRQ-(FIR_ORDER-1)]),sizeof(u16)*(((TIMER_SECOND<<1)-1)*HALF_SIZE_SAMPLE_FRQ+(FIR_ORDER-1)));
			memcpy(DigitalOxiSignalBuf[1],&(DigitalOxiSignalBuf[1][HALF_SIZE_SAMPLE_FRQ-(FIR_ORDER-1)]),sizeof(u16)*(((TIMER_SECOND<<1)-1)*HALF_SIZE_SAMPLE_FRQ+(FIR_ORDER-1)));
			DigitalOxiSignalBuf_Idx=(TIMER_SECOND-1)*SIZE_SAMPLE_FRQ+(FIR_ORDER-1);
		}
		else if(g_DigitalOxiSampleTime>(TIMER_SECOND<<1)+3)	//7.5s
		{
			memcpy(DigitalOxiSignalBuf[0],&(DigitalOxiSignalBuf[0][HALF_SIZE_SAMPLE_FRQ]),sizeof(u16)*(((TIMER_SECOND<<1)-1)*HALF_SIZE_SAMPLE_FRQ+(FIR_ORDER-1)));
			memcpy(DigitalOxiSignalBuf[1],&(DigitalOxiSignalBuf[1][HALF_SIZE_SAMPLE_FRQ]),sizeof(u16)*(((TIMER_SECOND<<1)-1)*HALF_SIZE_SAMPLE_FRQ+(FIR_ORDER-1)));
			DigitalOxiSignalBuf_Idx=((TIMER_SECOND<<1)-1)*SIZE_SAMPLE_FRQ+(FIR_ORDER-1);
		}
		for(i=0;i<HALF_SIZE_SAMPLE_FRQ;i++)
		{
			DigitalOxiSignalBuf[0][DigitalOxiSignalBuf_Idx]=DigitalOxiHalfSecBuf[0][i];
			DigitalOxiSignalBuf[1][DigitalOxiSignalBuf_Idx]=DigitalOxiHalfSecBuf[1][i];
			DigitalOxiSignalBuf_Idx=(DigitalOxiSignalBuf_Idx+1)%(TIMER_SECOND*SIZE_SAMPLE_FRQ+(FIR_ORDER-1));
		}		
		if(g_DigitalOxiSampleTime>=(TIMER_SECOND<<1)+1&&(g_DigitalOxiSampleTime%2))//与模拟血氧分开处理，比模拟血氧慢0.5s
		{
			firdatalen=TIMER_SECOND*SIZE_SAMPLE_FRQ;
		}
		else
		{
			return;
		}
		
		fir_16by16_stm32(FirDigitalOxiSignalBuf[0],DigitalOxiSignalBuf[0],&Fir_Coefs,firdatalen);
		fir_16by16_stm32(FirDigitalOxiSignalBuf[1],DigitalOxiSignalBuf[1],&Fir_Coefs,firdatalen);	
		
		if(GetP2PValue(firdatalen,DIGITAL_OXI_IDX))
		{
			if(PR_Process(DIGITAL_OXI_IDX))
			{
				SpO2_Process(DIGITAL_OXI_IDX);
			}
		}	 
  } */
}


