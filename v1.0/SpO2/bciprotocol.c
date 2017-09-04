#include "bciprotocol.h"
#include "global.h"
#include "afe4490.h"
#include "spo2tools.h"
#include "piprocess.h"
#include "plethwave.h"

vu16 g_SearchPulseTime=0;

u8 g_BciBuffer[BCI_BUFFER_SIZE];
vu16 g_BciBufferRdIdx=0;
vu16 g_BciBufferWrIdx=0;

BCIPROTOCOL bciprotocol;

void FillBciBuffer(u8 pleth,u8 heartbeat,u8 type)	//0:ir 1:r
{
	u8 i,bar;
	u8* pBuf=(u8*)&bciprotocol;
	
	bciprotocol.byte1.SignalStrength=g_CurrentPILevel;		//信号强度
	bciprotocol.byte1.SerachTooLong=0; 		//搜索时间太长
	bciprotocol.byte1.SensorNotConnect=((g_SenserOff&0x02)?1:0);  	//未接传感器

	bciprotocol.byte1.HeartBeat=heartbeat;  			//脉搏跳动声
	bciprotocol.byte1.SyncBit=1;
	bciprotocol.byte2.SyncBit=0;		//同步位
	bciprotocol.byte3.SensorError=(bciprotocol.byte1.SensorNotConnect)?1:(g_SenserOff&0x01);//传感器错误
//	bciprotocol.byte3.PulseH=g_PR[1]>0x7f?1:0;		//脉率第7位
	bciprotocol.byte3.SearchPulse=0;//搜索脉搏
	bciprotocol.byte3.SyncBit=0;		//同步位
	bciprotocol.byte4.SyncBit=0;		//同步位
	bciprotocol.byte5.SyncBit=0;		//同步位
	bciprotocol.byte6.SyncBit=0;		//同步位
	
	bciprotocol.byte6.Reserve=0;

	if(type==IR_IDX)//红外血氧从uart2
	{
		if(g_SenserOff)
		{
			bciprotocol.byte1.SignalStrength=0;
			bciprotocol.byte3.Bar=0;  		//棒图
			bciprotocol.byte2.Pleth=0x7f;  		//体积描记图
			bciprotocol.byte3.PulseH=1;		//脉率第7位
			bciprotocol.byte4.Pulse=0x7f;
			bciprotocol.byte5.SpO2=0x7f;  		//血氧			
			g_SearchPulseTime=0;
		}
		else
		{
			bar=(u16)pleth*g_CurrentPILevel/g_MaxPleth;
			if((float)pleth*g_CurrentPILevel/g_MaxPleth-bar>0.5)
			{
				bar+=1;
			}
			bciprotocol.byte3.Bar=bar;//棒图
			bciprotocol.byte2.Pleth=pleth;  		//体积描记图
			bciprotocol.byte3.PulseH=g_PR>0x7f?1:0;		//脉率第7位
			bciprotocol.byte4.Pulse=g_PR&0x7f;
			bciprotocol.byte5.SpO2=g_SpO2;  		//血氧
			if(PRIsValid(g_PR))
			{
				g_SearchPulseTime=0;
			}
			else
			{
				g_SearchPulseTime++;
				bciprotocol.byte3.SearchPulse=1;//搜索脉搏
				if(g_SearchPulseTime>=500)
				{
					bciprotocol.byte1.SerachTooLong=1; 		//搜索时间太长
				}
			}
		}
		for(i=0;i<6;i++)
		{
			g_BciBuffer[g_BciBufferWrIdx++]=pBuf[i];
			g_BciBufferWrIdx&=(BCI_BUFFER_SIZE-1);
		}
#if(USE_AFE4490_EVM==1)	
		USART_ITConfig(USART2,USART_IT_TXE,ENABLE);
#else
		USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
#endif		
	}	
	else	//红光从uart6
	{		
		if(g_SenserOff)
		{
			bciprotocol.byte3.Bar=0;  		//棒图
			bciprotocol.byte2.Pleth=0x7f;  		//体积描记图
			bciprotocol.byte3.PulseH=1;		//脉率第7位
			bciprotocol.byte4.Pulse=0x7f;
			bciprotocol.byte5.SpO2=0x7f;  		//血氧			
			g_SearchPulseTime=0;
		}
		else
		{
			//bciprotocol.byte3.Bar=(u16)pleth*16/100;  		//棒图
			bciprotocol.byte3.Bar=(u16)pleth*g_CurrentPILevel/g_MaxPleth;//棒图
			bciprotocol.byte2.Pleth=pleth;  		//体积描记图
			bciprotocol.byte3.PulseH=g_PR>0x7f?1:0;		//脉率第7位
			bciprotocol.byte4.Pulse=g_PR&0x7f;
			bciprotocol.byte5.SpO2=g_SpO2;  		//血氧
			if(PRIsValid(g_PR))
			{
				g_SearchPulseTime=0;
			}
			else
			{
				g_SearchPulseTime++;
				bciprotocol.byte3.SearchPulse=1;//搜索脉搏
				if(g_SearchPulseTime>=500)
				{
					bciprotocol.byte1.SerachTooLong=1; 		//搜索时间太长
				}
			}
		}				
		bciprotocol.byte3.PulseH=g_PR>0x7f?1:0;		//脉率第7位
		bciprotocol.byte4.Pulse=g_PR&0x7f;
		bciprotocol.byte5.SpO2=g_SpO2;  		//血氧
		for(i=0;i<6;i++)
		{
//			g_RspBuffer[g_RspWrIdx++]=pBuf[i];
			g_RspWrIdx&=(RSP_BUFFER_SIZE-1);
		}
#if(USE_AFE4490_EVM==1)	
		USART_ITConfig(USART6,USART_IT_TXE,ENABLE);
#else
		USART_ITConfig(UART4,USART_IT_TXE,ENABLE);
#endif		
		
	}
}



