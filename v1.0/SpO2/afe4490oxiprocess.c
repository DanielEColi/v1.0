#include "afe4490oxiprocess.h"
#include "afe4490.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "global.h"
#include "spo2tools.h"
#include "algorithm.h"
#include "bciprotocol.h"
#include "plethwave.h"
#include "parameterprocess.h"

s32 FirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];		//仅用于血氧计算，不用于波形显示
s32 IntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];		//仅用于峰值查找，不用于血氧和脉率计算

#if(SAVE_ORIGINAL_DATA==1)		
s32 OriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//原始数据
s32 TmpOriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//原始数据
vu16 TmpOriginalOxiSignalBufIdx[2]={0,0};																		//存储原始数据的索引
#endif

//s32 TmpFirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];	//仅用于血氧计算，不用于波形显示
//vu16 TmpFirOxiSignalBufIdx[2]={0,0};																		//存储FIR数据的索引

//s32 TmpIntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];	//仅用于波形显示，不用于血氧计算
//s32 TmpIntOxiSignalBufIdx[2]={0,0};																	//用于计算当前已产生多少整系数算法后的数据
//vu8 TmpIntOxiSignalBufFullFirst[2]={0,0};														//整系数数据是否是第一次满
//vu8 Old_TmpIntOxiSignalBufFullFirst[2]={0,0};												//整系数数据是否是第一次满，以前的状态，用于和现在的状态对比

//void Afe4490_Oxi_Handle(void)
//{
//	u8 result=1;
//	
//	if(g_SenserOff)	//探头脱落，直接退出
//	{
//		g_Simulation_Process_Flag=0;				
//		return;
//	}
//	
//	if(g_Simulation_Process_Flag)
//	{
////		GPIO_SetBits(GPIOB,GPIO_Pin_12); 
//		g_Simulation_Process_Flag=0;		
//		
//		#if(SAVE_ORIGINAL_DATA==1)
//		memcpy(FirOxiSignalBuf[0],TmpOriginalOxiSignalBuf[0],SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE*sizeof(s32));
//		memcpy(FirOxiSignalBuf[1],TmpOriginalOxiSignalBuf[1],SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE*sizeof(s32));
////			GPIO_SetBits(GPIOB,GPIO_Pin_12); 
//		WorkLed();
//		result=OXI_Handle(TmpOriginalOxiSignalBufIdx[R_IDX],SIMULATION_OXI_IDX);
//		#else
//		memcpy(FirOxiSignalBuf[0],TmpFirOxiSignalBuf[0],SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE*sizeof(s32));
//		memcpy(FirOxiSignalBuf[1],TmpFirOxiSignalBuf[1],SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE*sizeof(s32));
////			GPIO_SetBits(GPIOB,GPIO_Pin_12); 
//		WorkLed();
//		result=OXI_Handle(TmpFirOxiSignalBufIdx[R_IDX],SIMULATION_OXI_IDX);
//		#endif
//			//GPIO_ResetBits(GPIOB,GPIO_Pin_12); 
////		GPIO_ResetBits(GPIOB,GPIO_Pin_12); 
//	}				
//	if(result==0)
//	{
//		if((!SpO2IsValid(g_SpO2))||(!PRIsValid(g_PR)))
//		{
//			g_SpO2=0x7f;
//			g_PR=0xffff;
//		}				
//		else if(g_ClearTimes<30)//15s数据消失
//		{
//			g_ClearTimes++;
//		}
//		else
//		{
////			ClearParameterData(SIMULATION_OXI_IDX);	
//		}		
//	}
//	else
//	{
//		g_ClearTimes=0;		
//	}
//}

