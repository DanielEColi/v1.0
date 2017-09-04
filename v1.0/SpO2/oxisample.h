#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "datatype.h"
#include "circlequeue.h"

#define IRED_PIN          	GPIO_Pin_13
#define RED_PIN             GPIO_Pin_12
 
#define MAX_FREQUENCY		0xffff
#define MIN_FREQUENCY		0x0400
#define MAX_CAPTURE_TIMES	32

void SlowSignalSample(int times);
void FastSignalSample(int times);
void ClearSampleData(void);

extern u16 g_wavebuffer_idx;
extern u8 g_iredwavebuffer[];


extern vu32 g_CaptureBuffer[MAX_CAPTURE_TIMES];
extern vu8  g_CaptureIdx;
extern vu8  g_CaptureCount;

extern u16 DigitalOxiHalfSecBuf[2][HALF_SIZE_SAMPLE_FRQ];		//digital oxi半秒采的数据
extern u16 g_DigitalOxiSecBuf_Idx;								//数据头下标
extern vu8 g_DigitalOxiHalfSecFullFlag;					//1/2s data full flag




#endif


