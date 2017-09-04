#include "algorithm.h"
#include "afe4490oxiprocess.h"
#include "digitaloxiprocess.h"
#include "spo2tools.h"
#include "global.h"
#include "otheralgorithm.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "bciprotocol.h"
#include "iirfilter.h"
#include "intcoeff.h"
#include "firhighpass.h"
#include "plethwave.h"
#include "parameterprocess.h"
#include "piprocess.h"
#include "ledcurrent.h"
#include "curvefitting.h"
#include "threshold.h"
#include "leastsquaremethod.h"

vu16 Bak_Bot_Step_IR[2][INFLECT_POINT_MAX];	//备份的波谷值，用于计算脉搏音位置
vu8 Bak_Bot1_Num_IR[2];

vu8 g_PR_V_Idx[2]={0,0};
vu8 g_PR_V_Count[2]={0,0};

vu8 g_R_V_Idx[2]={0,0};
vu8 g_R_V_Count[2]={0,0};

float g_R_Value[2];//, g_R_Value_bak;
float buffer_rline[2][SIZE_SPO2];

u16 g_Bak_PR_Value[2]={0xffff,0xffff};
u16 bak_buffer_pr[2][BAK_SIZE_PR];
vu8 g_Bak_PR_V_Idx[2]={0,0};
vu8 g_Bak_PR_V_Count[2]={0,0};

float g_Bak_R_Value[2]={0.0,0.0};
float bak_buffer_rline[2][SIZE_SPO2];
vu8 g_Bak_R_V_Idx[2]={0,0};
vu8 g_Bak_R_V_Count[2]={0,0};

u16 buffer_pr[2][SIZE_PR];

vs32 FirLowPassTmpBuffer[2][FIR_ORDER];//临时存放fir之前的原始数据，每到一个处理一次
vu8 FirLowPassTmpInIdx[2]={0,0};

vu8 g_SampleCount=0;			//采样数据统计，主要是用于计时，用于计算血氧与心率

u8 g_DelayIntCoef[2]={0,0};			//延时82个数据显示，用于与fir波形数据有续结合
u16 g_LostIntDataNum[2]={300,300};	//丢失前300个int数据

u16 g_LostFirDataNum[2]={50,50};	//丢失前50个fir数据

//u8 sizedouble;
//u8 sizefloat;
s32 GetFilterData(const s32* in, double* b,u16 order)
{
	double data=0;
  u16 i;
	
//	sizedouble=sizeof(double);
//	sizefloat=sizeof(float);	
	for(i=0;i<order;i++)
	{
		data+=(b[i]*in[i]);
	}
	return (s32)data;	
}

/*u32 GetFilterData(const u32* in, short* b,u16 order)
{
	s32 data=0;
  u16 i;
	for(i=0;i<order;i++)
	{
		data+=(b[i]*in[i]);
	}
	//fir_16by16_stm32((void*)&data,in,&Fir_Coefs,1);
	return (u32)data;
}*/


