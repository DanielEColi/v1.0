#include "spo2tools.h"
#include "datatype.h"
#include "global.h"
#include "digitaloxiprocess.h"
#include "afe4490.h"

u32 sn_min,sn_max;

s32 s32_value_min,s32_value_max;

s32 GetMin(s32 a,s32 b)
{
	if(a<b)
	{
		return a;
	}
	return b;
}

s32 GetMax(s32 a,s32 b)
{
	if(a>b)
	{
		return a;
	}
	return b;
}

s32 FindMax(s32* pBuf,u16 len)
{
	s32 max=pBuf[0];
	u16 i;
	for(i=1;i<len;i++)
	{
		if(max<pBuf[i])
		{
			max=pBuf[i];
		}
	}
	return max;
}

s32 FindMin(s32* pBuf,u16 len)
{
	s32 min=pBuf[0];
	u16 i;
	for(i=1;i<len;i++)
	{
		if(min>pBuf[i])
		{
			min=pBuf[i];
		}
	}
	return min;
}

//U8Êý¾ÝÉýÐòÅÅÁÐ
void SortU8(u8* pBuf,u16 len)
{
	u16  i,j;
	u8 k;
	u8* pData=pBuf;
	for(i=0;i<len;i++)
	{
		for(j=i;j<len;j++)
		{
			if(pData[i]>pData[j])
			{
				k=pData[i];
				pData[i]=pData[j];
				pData[j]=k;
			}
		}
	}
}

void SortU16(u16* pBuf,u16 len)
{
	u16  i,j;
	u16 k;
	u16* pData=pBuf;
	for(i=0;i<len;i++)
	{
		for(j=i;j<len;j++)
		{
			if(pData[i]>pData[j])
			{
				k=pData[i];
				pData[i]=pData[j];
				pData[j]=k;
			}
		}
	}	
}

void MaxMinSn_S32(s32 *vf_buf,u16 len)
{
	u16 i;	
	s32_value_min=*vf_buf;
	s32_value_max=*vf_buf;
	
	for(i=1;i<len;i++)
	{
		if(vf_buf[i]<s32_value_min)
		{
			sn_min=i;
			s32_value_min=vf_buf[i];
		}
		else
		{
			if(vf_buf[i]>s32_value_max)
			{
				sn_max=i;
        s32_value_max=vf_buf[i];
			}
		}
	}	
}

u32 CalcAverFreq(vu32* pbuf,u16 len)
{
	u32 total=0;
	u32 freq=0;
//	u16 i;

	SortVU32(pbuf,len);

	if(len>7)
	{
/*		for(i=len>>2;i<len*3>>2;i++)
 		{
			total+=pbuf[i];
		}
		freq=total/(((len*3)>>2)-(len>>2));	*/
		total=Sum_VU32(pbuf,len);
		total=total-pbuf[0]-pbuf[1]-pbuf[len-1]-pbuf[len-2];
		freq=(total/(len-4));
	}
	else if(len>4)
	{
		total=Sum_VU32(pbuf,len);
		total=total-pbuf[0]-pbuf[len-1];
		freq=(total/(len-2));
	}
	else if(len>0)
	{
		total=Sum_VU32(pbuf,len);
		freq=(total/len);
	}
	freq&=0xffff;
	return freq;
}

void SortFloat(float* pBuf,u16 len)
{
	u16  i,j;
	float k;
	float* pData=pBuf;
	for(i=0;i<len;i++)
	{
		for(j=i;j<len;j++)
		{
			if(pData[i]>pData[j])
			{
				k=*pData;
				*pData=pData[j];
				pData[j]=k;
			}
		}
	}
}

void SortVU32(vu32 *pBuf,u16 len)
{	
	u16  i,j;
	vu32 k;
	vu32* pData=pBuf;
	for(i=0;i<len;i++)
	{
		for(j=i;j<len;j++)
		{
			if(pData[i]>pData[j])
			{
				k=*pData;
				*pData=pData[j];
				pData[j]=k;
			}
		}
	}
}

