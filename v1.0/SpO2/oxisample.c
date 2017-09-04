#include "oxisample.h"
#include "global.h"
#include "digitaloxiprocess.h"
#include "circlequeue.h"
#include "spo2tools.h"
#include "bciprotocol.h"

vu32 g_CaptureBuffer[MAX_CAPTURE_TIMES];
vu8  g_CaptureIdx=0;
vu8  g_CaptureCount=0;

u32 g_DigitalWaveBuf[3*SIZE_SAMPLE_FRQ];		//digital 每秒采的数据
u16 g_DigitalWaveBuf_WrIdx=0;				//数据头下标
vu8 g_DigitalWaveBufFull=0;
u16 g_DigitalWaveBuf_RdIdx=0;	

u16 DigitalOxiHalfSecBuf[2][HALF_SIZE_SAMPLE_FRQ];		//digital oxi半秒采的数据
u16 g_DigitalOxiSecBuf_Idx=0;								//数据头下标
vu8 g_DigitalOxiHalfSecFullFlag=0;					//1/2s data full flag

void DigitalOxi_Sampling(u16 ir_freq,u16 r_freq)
{
	DigitalOxiHalfSecBuf[0][g_DigitalOxiSecBuf_Idx]=ir_freq;
	DigitalOxiHalfSecBuf[1][g_DigitalOxiSecBuf_Idx]=r_freq;
	g_DigitalOxiSecBuf_Idx=(g_DigitalOxiSecBuf_Idx+1)%HALF_SIZE_SAMPLE_FRQ;
	if(g_DigitalOxiSecBuf_Idx==0)
	{
		g_DigitalOxiHalfSecFullFlag=1;
	}
}

void FillSpO2RedPacket(u32 freq)
{
	static u8 flag=0;
	g_DigitalWaveBuf[g_DigitalWaveBuf_WrIdx]=freq;
	g_DigitalWaveBuf_WrIdx=(g_DigitalWaveBuf_WrIdx+1)%(3*SIZE_SAMPLE_FRQ);		
	if(g_DigitalWaveBuf_WrIdx==0)
	{
		g_DigitalWaveBufFull=1;
	}
	if(/*g_DigitalWaveBufFull&&*/g_DigitalWaveBuf_RdIdx!=g_DigitalWaveBuf_WrIdx)
	{
		u8 pleth;
		MaxMinSn_U32(g_DigitalWaveBuf,3*SIZE_SAMPLE_FRQ);		
		pleth=(value_max-(double)g_DigitalWaveBuf[g_DigitalWaveBuf_RdIdx])/(value_max-value_min)*100;//先*100，担心溢出
		g_DigitalWaveBuf_RdIdx=(g_DigitalWaveBuf_RdIdx+1)%(3*SIZE_SAMPLE_FRQ);
		if(flag)
		{
			FillBciBuffer(pleth,0,1);
		}
		flag=1-flag;
	}
}

void FillSpO2IRedPacket(u32 freq)
{
	static u8 flag=0;

	g_DigitalWaveBuf[g_DigitalWaveBuf_WrIdx]=freq;
	g_DigitalWaveBuf_WrIdx=(g_DigitalWaveBuf_WrIdx+1)%(3*SIZE_SAMPLE_FRQ);		
	if(g_DigitalWaveBuf_WrIdx==0)
	{
		g_DigitalWaveBufFull=1;
	}
	if(/*g_DigitalWaveBufFull&&*/g_DigitalWaveBuf_RdIdx!=g_DigitalWaveBuf_WrIdx)
	{
		u8 pleth;
		MaxMinSn_U32(g_DigitalWaveBuf,3*SIZE_SAMPLE_FRQ);		
		pleth=(value_max-(double)g_DigitalWaveBuf[g_DigitalWaveBuf_RdIdx])/(value_max-value_min)*100;//先*100，担心溢出
		g_DigitalWaveBuf_RdIdx=(g_DigitalWaveBuf_RdIdx+1)%(3*SIZE_SAMPLE_FRQ);
		if(flag)
		{
			FillBciBuffer(pleth,0,1);
		}
		flag=1-flag;
	}
}

void ClearSampleData(void)
{	
	g_PR[0]=0xffff;
	g_SpO2[0]=0x7f;
	g_HaveWaveData=0;
	g_HaveSignal=0;
	g_DigitalOxiSampleTime=0;
	ClearProcessOxiData(0);
}

void FastSignalSample(int times)
{
	static vu32 ir_freq=0,r_freq=0;

	switch(times)//血氧采样计数
	{
	case 28://28
		TIM_ITConfig(TIM1,TIM_IT_CC2,ENABLE);
		break;			
	case 48://48//IRED
		//WorkLed();
		TIM_ITConfig(TIM1, TIM_IT_CC2, DISABLE);
		ir_freq=CalcAverFreq(g_CaptureBuffer,g_CaptureCount);
		FillSpO2IRedPacket(ir_freq);
		if((ir_freq>MIN_FREQUENCY)&&(ir_freq<MAX_FREQUENCY))
		{
			g_HaveSignal=1;
		}
		else
		{
			ClearSampleData();
		}		
		break;			
	case 78://78
		TIM_ITConfig(TIM1,TIM_IT_CC2,ENABLE);
		break;	 
	case 95://98	//RED
//		WorkLed();
 		TIM_ITConfig(TIM1, TIM_IT_CC2, DISABLE);
		r_freq=CalcAverFreq(g_CaptureBuffer,g_CaptureCount);
		FillSpO2RedPacket(r_freq);
		if((r_freq>MIN_FREQUENCY)&&(r_freq<MAX_FREQUENCY))
   	{
			g_HaveSignal=1;
			DigitalOxi_Sampling(ir_freq,r_freq);
		}
		else
  	{
			ClearSampleData();		
		}	
		break;						
	default:
		return;
  }
	g_CaptureCount=0;
	g_CaptureIdx=0;	
}