//增加fir之前的数据，装满后来一个数处理一个
u8 AddOriginalData(s32 data,u8 type)
{
	s32 val=0;
	s32 plethdata=0;	
	u16 tmpidx=0;	
	
//添加原始数据
#if(SAVE_ORIGINAL_DATA==1)		
	if(TmpOriginalOxiSignalBufIdx[type]==ORIGINAL_ARRAY_COUNT)
	{
		tmpidx=ORIGINAL_ARRAY_COUNT-1;
		memmove(TmpOriginalOxiSignalBuf[type],&TmpOriginalOxiSignalBuf[type][1],sizeof(s32)*tmpidx);
		TmpOriginalOxiSignalBuf[type][tmpidx]=data;			
	}
	else
	{
		TmpOriginalOxiSignalBuf[type][TmpOriginalOxiSignalBufIdx[type]]=data;
		TmpOriginalOxiSignalBufIdx[type]++;
	}
#endif	
//
#if(IIR_FILTER_ENABLE==1)	
	val=IIRFilter(val,type);
#endif	

#if(FIR_FILTER_ENABLE==1)
	if(FirLowPassTmpInIdx[type]==FIR_ORDER)
	{
		memmove((void*)FirLowPassTmpBuffer[type],(void*)&FirLowPassTmpBuffer[type][1],sizeof(FirLowPassTmpBuffer[type])-sizeof(vu32));
		FirLowPassTmpBuffer[type][FIR_ORDER-1]=data;		
//		val=GetFilterData((s32*)FirLowPassTmpBuffer[type],B,FIR_ORDER);
		if(g_LostFirDataNum[type]>0)
		{
			g_LostFirDataNum[type]--;
			plethdata=0;
		}
		else
		{
			if(TmpFirOxiSignalBufIdx[type]<SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE)
			{
				TmpFirOxiSignalBuf[type][TmpFirOxiSignalBufIdx[type]]=val;
				TmpFirOxiSignalBufIdx[type]++;
			}
			else
			{
				tmpidx=(SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE)-1;
				memmove(TmpFirOxiSignalBuf[type],&TmpFirOxiSignalBuf[type][1],sizeof(s32)*tmpidx);
				TmpFirOxiSignalBuf[type][tmpidx]=val;		
			}
			plethdata=TmpFirOxiSignalBuf[type][(TmpFirOxiSignalBufIdx[type]+SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE-DELAY_INT)%600];//往前延了82个数据，为了与int数据接合
		}
//		heartbeat=CheckHeartBeat(type);
#if(INT_COEFF_ENABLE==1)
		val=IntCoeff(val,type);	
		if(g_LostIntDataNum[type]==0)
		{
			if(TmpIntOxiSignalBufIdx[type]==SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT)
			{
				TmpIntOxiSignalBufFullFirst[type]=1;
				tmpidx=(SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE+DELAY_COUNT)-1;
				memmove(TmpIntOxiSignalBuf[type],&TmpIntOxiSignalBuf[type][1],sizeof(s32)*tmpidx);
				TmpIntOxiSignalBuf[type][tmpidx]=val;
			}
			else
			{
				TmpIntOxiSignalBuf[type][TmpIntOxiSignalBufIdx[type]]=val;
				TmpIntOxiSignalBufIdx[type]++;
			}
		}
		else
		{
			g_LostIntDataNum[type]--;
		}
#endif			
	}
	else
	{
		FirLowPassTmpBuffer[type][FirLowPassTmpInIdx[type]++]=data;
		plethdata=0;
		g_LostIntDataNum[type]=300;
		g_SampleCount=0;
	}
#else
	val=data;
	plethdata=val;
#endif

#if(LEASTSQUAREMETHOD_ENABLE==1)
	{
#define NUM_COUNT	7		
		float x[NUM_COUNT];
		float y[NUM_COUNT];
		u16 i;
		u16 idx=TmpIntOxiSignalBufIdx[type]-NUM_COUNT;
		if(TmpIntOxiSignalBufIdx[type]>=162+NUM_COUNT)
		{
			for(i=0;i<NUM_COUNT;i++)
			{
				x[i]=i;
				y[i]=(float)TmpIntOxiSignalBuf[type][idx+i];
			}
			val=TestLeastSquareMethod(x,y,NUM_COUNT);
		}
	}
#endif	
	if(TmpIntOxiSignalBufFullFirst[type]==1&&Old_TmpIntOxiSignalBufFullFirst[type]==0)//202是FIR与INT滤波需要的数据
	{
		Old_TmpIntOxiSignalBufFullFirst[type]=1;
	}
	else if(TmpIntOxiSignalBufFullFirst[type]==1)
	{
//		AddSimulationWaveData(type,Curve_Fitting(val,type));
		
		if(g_DelayIntCoef[type]<DELAY_COUNT)//往后延82点
		{
			g_DelayIntCoef[type]++;
			AddSimulationWaveData(type,plethdata);
		}
		else if(g_DelayIntCoef[type]==DELAY_COUNT)
		{
			g_DelayIntCoef[type]++;
			CopyPlethData(type,TmpIntOxiSignalBuf[type]);				
		}
		else
		{
			AddSimulationWaveData(type,val);
		}
	}
	else
	{
		AddSimulationWaveData(type,plethdata);		
	}	
	
	if(type==R_IDX)
	{
#if(SAMPLE_100HZ==1)	
		if(g_SampleCount++==50)	//50个数计算一次，加快响应速度,20个数数据变化太快
//		if(g_SampleCount++==25)	//50个数计算一次，加快响应速度,20个数数据变化太快
#else
		if(g_SampleCount++==60)	//60个数计算一次，加快响应速度
#endif	
		{
			g_SampleCount=0;
			g_Simulation_Process_Flag=1;
		}	
	}
	
	return 1;
}

