#ifndef __INTCOEFF_H__
#define __INTCOEFF_H__

#include "datatype.h"

#define USE_CHENHUA_METHOD	0

#define CHANNEL	2

#if(USE_CHENHUA_METHOD==1)

#if(SAMPLE_100HZ==1)
#define ICINLEN		241	 //���뻺�峤��
#define ICOUTLEN 	5	 //������峤��
#else
#define ICINLEN		420	 //���뻺�峤��
#define ICOUTLEN 	15	 //������峤��
#endif

#else

/*#define XBUFFER_SIZE	401
#define Y1BUFFER_SIZE	401
#define Y2BUFFER_SIZE	3*/

#define XBUFFER_SIZE	81
#define Y1BUFFER_SIZE	81
#define Y2BUFFER_SIZE	3

#endif

extern double ICOutBuf[CHANNEL];				 //ʵ�����
s32 IntCoeff(s32 val,u8 type);
void ResetIntCoeff(void);

#endif

