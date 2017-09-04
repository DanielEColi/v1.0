#ifndef __IIRFIRLTER_H__
#define __IIRFIRLTER_H__

#include "datatype.h"

void IIRReset(u8 type);
u32 IIRFilter(u32 in,u8 type);

void iirarma_stm32(void *y, void *x,  short *h2, short *h1, int ny);

#endif