void ClearParameterData(u8 type)
{	
	g_PR_V_Count[type]=0;
	g_PR_V_Idx[type]=0;
  g_R_V_Idx[type]=0;
  g_R_V_Count[type]=0;

	g_Bak_R_V_Idx[type]=0;
	g_Bak_R_V_Count[type]=0;
	g_Bak_R_Value[type]=0.0;
	g_Bak_PR_V_Count[type]=0;
	g_Bak_PR_V_Idx[type]=0;
	g_Bak_PR_Value[type]=0xffff;
	g_PR=0xffff;
	g_SpO2=0x7f;		
	g_PRStepInterval=0;
}

void ClearProcessOxiData(u8 type)
{
	u8 i;
	ClearParameterData(type);
	g_FirstTestValue=1;
	Bak_Bot1_Num_IR[1]=0;
	ClearSimulationWaveData();
	for(i=0;i<2;i++)
	{
		g_DelayIntCoef[i]=0;
		g_LostFirDataNum[i]=50;
		g_LostIntDataNum[i]=300;
		FirLowPassTmpInIdx[i]=0;
		TmpFirOxiSignalBufIdx[i]=0;
		TmpIntOxiSignalBufIdx[i]=0;
		Old_TmpIntOxiSignalBufFullFirst[i]=0;
		TmpIntOxiSignalBufFullFirst[i]=0;
	}
	
	memset((void*)TmpFirOxiSignalBuf,0,sizeof(TmpFirOxiSignalBuf));
	memset((void*)FirLowPassTmpBuffer,0,sizeof(FirLowPassTmpBuffer));
	ResetIntCoeff();
	
	g_SampleCount=0;

	g_Simulation_Process_Flag=0;	
}

static s32* pTopValue=NULL;
static u16* pTopStep=NULL;
static s32* pBottomValue=NULL;
static u16* pBottomStep=NULL;
static u8*	pTopNumber=NULL;
static u8*	pBottomNumber=NULL;
static u8 top_number=0,bottom_number=0;
static u16 count_top=0,count_bottom=0;

/*********************************************************************************
*Function:  FindP2P
*Description:  find peak-to-peak value and point
*Author:	Mike
*Input:  
				vfbuf:data buffer 
				len_datapoint:p2p length 
				LedType:0-ired 1-red 
				len:data length 
				type:0-digital 1-sim
*Output: 
*Return: result:0-fail 1-success
*Others:  
**********************************************************************************/ 
void FindP2P_Method1(s32 *vf_buf, u16 len_datapoint, u8 LedType,u16 len,u8 type)
//LedType 0:ired 1:red
{
	s16 i;
  u16 k,m;
  u8 j;
//	u8 findtop=1;
	u8 v=len_datapoint+1;
	u8 top_number=0,bottom_number=0;
	
	/*if(LedType>1)
	{
		LedType=LedType;
	}*/
	
	pTopValue=P2P_Value[2*LedType];
	pTopStep=P2P_Step[2*LedType];
	pBottomValue=P2P_Value[2*LedType+1];
	pBottomStep=P2P_Step[2*LedType+1];
	pTopNumber=&P2P_Num[2*LedType];
	pBottomNumber=&P2P_Num[2*LedType+1];

	for(i=len_datapoint;i<len-len_datapoint;i++)
  {
   	count_top=0;
		count_bottom=0;
		for(j=1;j<v;j++)
    {            
			m=i+j;	//right data
      k=i-j;	//left data
      if((vf_buf[i]>=vf_buf[k])&&(vf_buf[i]>vf_buf[m]))
			//if((vf_buf[i]>vf_buf[k])&&(vf_buf[i]>vf_buf[m]))
      {
				count_top++;
      }
			else if((vf_buf[i]<vf_buf[k])&&(vf_buf[i]<=vf_buf[m]))
			//else if((vf_buf[i]<vf_buf[k])&&(vf_buf[i]<vf_buf[m]))
			{	
				count_bottom++;
			}
      else
      {
        break;  //exit this cycle                
      }
    }
		if(count_top>=len_datapoint)
    {		
			if(top_number<INFLECT_POINT_MAX)
	    {
//				if(findtop==0)
				{
//					findtop=1;					
					pTopValue[top_number]=vf_buf[i];	//save every top point's value & step
					pTopStep[top_number]=i;
					top_number++;	
				}
			}
    }
		else if(count_bottom>=len_datapoint)
		{
			if(bottom_number<INFLECT_POINT_MAX)
	    {
//				if(findtop==1)
				{
//					findtop=0;
					pBottomValue[bottom_number]=vf_buf[i];
					pBottomStep[bottom_number]=i;
					bottom_number++;			
				}
			}
		}
  }
	*pTopNumber=top_number;
	*pBottomNumber=bottom_number;
}


