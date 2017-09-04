#ifndef __PROCESSPACKET_H__
#define __PROCESSPACKET_H__

#include "datatype.h"

void AnalyseUart1Packet(void);

extern u8 g_PatientType;		//0:成人  1:儿童  2:新生儿
extern u8 g_AverCycle;			//0:1 beat 1:4 beat 2:8 beat 3:16 beat

#endif

