#include "ledcurrent.h"
#include "piprocess.h"
#include "global.h"
#include "afe4490.h"
#include "spo2tools.h"
#include <stdlib.h>

//sample time=400 us r*c<sample time/10	
static u8 rf[7]={0x05,0x04,0x03,0x02,0x01,0x00,0x06};	//10k,25k,50k,100k,250k,500k,1m
static u8 rc[7]={0xf8,0xf8,0xf8,0xf8,0x80,0x50,0x28};

//<=50mA，Tx reference voltage 0.5V
//<=75mA，Tx reference voltage 0.75V
//<=100mA，Tx reference voltage 1.0V
//now all is Tx reference voltage 1.0V 100mA
static u8 g_CurrentLevelMap[10]={25,50,75,100,125,150,175,200,225,250};//0:10mA 1:20mA 2:30mA 3:40mA 4:50mA 5:60mA 6:70mA 7:80mA 8:90mA 9:100mA

u8 g_LedGain[2]={DEAULT_LED1_GAIN_LEVEL,DEAULT_LED2_GAIN_LEVEL};		//放大倍数，与RF成正比
//根据灌注计算电流
u8 g_LedCurrentLevel[2]={DEAULT_LED1_CURRENT_LEVEL,DEAULT_LED2_CURRENT_LEVEL};	//0:10mA 1:20mA 2:30mA 3:40mA 4:50mA 5:60mA 6:70mA 7:80mA 8:90mA 9:100mA

//type 0:IR 1:R
void SetLedCurrent(u8 type,u8 indexcurrent)
{
//0:10mA 1:20mA 2:30mA 3:40mA 4:50mA 5:60mA 6:70mA 7:80mA 8:90mA 9:100mA	
	u32 OldRegister;
#if(CURRENT_ADJUST_STYLE==0)	
	u8 current=g_CurrentLevelMap[g_LedCurrentLevel[type]];	//电流
#else
	u8 current=g_LedCurrentLevel[type];	//电流
#endif
	current=indexcurrent;
	if(type==0)
	{
		OldRegister=ReadCommand(0x22);
		
		WriteCommand(0x22,(0x00ff&OldRegister)|(current<<8)|0x20000);	//default
		//WriteCommand(0x22,(0x000ff&OldRegister)|(g_LedCurrentLevel[0]<<8));
		OldRegister=ReadCommand(0x22);
	}
	else
	{
		OldRegister=ReadCommand(0x22);
		WriteCommand(0x22,(0xff00&OldRegister)|(current)|0x20000);	//default
		//WriteCommand(0x22,(0x0ff00&OldRegister)|(g_LedCurrentLevel[1]));
	}
}

//当手指/探头脱落到未脱落时，首先设置到最大电流，最大增益，然后重新调整到最大的电流与增益，尽量确保使用大电流测量
void SetMaxCurrentGain(void)
{
	u8 i;
	for(i=0;i<2;i++)
	{
		g_LedCurrentLevel[i]=MAX_CURRENT_VALUE;
//		SetLedCurrent(i);
		SetGain(i,MAX_GAIN_VALUE);
	}
}

u8 AddLedCurrent(u8 LedType)
{
#if(CURRENT_ADJUST_STYLE==0)
	if(g_LedCurrentLevel[LedType]<MAX_CURRENT_VALUE)
	{
		g_LedCurrentLevel[LedType]++;
//		SetLedCurrent(LedType);
		//AFE4490_Read_All_Register();
		return 1;
	}
#else
	s16 v;	
	if(g_LedCurrentLevel[LedType]<MAX_CURRENT_VALUE)
	{
		v=g_LedCurrentLevel[LedType]+20;
		if(v>MAX_CURRENT_VALUE)
		{
			g_LedCurrentLevel[LedType]=MAX_CURRENT_VALUE;
		}
		else
		{
			g_LedCurrentLevel[LedType]=v;
		}		
//		SetLedCurrent(LedType);
		//AFE4490_Read_All_Register();
		return 1;
	}	
#endif	
	return 0;
}

