#include "plethwave.h"
#include "bciprotocol.h"
#include <string.h>
#include "global.h"

u8 HeartBeatFlag[WAVE_BUFFER_SIZE];					//心跳声

vs32 Afe4490WaveBuf[2][WAVE_BUFFER_SIZE];		//afe4490每秒采的数据
vu16 g_Afe4490WaveBuf_WrIdx[2]={0,0};				//数据头下标
vu16 g_Afe4490WaveBuf_RdIdx[2]={0,0};	
vu8 g_WaveBufFull[2]={0,0};

s32 g_MaxPlethData[2]={0,0};		//最大血容积波形值
s32 g_MinPlethData[2]={0,0};		//最小血容积波形值

vu8 g_HeartBeat=0;					//心跳音标志
vu16 g_HeartBeatInterval=0;	//心跳音间隔，防止数据抖动后产生多个心跳音

u8 g_MaxPleth=100;					//最大血容积波形值

u8 CheckHeartBeat(u8 LedType)
{
	s32 min;
	u16 i;
	u16 idx,count;
	if(LedType==R_IDX||g_PRStepInterval==0)
	{
		return 0;
	}
	count=((g_PRStepInterval>>1)-g_PRStepInterval/10);
	idx=(g_Afe4490WaveBuf_WrIdx[0]+WAVE_BUFFER_SIZE-count)%WAVE_BUFFER_SIZE;
	min=Afe4490WaveBuf[0][g_Afe4490WaveBuf_WrIdx[0]];
	g_HeartBeatInterval++;
	for(i=0;i<count;i++)
	{
		if(Afe4490WaveBuf[0][idx]<min)
		{
			g_HeartBeat=0;
			return 0;
		}
		idx=(idx+1)%WAVE_BUFFER_SIZE;
	}
	if((g_HeartBeat==0)&&(g_HeartBeatInterval>(g_PRStepInterval>>2)))//后面一个判断是防数据抖动
	{
		g_HeartBeatInterval=0;
		g_HeartBeat=1;
		return 1;
	}	
	return 0;
}

void AddSimulationWaveData(u8 LedType,s32 wavedata)
{
	Afe4490WaveBuf[LedType][g_Afe4490WaveBuf_WrIdx[LedType]]=wavedata;
	if(LedType==0)
	{
		HeartBeatFlag[g_Afe4490WaveBuf_WrIdx[LedType]]=CheckHeartBeat(LedType);
	}
	g_Afe4490WaveBuf_WrIdx[LedType]=(g_Afe4490WaveBuf_WrIdx[LedType]+1)%(WAVE_BUFFER_SIZE);		
	if(g_Afe4490WaveBuf_WrIdx[LedType]==0)
	{
		g_WaveBufFull[LedType]=1;
	}
}

void CopyPlethData(u8 type,s32* pData)
{
	g_Afe4490WaveBuf_WrIdx[type]=0;
	g_Afe4490WaveBuf_RdIdx[type]=0;
	g_WaveBufFull[type]=1;
	memcpy((void*)Afe4490WaveBuf[type],(void*)pData,sizeof(s32)*WAVE_BUFFER_SIZE);	
}

void ClearSimulationWaveData(void)
{
	g_MaxPleth=100;
	memset((void*)Afe4490WaveBuf,0,sizeof(Afe4490WaveBuf));
	memset((void*)g_Afe4490WaveBuf_WrIdx,0,sizeof(g_Afe4490WaveBuf_WrIdx));
	memset((void*)g_Afe4490WaveBuf_RdIdx,0,sizeof(g_Afe4490WaveBuf_RdIdx));
	memset((void*)g_WaveBufFull,0,sizeof(g_WaveBufFull));
}

void MaxMinPlethData(s32 *pBuf,u8 type)
{
	u16 i;
	u16 len;
	
	g_MaxPlethData[type]=pBuf[0];
	g_MinPlethData[type]=pBuf[0];
	
	if(g_WaveBufFull[type])
	{
		len=WAVE_BUFFER_SIZE-1;
	}
	else
	{
		len=g_Afe4490WaveBuf_WrIdx[type];
	}
	
	for(i=0;i<len;i++)
	{
		if(pBuf[i])
		{
			g_MaxPlethData[type]=pBuf[i];
			g_MinPlethData[type]=pBuf[i];	
			break;
		}
	}	
	for(;i<len;i++)
	{
		if(pBuf[i]>g_MaxPlethData[type])
		{
			g_MaxPlethData[type]=pBuf[i];
		}
		else if(pBuf[i])
		{
			if(pBuf[i]<g_MinPlethData[type])
      {
				g_MinPlethData[type]=pBuf[i];
			}
		}
	}	
}

u8 GetBciPlethData(u8 type)
{	
	u8 pleth,heartbeat=0;

	while(g_Afe4490WaveBuf_RdIdx[type]!=g_Afe4490WaveBuf_WrIdx[type])
	{
		g_Afe4490WaveBuf_RdIdx[type]=(g_Afe4490WaveBuf_RdIdx[type]+1)%(WAVE_BUFFER_SIZE);
		if(type==IR_IDX)
		{
			heartbeat|=HeartBeatFlag[g_Afe4490WaveBuf_RdIdx[type]];			
		}		
	}	
	g_Afe4490WaveBuf_RdIdx[type]=((g_Afe4490WaveBuf_WrIdx[type]+WAVE_BUFFER_SIZE-1)%WAVE_BUFFER_SIZE);	//取当前保存的最新数据
	MaxMinPlethData((s32*)Afe4490WaveBuf[type],type);			
	pleth=(g_MaxPlethData[type]-(double)Afe4490WaveBuf[type][g_Afe4490WaveBuf_RdIdx[type]])/(g_MaxPlethData[type]-g_MinPlethData[type])*g_MaxPleth;//先*100，担心溢出
	
/*	for(i=0;i<2;i++)
	{
		if(g_Afe4490WaveBuf_RdIdx[type]!=g_Afe4490WaveBuf_WrIdx[type])
		{
			if(i)
			{
				MaxMinPlethData((s32*)Afe4490WaveBuf[type],type);		
				pleth=(g_MaxPlethData[type]-(double)Afe4490WaveBuf[type][g_Afe4490WaveBuf_RdIdx[type]])/(g_MaxPlethData[type]-g_MinPlethData[type])*g_MaxPleth;//先*100，担心溢出
			}
			if(type==IR_IDX)
			{
				heartbeat|=HeartBeatFlag[g_Afe4490WaveBuf_RdIdx[type]];			
			}
			g_Afe4490WaveBuf_RdIdx[type]=(g_Afe4490WaveBuf_RdIdx[type]+1)%(WAVE_BUFFER_SIZE);	
		}
		else 
		{
			return 0;
		}
	}*/
	FillBciBuffer(pleth,heartbeat,type);
	return 1;
}