void FindP2P(s32 *vf_buf, u16 len_datapoint, u8 LedType,u16 len,u8 type)
//LedType 0:ired 1:red
{
	u16 i,j,k,m;
	u16 v=len_datapoint+1;
	s32 max;
	
	pTopValue=P2P_Value[2*LedType];
	pTopStep=P2P_Step[2*LedType];
	pBottomValue=P2P_Value[2*LedType+1];
	pBottomStep=P2P_Step[2*LedType+1];
	pTopNumber=&P2P_Num[2*LedType];
	pBottomNumber=&P2P_Num[2*LedType+1];
	top_number=0;
	bottom_number=0;

	i=len_datapoint;
	while(i<len-len_datapoint)
	{
		count_bottom=0;
		for(j=1;j<v;j++)
    {            
			m=i+j;	//right data
      k=i-j;	//left data
			if((vf_buf[i]<=vf_buf[k])&&(vf_buf[i]<vf_buf[m]))
			//else if((vf_buf[i]<vf_buf[k])&&(vf_buf[i]<vf_buf[m]))
			{	
				count_bottom++;
			}
      else
      {
        break;  //exit this cycle                
      }
    }		
		if(count_bottom>=len_datapoint)
		{
			if(bottom_number<INFLECT_POINT_MAX)
	    {
				pBottomValue[bottom_number]=vf_buf[i];
				pBottomStep[bottom_number]=i;
				bottom_number++;			
			}
			i+=len_datapoint;
			continue;
		}		
		i++;
	}
	for(i=0;i<bottom_number-1;i++)
	{
		k=pBottomStep[i];
		max=pBottomValue[i];
		for(j=pBottomStep[i]+1;j<pBottomStep[i+1];j++)
		{
			if(max<vf_buf[j])
			{
				max=vf_buf[j];
				k=j;
			}
		}
		pTopValue[top_number]=max;	//save every top point's value & step
		pTopStep[top_number]=k;
		top_number++;		
	}
	*pTopNumber=top_number;
	*pBottomNumber=bottom_number;
}

/*
//过滤错误的峰值，主要是根据峰值之间的间距判断是否正确
u8 FilterErrorP2P(u16* pStep,u8* pNum)
{
	static u16 interval[50];
//	static u8 equalcount[50];	//找出的最佳值
	u8 InIdx;	//存储间隔值的下标
	u8 i,j,step=1;
	u8 equalcount=0;
	u16 tmpinterval,bakinterval;
	u16 bIdx=0;		//开始位置	
	u8 num=*pNum;
	u8 bFind=0;			//是否找到合适的位置
	
	while(bIdx<num-1)
	{
		step=1;
		while(step<num-1&&bIdx+step<num)
		{
			InIdx=0;
			equalcount=0;
			interval[InIdx++]=pStep[bIdx];
			interval[InIdx++]=pStep[bIdx+step];
			bakinterval=pStep[bIdx+step]-pStep[bIdx];
			i=bIdx+step;
			while(i<num-1)
			{
				j=i+1;
				bFind=0;
				while(j<=num-1)				
				{
					tmpinterval=pStep[j]-pStep[i];
					if((tmpinterval<=bakinterval+g_P2P_InterVal)&&(tmpinterval>=bakinterval-g_P2P_InterVal))
					{
						interval[InIdx++]=pStep[j];
						equalcount++;
						bFind=1;
						i=j;						
						break;
					}
					else if(tmpinterval>bakinterval+g_P2P_InterVal)
					{
						break;
					}
					j++;					
				}
				if(tmpinterval>bakinterval+g_P2P_InterVal)
				{
					break;
				}
				if(bFind==0)
				{
					i++;
				}
			}		
			if(equalcount>=1)
			{
				break;
			}
			step++;
		}
		if(equalcount>=1)
		{
			break;
		}
		bIdx++;
	}
	if(equalcount>=1)
	{
		for(i=0;i<equalcount+2;i++)
		{
			pStep[i]=interval[i];
		}
		*pNum=equalcount+2;
	}
	return (equalcount>=1);
//	return 1;
}*/