u8 ReduceLedCurrent(u8 LedType)
{
#if(CURRENT_ADJUST_STYLE==0)
	if(g_LedCurrentLevel[LedType]>MIN_CURRENT_VALUE)
	{
		g_LedCurrentLevel[LedType]--;
//		SetLedCurrent(LedType);
		//AFE4490_Read_All_Register();
		return 1;
	}
#else
	s16 v;		
	if(g_LedCurrentLevel[LedType]>MIN_CURRENT_VALUE)
	{
		v=g_LedCurrentLevel[LedType]-20;
		if(v<MIN_CURRENT_VALUE)
		{
			g_LedCurrentLevel[LedType]=MIN_CURRENT_VALUE;
		}
		else
		{
			g_LedCurrentLevel[LedType]=v;
		}		
//		SetLedCurrent(LedType);
		//AFE4490_Read_All_Register();
		return 1;
	}	
#endif	
	return 0;
}

void SetGain(u8 LedType,u8 gain)
{
	g_LedGain[LedType]=gain;
	WriteCommand(0x20+LedType,rf[g_LedGain[LedType]]|rc[g_LedGain[LedType]]);		
}

//当电流调至最小时，还出现AD满载的情况，则调整放大倍数
//调整放大倍数 RF CF
//RF与放大倍数成正比
//flag:0:add gain 1:reduce gain 2:循环调节
u8 AdjustGain(u8 LedType,u8 flag)			
{
	u8 adjustflag=0;		//是否已调整电流标志
	switch(flag)
	{
	case 0:
		if(g_LedGain[LedType]<MAX_GAIN_VALUE)
		{
			g_LedGain[LedType]++;
			g_LedCurrentLevel[LedType]=MIN_CURRENT_VALUE;	
			adjustflag=1;
		}		
		break;
	case 1:
		if(g_LedGain[LedType]>MIN_GAIN_VALUE)
		{
			g_LedGain[LedType]--;
			g_LedCurrentLevel[LedType]=MAX_CURRENT_VALUE;			
			adjustflag=1;
		}		
		break;
	case 2:
		adjustflag=1;
		if(g_LedGain[LedType]<MAX_GAIN_VALUE)
		{
			g_LedGain[LedType]++;
		}
		else
		{
			g_LedGain[LedType]=MIN_GAIN_VALUE;
		}	
		g_LedCurrentLevel[LedType]=MIN_CURRENT_VALUE;		
		break;
	default:
		break;
	}
	if(adjustflag)
	{
//		SetLedCurrent(LedType);
		WriteCommand(0x20+LedType,rf[g_LedGain[LedType]]|rc[g_LedGain[LedType]]);
	}
	return adjustflag;	
}

/*********************************************************************************
*Function:  AdjustCurrent
*Description:  Adjust Current
*Author:	Mike
*Input:  
				type:Led Type 0:IR 1:R    flag:0:add current 1:reduce current
*Output: 
*Return: result:0-fail 1-success
*Others:  
**********************************************************************************/ 
u8 AdjustCurrent(u8 type,u8 flag)
{
//原则上采用的是大电流，小增益，使之具有较好的信号质量	
//如果是需要增大AD信号值，首先考虑增大电流，其次再考虑增大增益	
//反之，需要降低AD信号值，则先考虑降低增益，其次再考虑降低增益	
	switch(flag)
	{
	case 0:
		if(!AddLedCurrent(type))
		{
			return AdjustGain(type,flag);
		}
		break;
	case 1:
		if(!AdjustGain(type,flag))
		{
			return ReduceLedCurrent(type);
		}		
		break;
	case 2:
		if(!AddLedCurrent(type))
		{
			return AdjustGain(type,2);
		}		
		break;
	default:
		break;
	}
	return 1;
}

