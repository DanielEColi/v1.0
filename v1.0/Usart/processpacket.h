#ifndef __PROCESSPACKET_H__
#define __PROCESSPACKET_H__

#include "datatype.h"

void AnalyseUart1Packet(void);

extern u8 g_PatientType;		//0:����  1:��ͯ  2:������
extern u8 g_AverCycle;			//0:1 beat 1:4 beat 2:8 beat 3:16 beat

#endif

