#ifndef __INTCOEFF_H__
#define __INTCOEFF_H__

#include "datatype.h"

#define USE_CHENHUA_METHOD	0

#define CHANNEL	2

#if(USE_CHENHUA_METHOD==1)

#if(SAMPLE_100HZ==1)
#define ICINLEN		241	 //输入缓冲长度
#define ICOUTLEN 	5	 //输出缓冲长度
#else
#define ICINLEN		420	 //输入缓冲长度
#define ICOUTLEN 	15	 //输出缓冲长度
#endif

#else

/*#define XBUFFER_SIZE	401
#define Y1BUFFER_SIZE	401
#define Y2BUFFER_SIZE	3*/

#define XBUFFER_SIZE	81
#define Y1BUFFER_SIZE	81
#define Y2BUFFER_SIZE	3

#endif

extern double ICOutBuf[CHANNEL];				 //实际输出
s32 IntCoeff(s32 val,u8 type);
void ResetIntCoeff(void);

#endif