u32 Sum_VU32(vu32 *vf_buf,u16 len)
{
	u16 i;
	u32 j=0;
	for(i=0;i<len;i++)
	{
		j+=vf_buf[i];
	}
	return j;
}

u8 PRIsValid(u16 value)
{
	if(value>PR_MAX||value<PR_MIN)
	{
		return 0;
	}
	return 1;	
}

u8 SpO2IsValid(u8 value)
{
	if(value>SPO2_MAX||value<SPO2_MIN)
	{
		return 0;
	}
	return 1;	
}

void sort_dunk(u16 *vf_buf, u16 len_data)
{
	u8 i;
	u16 j,k;
	u16 *data_pt;
	if(len_data==0)
	{
		return;
	}
	data_pt=vf_buf;//data_pt=&vf_buf[0];
	for(j = len_data  - 1;j>0;j--)
	{
		for(i=j;i>0;i--)
		{
			if(*data_pt>*(data_pt+i))
			{
				k=*data_pt;
				*data_pt=*(data_pt+i);
				*(data_pt+i)=k;
			}
		}
		data_pt+=1;
	}
}

void sort_dunk_float(float *vf_buf, u8 len_data)
{
	u8 i,j;
	float k;
	float *pt_floatdata;
	pt_floatdata = vf_buf;//pt_floatdata=&vf_buf[0];
	for(j=len_data-1; j>0; j--)
	{
		for(i=j; i>0; i--)
		{
			if(*pt_floatdata > *(pt_floatdata+i))
			{
				k = *pt_floatdata;
				*pt_floatdata = *(pt_floatdata+i);
				*(pt_floatdata+i) = k;
			}
		}
		pt_floatdata += 1;
	}
}

void sort_dunk_byte(u8 *vf_buf, u8 len_data)
{
	u8 i,j,k;
	u8 *data_pt_byte;	
	data_pt_byte=vf_buf;//data_pt_byte=&vf_buf[0];
	for(j = len_data-1;j>0;j--)
	{
		for(i=j;i>0;i--)
		{
		if(*data_pt_byte>*(data_pt_byte+i))
			{
			k=*data_pt_byte;
			*data_pt_byte=*(data_pt_byte+i);
			*(data_pt_byte+i)=k;
			}
		}
		data_pt_byte+=1;
	}
}

u32 GetValueInterval(u32 a,u32 b)
{
	if(a>b)
	{
		return a-b;
	}
	return b-a;
}

s32 GetS32ValueInterval(s32 a,s32 b)
{
	if(a>b)
	{
		return a-b;
	}
	return b-a;
}

u16 CalcPR(u16* pr,u8 len)
{
	u8 i;
	u16 sum=0;
	u16 calclen=(len+1)/2;
	u16 idx=(len-calclen)/2;
	SortU16(pr,len);
	for(i=0;i<calclen;i++)
	{
		sum+=pr[idx++];
	}
	sum=sum/calclen;
	return sum;
}

u8 CalcSpO2(u8* spo2,u8 len)
{
	u8 i;
	u16 sum=0;
	u16 calclen=(len+1)/2;
	u16 idx=(len-calclen)/2;
	SortU8(spo2,len);
	for(i=0;i<calclen;i++)
	{
		sum+=spo2[idx++];
	}
	sum=sum/calclen;
	return sum;
}

float CalcRValue(float* rvalue,u8 len)
{
	u8 i;
	float sum=0.0;
	u16 calclen=(len+1)/2;
	u16 idx=(len-calclen)/2;
	sort_dunk_float(rvalue,len);
	for(i=0;i<calclen;i++)
	{
		sum+=rvalue[idx++];
	}
	sum=sum/calclen;
	return sum;
}

u8 CheckSpO2(u8 spo2)
{
	if(spo2>=SPO2_MAX)
	{
		spo2=SPO2_MAX-1;
	}
	else if(spo2<SPO2_MIN)   
	{
			spo2=0x7f;
	}
	return spo2;
}






