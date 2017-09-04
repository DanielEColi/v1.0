#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <math.h>
#include <stdlib.h>
#include "stdbool.h"
#include "datatype.h"
#include "stm32f2xx_conf.h"
#include "stm32_dsp.h"
#include "uart.h"
#include "strategy.h"
#include "afe4490.h"


#define FIR_ORDER							40//24


extern COEFS Fir_Coefs;
//extern short B[FIR_ORDER];
//extern double B[FIR_ORDER];

#define SOFTWARE_VERSION		0x0001
#define MODULE_NUMBER			0x0000

#define TX0_BUFFER_SIZE			256
#define RSP_BUFFER_SIZE			2048 //256
#define RX0_BUFFER_SIZE			256
#define RX0_PACKAGE_SIZE		256


#define WORKLED_TICK			1000

//extern vu8 g_RspBuffer[RSP_BUFFER_SIZE];

extern volatile u16 g_RspRdIdx;
extern volatile u16 g_RspWrIdx;

//extern u8 g_Rx0Buffer[RX0_BUFFER_SIZE];


extern volatile u16 g_Rx0WrIdx;
extern volatile u16 g_Rx0RdIdx;

extern volatile u16 g_Rx1WrIdx;
extern volatile u16 g_Rx1RdIdx;

extern vu8 g_AFE4490_DIAG_EN;		//AFE4490���ʹ��

extern volatile u16 g_delay_tick;

extern u8 g_ClearTimes;
extern u8 g_DigitalOxiSampleTime; 
extern u8 g_SpO2;		//SpO2's value
extern u16 g_PR;	//PR's value
extern u8 g_SenserOff;			//sensor off״̬ bit0:sensor off  bit1:sensor not connect

extern vu8 g_Simulation_Process_Flag;	//simulation process flag

extern vu8 g_PRStepInterval;				//PR���ݼ��

extern u16 g_BakPR;			//�����õ�PR�����������prֵ��������spo2ֵ�����

extern vu8 g_100us_times;	//100us ��ʱ����ʱ

extern vu8 uartbuffer[32];

extern vu8 g_CalcDataType;		//���ڼ�����������ͣ�0:fir������� 1:int�������

extern int dataready;
extern unsigned char str[20];
void SystickDelay1ms(u16 ms);
void WorkLed(void);
u8 CheckSum(u8* buf,u8 len);

#endif

