#include "circlequeue.h"
#include <stdlib.h>

void Alloc(PCIRCLEQUEUE pQueue,u16 size)
{
	pQueue->size=size;

	pQueue->pBuf=(vu16*)malloc(size*sizeof(vu16));
}

void Reset(PCIRCLEQUEUE pQueue)
{
	pQueue->head=0;
	pQueue->tail=0;	
	pQueue->count=0;
	pQueue->circleflag=0;
}

u8 IsFull(PCIRCLEQUEUE pQueue)
{
	if(pQueue->head==((pQueue->tail+1)%pQueue->size))	//如果头在尾后加1,则是满的
	{
		return 1;
	}
	return 0;
}

u8 IsEmpty(PCIRCLEQUEUE pQueue)
{
	if(pQueue->head==pQueue->tail)	//如果头与尾相等
	{
		return 1;
	}
	return 0;
}

#define FILTER_TYPE		0

void PushDataToQueue(PCIRCLEQUEUE pQueue,u16 data)
{
#if(FILTER_TYPE==1)	//
	static u32 bak_data[5]={0,0,0,0,0};
	static u8 i=0;	
	static u16 cnt=0;
	static long filter_data=0;
	if(cnt>4)
	{
		for(i=0;i<4;i++)		
		{
			bak_data[i]=bak_data[i+1];
		}
		bak_data[4]=data;
		filter_data=(12*bak_data[1]+17*bak_data[2]+12*bak_data[3]-3*bak_data[0]-3*bak_data[4])/35;
		data=(filter_data&0xffff);
	}
	else
	{
		bak_data[cnt]=data;		
		cnt++;
		return;
	}
#endif
#if(FILTER_TYPE==2)		  //IIR
	static u8 i=0;	
	static u16 oldy[2]={0,0};
	static u16 cnt1=0;
	static u16 bak_data[17]={0};
	static u16 cnt2=0;
	static u32 calc_data=0;
	if(cnt2>16)
	{
		for(i=0;i<16;i++)		
		{
			bak_data[i]=bak_data[i+1];
		}
		bak_data[16]=data;
		if(cnt1>0)
		{
			calc_data=data+bak_data[0]-2*bak_data[8]+2*oldy[1]-oldy[0];			
			//data=data+bak_data[0]-2*bak_data[8]+2*oldy[1]-oldy[0];		
		}
		else
		{
			calc_data=data-2*bak_data[8]+bak_data[0];
			//data=data-2*bak_data[8]+bak_data[0];
		}
		calc_data>>=8;
		if(cnt1>1)
		{
			oldy[0]=oldy[1];
			//oldy[1]=data;	
			oldy[1]=calc_data;	
		}
		else
		{
			oldy[cnt1]=calc_data;
			//oldy[cnt1]=data;
			cnt1++;
		}
	}
	else
	{
		bak_data[cnt2]=data;		
		cnt2++;
		return;
	}
#endif

	//if(IsFull(pQueue)==0)
	{
/*		if(pQueue->tail==32)
		{
			data=data;
		} */
		pQueue->pBuf[pQueue->tail]=data;
		pQueue->tail++;
		if(pQueue->tail>=pQueue->size)
		{
			pQueue->tail=0;
		}		
		if(pQueue->tail==0)
		{
			pQueue->circleflag=1;
		}
		if(pQueue->count<pQueue->size)
		{
			pQueue->count++;
		}
	}
}

u8 PopQueueData(PCIRCLEQUEUE pQueue,u32* pData)
{
	if(IsEmpty(pQueue)==0)
	{
		*pData=pQueue->pBuf[pQueue->head];
		pQueue->head=(pQueue->head+1)%pQueue->size;
		return 1;
	}
	return 0;
}


