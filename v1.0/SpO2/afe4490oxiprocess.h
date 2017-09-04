#ifndef __AFE4490_OXI_PROCESS_H__
#define __AFE4490_OXI_PROCESS_H__

#include "datatype.h"
#include "afe4490.h"

#define DELAY_COUNT		122		//int:82		 fir:40		int��ϵ����ʱ82����fir��ʱ40��
#define DELAY_INT			82

#define ORIGINAL_ARRAY_COUNT	(SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT)

void Afe4490_Oxi_Handle(void);

extern s32 FirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];		//������Ѫ�����㣬�����ڲ�����ʾ
extern s32 IntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];		//�����ڷ�ֵ���ң�������Ѫ�������ʼ���

extern s32 TmpFirOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE];
extern vu16 TmpFirOxiSignalBufIdx[2];		

#if(SAVE_ORIGINAL_DATA==1)
extern s32 OriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//ԭʼ����
extern s32 TmpOriginalOxiSignalBuf[2][ORIGINAL_ARRAY_COUNT];	//ԭʼ����
extern vu16 TmpOriginalOxiSignalBufIdx[2];																		//�洢ԭʼ���ݵ�����
#endif

extern s32 TmpIntOxiSignalBuf[2][SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT];	//�����ڲ�����ʾ��������Ѫ������
extern s32 TmpIntOxiSignalBufIdx[2];																	//���ڼ��㵱ǰ�Ѳ���������ϵ���㷨�������
extern vu8 TmpIntOxiSignalBufFullFirst[2];														//��ϵ�������Ƿ��ǵ�һ����
extern vu8 Old_TmpIntOxiSignalBufFullFirst[2];												//��ϵ�������Ƿ��ǵ�һ��������ǰ��״̬�����ں����ڵ�״̬�Ա�

#endif

