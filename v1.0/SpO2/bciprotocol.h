#ifndef __BCIPROTOCOL_H__
#define __BCIPROTOCOL_H__

#include "datatype.h"
#include "afe4490.h"


#define BCI_BUFFER_SIZE	64

#pragma pack(push)
#pragma pack(1)

typedef struct TByte1
{
	u8 SignalStrength:4;			//信号强度
	u8 SerachTooLong:1; 		//搜索时间太长
	u8 SensorNotConnect:1;  	//未接传感器
	u8 HeartBeat:1;  	//脉搏跳动声
	u8 SyncBit:1;		//同步位
}BYTE1;

typedef struct TByte2
{
	u8 Pleth:7;  		//体积描记图
	u8 SyncBit:1;		//同步位
}BYTE2;

typedef struct TByte3
{
	u8 Bar:4;  		//棒图
	u8 SensorError:1;//传感器错误
	u8 SearchPulse:1;//搜索脉搏
	u8 PulseH:1;		//脉率第7位
	u8 SyncBit:1;		//同步位
}BYTE3;

typedef struct TByte4
{
	u8 Pulse:7;  		//血氧
	u8 SyncBit:1;		//同步位
}BYTE4;

typedef struct TByte5
{
	u8 SpO2:7;  		//体积描记图
	u8 SyncBit:1;		//同步位
}BYTE5;

typedef struct TByte6
{
	u8 Reserve:7;  		//体积描记图
	u8 SyncBit:1;		//同步位
}BYTE6;

typedef struct TBciProtocol
{
	BYTE1	byte1;
	BYTE2	byte2;
	BYTE3	byte3;
	BYTE4	byte4;
	BYTE5	byte5;
	BYTE6	byte6;
}BCIPROTOCOL;

#pragma pack(pop)

extern u8 g_BciBuffer[BCI_BUFFER_SIZE];
extern volatile u16 g_BciBufferRdIdx;
extern volatile u16 g_BciBufferWrIdx;



void FillBciBuffer(u8 pleth,u8 heartbeat,u8 uart);

#endif

