#include "global.h"
extern 
//double B[FIR_ORDER] = {	//100hz fpass 8 fstop 15 order 40
//	0.00225181086, 0.001675570151,0.0002154390531,-0.002801523311,-0.005952642765,
//     -0.006830364,-0.003404424293, 0.004258663394,  0.01300100703,  0.01736020111,
//    0.01224981807, -0.00350425276, -0.02472930029, -0.04039776325, -0.03757329658,
//  -0.007474631071,  0.04915208742,   0.1202663928,   0.1858227402,   0.2251285166,
//     0.2251285166,   0.1858227402,   0.1202663928,  0.04915208742,-0.007474631071,
//   -0.03757329658, -0.04039776325, -0.02472930029, -0.00350425276,  0.01224981807,
//    0.01736020111,  0.01300100703, 0.004258663394,-0.003404424293,   -0.006830364,
//  -0.005952642765,-0.002801523311,0.0002154390531, 0.001675570151,  0.00225181086
//};


//COEFS Fir_Coefs={B,FIR_ORDER};

//vu8 g_RspBuffer[RSP_BUFFER_SIZE]; 	//回复数据包缓冲

//u8 g_Rx0Buffer[RX0_BUFFER_SIZE];
volatile u16 g_Rx0WrIdx=0;
volatile u16 g_Rx0RdIdx=0;

volatile u16 g_RspRdIdx=0;			//主动回复包索引
volatile u16 g_RspWrIdx=0;			//主动回复包索引

volatile u16 g_delay_tick=0;

vu8 g_AFE4490_DIAG_EN=0;		//AFE4490诊断功能使能

u8 g_ClearTimes=0;
u8 g_SpO2=0x7f;		//SpO2's value
u16 g_PR=0xffff;	//PR's value
u8 g_SenserOff=0x00;			//sensor off状态 bit0:sensor off  bit1:sensor not connect

vu8 g_100us_times=0;	//100us 定时器计时

vu8 g_Simulation_Process_Flag=0;	//Simulation process flag,now is 200ms

vu8 g_PRStepInterval=0;				//PR数据间隔

vu8 uartbuffer[32];

u16 g_BakPR=0xffff;			//备份用的PR，避免仅出现pr值，不出现spo2值的情况

vu8 g_CalcDataType=0;		//用于计算的数据类型，0:fir后的数据 1:int后的数据

u8 g_CurrentPILevel=15;


int dataready=0;
unsigned char str[20]={'\0'};

void SystickDelay1ms(u16 ms)
{
	g_delay_tick=ms;
	while(g_delay_tick)
	{
		;
	}	
}

//灯亮
void WorkLed(void)
{
#if(USE_AFE4490_EVM==1)	
	GPIO_ToggleBits(GPIOE,GPIO_Pin_2); 
#else
	GPIO_ToggleBits(GPIOB,GPIO_Pin_12); 
#endif	
}

u8 CheckSum(u8* buf,u8 len)
{
	u8 checksum=0;
	u8 i;
	for(i=0;i<len;i++)
	{
		checksum+=buf[i];
	}
	return checksum;
}


