#ifndef __BCIPROTOCOL_H__
#define __BCIPROTOCOL_H__

#include "datatype.h"
#include "afe4490.h"


#define BCI_BUFFER_SIZE	64

#pragma pack(push)
#pragma pack(1)

typedef struct TByte1
{
	u8 SignalStrength:4;			//�ź�ǿ��
	u8 SerachTooLong:1; 		//����ʱ��̫��
	u8 SensorNotConnect:1;  	//δ�Ӵ�����
	u8 HeartBeat:1;  	//����������
	u8 SyncBit:1;		//ͬ��λ
}BYTE1;

typedef struct TByte2
{
	u8 Pleth:7;  		//������ͼ
	u8 SyncBit:1;		//ͬ��λ
}BYTE2;

typedef struct TByte3
{
	u8 Bar:4;  		//��ͼ
	u8 SensorError:1;//����������
	u8 SearchPulse:1;//��������
	u8 PulseH:1;		//���ʵ�7λ
	u8 SyncBit:1;		//ͬ��λ
}BYTE3;

typedef struct TByte4
{
	u8 Pulse:7;  		//Ѫ��
	u8 SyncBit:1;		//ͬ��λ
}BYTE4;

typedef struct TByte5
{
	u8 SpO2:7;  		//������ͼ
	u8 SyncBit:1;		//ͬ��λ
}BYTE5;

typedef struct TByte6
{
	u8 Reserve:7;  		//������ͼ
	u8 SyncBit:1;		//ͬ��λ
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

