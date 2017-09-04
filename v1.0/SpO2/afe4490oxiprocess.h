#ifndef __AFE4490_OXI_PROCESS_H__
#define __AFE4490_OXI_PROCESS_H__

#include "datatype.h"
#include "afe4490.h"

#define DELAY_COUNT		122		//int:82		 fir:40		int整系数延时82个，fir延时40个
#define DELAY_INT			82

#define ORIGINAL_ARRAY_COUNT	(SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT)

void Afe4490_Oxi_Handle(void);

extern s32 FirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];		//仅用于血氧计算，不用于波形显示
extern s32 IntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];		//仅用于峰值查找，不用于血氧和脉率计算

extern s32 TmpFirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];
extern vu16 TmpFirOxiSignalBufIdx[2];		

#if(SAVE_ORIGINAL_DATA==1)
extern s32 OriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//原始数据
extern s32 TmpOriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//原始数据
extern vu16 TmpOriginalOxiSignalBufIdx[2];																		//存储原始数据的索引
#endif

extern s32 TmpIntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];	//仅用于波形显示，不用于血氧计算
extern s32 TmpIntOxiSignalBufIdx[2];																	//用于计算当前已产生多少整系数算法后的数据
extern vu8 TmpIntOxiSignalBufFullFirst[2];														//整系数数据是否是第一次满
extern vu8 Old_TmpIntOxiSignalBufFullFirst[2];												//整系数数据是否是第一次满，以前的状态，用于和现在的状态对比

#endif

