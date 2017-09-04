#ifndef __PI_PROCESS_H__
#define __PI_PROCESS_H__
//PI ¹àÖùÖ¸Êý¼ÆËã

#include "datatype.h"

#define PI_ARRAY_SIZE			300

extern u8 g_CurrentPILevel;

void AddPIData(s32 ir_adc,s32 r_adc);
void ClearPIData(void);

#endif

