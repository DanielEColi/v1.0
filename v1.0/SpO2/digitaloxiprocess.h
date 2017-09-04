#ifndef __PROCESSOXI_H__
#define __PROCESSOXI_H__

#include "datatype.h"
#include "algorithm.h"

#define HALF_SIZE_SAMPLE_FRQ	(50)

#define FIR_CALC_NUM      	24

extern s32 FirDigitalOxiSignalBuf[2][TIMER_SECOND*SIZE_SAMPLE_FRQ];

void ClearProcessOxiData(u8 type);
void Digital_Oxi_Handle(void);
u8 GetPlethWaveData(void);

#endif

