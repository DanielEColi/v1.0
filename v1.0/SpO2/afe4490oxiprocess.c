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

s32 FirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];		//������Ѫ�����㣬�����ڲ�����ʾ
s32 IntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];		//�����ڷ�ֵ���ң�������Ѫ�������ʼ���

#if(SAVE_ORIGINAL_DATA==1)		
s32 OriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//ԭʼ����
s32 TmpOriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//ԭʼ����
vu16 TmpOriginalOxiSignalBufIdx[2]={0,0};																		//�洢ԭʼ���ݵ�����
#endif

//s32 TmpFirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];	//������Ѫ�����㣬�����ڲ�����ʾ
//vu16 TmpFirOxiSignalBufIdx[2]={0,0};																		//�洢FIR���ݵ�����

//s32 TmpIntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];	//�����ڲ�����ʾ��������Ѫ������
//s32 TmpIntOxiSignalBufIdx[2]={0,0};																	//���ڼ��㵱ǰ�Ѳ���������ϵ���㷨�������
//vu8 TmpIntOxiSignalBufFullFirst[2]={0,0};														//��ϵ�������Ƿ��ǵ�һ����
//vu8 Old_TmpIntOxiSignalBufFullFirst[2]={0,0};												//��ϵ�������Ƿ��ǵ�һ��������ǰ��״̬�����ں����ڵ�״̬�Ա�

//void Afe4490_Oxi_Handle(void)
//{
//	u8 result=1;
//	
//	if(g_SenserOff)	//̽ͷ���䣬ֱ���˳�
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
//		else if(g_ClearTimes<30)//15s������ʧ
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

