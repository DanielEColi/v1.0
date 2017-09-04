#ifndef __PLETHWAVE_H__
#define __PLETHWAVE_H__

#include "datatype.h"
#include "afe4490.h"

#define WAVE_BUFFER_SIZE		(6*AFE4490_SAMPLE_RATE)

extern u8 HeartBeatFlag[WAVE_BUFFER_SIZE];
extern vs32 Afe4490WaveBuf[2][WAVE_BUFFER_SIZE];		//afe4490ÿ��ɵ�����
extern vu16 g_Afe4490WaveBuf_WrIdx[2];				//����ͷ�±�
extern vu16 g_Afe4490WaveBuf_RdIdx[2];	
extern vu8 g_WaveBufFull[2];

extern s32 g_MaxPlethData[2];		//���Ѫ�ݻ�����ֵ
extern s32 g_MinPlethData[2];		//��СѪ�ݻ�����ֵ

extern u8 g_MaxPleth;					//���Ѫ�ݻ�����ֵ

void AddSimulationWaveData(u8 type,s32 wavedata);
void CopyPlethData(u8 type,s32* pData);
void ClearSimulationWaveData(void);
void MaxMinPlethData(s32 *pBuf,u8 type);
u8 GetBciPlethData(u8 type);

#endif