//1、调节ADC值，应该在100w左右
//0:ir 1:r
void LedCurrentControl(s32* adcvalue)
{
	static u8 waittime=0;
/*	u8 i;
	for(i=0;i<2;i++)
	{	
		if(adcvalue[i]<MIN_ADC_VALUE)
		{
			AdjustCurrent(i,0);
		}
		else if(adcvalue[i]>MAX_ADC_VALUE)
		{
			AdjustCurrent(i,1);
		}
	}	*/
	
	u8 i;
	s16 v;

	if(++waittime<2)
	{
		return;
	}
	waittime=0;
	for(i=0;i<2;i++)
	{	
		if(g_SenserOff&&i)
		{		
			AdjustCurrent(i,2);
			return;
		}
		if(adcvalue[i]<MIN_ADC_VALUE)
		{			
			if(g_LedCurrentLevel[i]<MAX_CURRENT_VALUE)
			{
				v=g_LedCurrentLevel[i]+20;				
				if(v>MAX_CURRENT_VALUE)
				{				
					g_LedCurrentLevel[i]=MAX_CURRENT_VALUE;
				}
				else
				{
					g_LedCurrentLevel[i]=v;
				}
//				SetLedCurrent(i);			
			}
			else
			{
				if(g_LedGain[i]<6)
				{
					g_LedGain[i]=g_LedGain[i]+1;
					if(g_LedGain[i]>6)
					{
						g_LedGain[i]=6;
					}
					SetGain(i,g_LedGain[i]);				
				}
			}
			waittime=0;
		}
		else if(adcvalue[i]>MAX_ADC_VALUE)
		{
			if(g_LedGain[i])
			{
				g_LedGain[i]=g_LedGain[i]-1;
				SetGain(i,g_LedGain[i]);
			}
			else
			{
				if(g_LedCurrentLevel[i]!=MIN_CURRENT_VALUE)
				{
					v=g_LedCurrentLevel[i]-20;
					if(v<MIN_CURRENT_VALUE)
					{				
						g_LedCurrentLevel[i]=MIN_CURRENT_VALUE;				
					}			
					else
					{
						g_LedCurrentLevel[i]=v;
					}
//					SetLedCurrent(i);			
				}
			}
			waittime=0;
		}
	}		
}

/*
static vs32 SampleData[2][JUDGE_FINGER_OFF_NUM];
static vu8	SampleDataIdx[2]={0,0};
static vu8  SampleDataCount[2]={0,0};

//检测是否手指脱落，原理：如果JUDGE_FINGER_OFF_NUM个值变化小于COMPARE_VALUE，则还是手指脱落，通常如果正常接上手指后，ad采样值变化会较大
//实验证明这种方法效果不好
u8 CheckFingerOff2(void)
{
	s32 min[2],max[2];
	if((SampleDataCount[0]<JUDGE_FINGER_OFF_NUM)||(SampleDataCount[1]<JUDGE_FINGER_OFF_NUM))
	{
		return 1;
	}
	
	min[0]=FindMin((s32*)SampleData[0],JUDGE_FINGER_OFF_NUM);
	min[1]=FindMin((s32*)SampleData[1],JUDGE_FINGER_OFF_NUM);
	max[0]=FindMax((s32*)SampleData[0],JUDGE_FINGER_OFF_NUM);
	max[1]=FindMax((s32*)SampleData[1],JUDGE_FINGER_OFF_NUM);
	if((max[0]-min[0]<COMPARE_VALUE)||(max[1]-min[1]<COMPARE_VALUE))
	{
		return 1;
	}
	return 0;
}

void AddLedCurrentSampleData(s32* adcvalue)
{
//adcvalue 0:red 1:ired	 sampledata 0:ired 1:red
	SampleData[0][SampleDataIdx[0]]=adcvalue[1];
	SampleData[1][SampleDataIdx[1]]=adcvalue[0];
	SampleDataIdx[0]=(SampleDataIdx[0]+1)%JUDGE_FINGER_OFF_NUM;
	SampleDataIdx[1]=(SampleDataIdx[1]+1)%JUDGE_FINGER_OFF_NUM;
	if(SampleDataCount[0]<JUDGE_FINGER_OFF_NUM)
	{
		SampleDataCount[0]++;
	}
	if(SampleDataCount[1]<JUDGE_FINGER_OFF_NUM)
	{
		SampleDataCount[1]++;
	}	
}*/


