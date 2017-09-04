#ifndef __PLETHWAVE_H__
#define __PLETHWAVE_H__

#include "datatype.h"
#include "afe4490.h"

#define WAVE_BUFFER_SIZE		(6*AFE4490_SAMPLE_RATE)

extern u8 HeartBeatFlag[WAVE_BUFFER_SIZE];
extern vs32 Afe4490WaveBuf[2][WAVE_BUFFER_SIZE];		//afe4490每秒采的数据
extern vu16 g_Afe4490WaveBuf_WrIdx[2];				//数据头下标
extern vu16 g_Afe4490WaveBuf_RdIdx[2];	
extern vu8 g_WaveBufFull[2];

extern s32 g_MaxPlethData[2];		//最大血容积波形值
extern s32 g_MinPlethData[2];		//最小血容积波形值

extern u8 g_MaxPleth;					//最大血容积波形值

void AddSimulationWaveData(u8 type,s32 wavedata);
void CopyPlethData(u8 type,s32* pData);
void ClearSimulationWaveData(void);
void MaxMinPlethData(s32 *pBuf,u8 type);
u8 GetBciPlethData(u8 type);

#endif

