#ifndef _CIRCLEQUEUE_H_
#define _CIRCLEQUEUE_H_

#include "digitaloxiprocess.h"

#define BUFFER_SIZE		(HALF_SIZE_SAMPLE_FRQ)

typedef struct TCircleQueue
{
	vu16* 	pBuf;
	vu16 	head;
	vu16 	tail;
	vu16 	size;
	vu16 	count;
	vu8 	circleflag;		//��ʾ��������һȦ
}CIRCLEQUEUE,*PCIRCLEQUEUE;


extern void Reset(PCIRCLEQUEUE pQueue);
extern void InitQueue(void);
extern u8 PopQueueData(PCIRCLEQUEUE pQueue,u32* pData);
extern void PushDataToQueue(PCIRCLEQUEUE pQueue,u16 data);

#endif