//
static s32* pIRed;
static s32* pRed;
static u16 point_interval;	//COUNT_POINT_MIN modify value//这个值用于表示多少个数据个有一个峰值	

/*********************************************************************************
*Function:  GetP2PValue
*Description:  Get peak-to-peak value
*Author:	Mike
*Input:  len:data length type:0-digital oxi 1-sim
*Output: 
*Return: result:0-fail 1-success
*Others:  
**********************************************************************************/ 
u8 GetP2PValue(u16 len,u8 type)
{	
	if(g_CalcDataType==1)
	{
		FindP2P(pIRed,COUNT_POINT_MIN,0,len,type);
		FindP2P(pRed,COUNT_POINT_MIN,1,len,type);
		if(PRSpO2_Process(type))
		{
			return 1;
		}		
	}
	else
	{
		point_interval=COUNT_POINT_MIN;	//COUNT_POINT_MIN modify value//这个值用于表示多少个数据个有一个峰值	
		while(point_interval<=COUNT_POINT_MAX)
		{	
			FindP2P_Method1(pIRed,point_interval,0,len,type);
			FindP2P_Method1(pRed,point_interval,1,len,type);
			if(PRSpO2_Process(type))
			{
				return 1;
			}
			point_interval++;					
		}	
	}

	return 0;
}

u8 OXI_Handle(u16 len,u8 type)
{
	pIRed=FirOxiSignalBuf[0];
	pRed=FirOxiSignalBuf[1];
	return GetP2PValue(len,type);	
}

//使用面积法求R值
#if(SPO2_METHOD_AREA==1)
u8 CalcRValueArea(u16 bot_ir_step1,u16 bot_ir_step2,u16 bot_r_step1,u16 bot_r_step2,float* r)
{
/*	u16 i;
	s32 irbaseline=((FirOxiSignalBuf[0][bot_ir_step1]+FirOxiSignalBuf[0][bot_ir_step1+1])>>1);
	s32 rbaseline=((FirOxiSignalBuf[1][bot_r_step1]+FirOxiSignalBuf[1][bot_r_step1+1])>>1);
	s32 irarea=0,rarea=0;
	for(i=bot_ir_step1;i<=bot_ir_step2;i++)
	{
		irarea+=(FirOxiSignalBuf[0][i]-irbaseline);
	}
	for(i=bot_r_step1;i<=bot_r_step2;i++)
	{
		rarea+=(FirOxiSignalBuf[1][i]-rbaseline);
	}	
	if(irarea==0)
	{
		return 0;		
	}
	*r=(((float)rarea)/irarea);	
	return 1;*/
	
	float irarea=0.00,rarea=0.00;
	u16 i;
	float k[2];
	float b[2];
	k[0]=((float)(FirOxiSignalBuf[0][bot_ir_step2]-FirOxiSignalBuf[0][bot_ir_step1]))/(bot_ir_step2-bot_ir_step1);
	k[1]=((float)(FirOxiSignalBuf[1][bot_r_step2]-FirOxiSignalBuf[1][bot_r_step1]))/(bot_r_step2-bot_r_step1); 
	
	b[0]=FirOxiSignalBuf[0][bot_ir_step2]-k[0]*bot_ir_step2;
	b[1]=FirOxiSignalBuf[1][bot_r_step2]-k[1]*bot_r_step2;
	
	for(i=bot_ir_step1;i<=bot_ir_step2;i++)
	{
		irarea+=(FirOxiSignalBuf[0][i]-(k[0]*i+b[0]));
	}
	for(i=bot_r_step1;i<=bot_r_step2;i++)
	{
		rarea+=(FirOxiSignalBuf[1][i]-(k[1]*i+b[1]));
	}		
	
	if(irarea==0.000)
	{
		return 0;		
	}
	*r=(((float)rarea)/irarea);	
	return 1;	
}
#endif


