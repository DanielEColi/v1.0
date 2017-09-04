#include "parameterprocess.h"
#include "algorithm.h"
#include "global.h"
#include "spo2tools.h"
#include "afe4490.h"
#include "rtable.h"
#include <string.h>
#include <stdlib.h>
#include "afe4490oxiprocess.h"
#include "threshold.h"
#include "processpacket.h"
#include "plethwave.h"
#include "piprocess.h"

float g_PI_Bak[SIZE_SPO2];			//多个pi指数值备份
u8 g_PI_Num=0;
u8 g_PI_Idx=0;

u16 Step_Idx_Start[4]={0};	//0:ired top start 1:ired bot start 2:red top start 3:red bot start
u16 Step_Idx_End[4]={0};		//0:ired top end 1:ired bot end 2:red top end 3:red bot end

float aver_step[2]={0.0,0.0};
float Independent_Aver_Step[4]={0.0};	//独立的步长值 0:ired top 1:ired bot 2:red top 3:red bot

u8 P2P_Num[4]={0,0};		//0:ired top 1:ired bot 2:red top 3:red bot
s32 P2P_Value[4][INFLECT_POINT_MAX];//0:ired top 1:ired bot 2:red top 3:red bot
u16 P2P_Step[4][INFLECT_POINT_MAX];//0:ired top 1:ired bot 2:red top 3:red bot

u8 SpO2_P2P_Num[4]={0,0};
s32 SpO2_P2P_Value[4][INFLECT_POINT_MAX];
u16 SpO2_P2P_Step[4][INFLECT_POINT_MAX];

u8 Bak_P2P_Num[4];
s32 Bak_P2P_Value[4][INFLECT_POINT_MAX];
u16 Bak_P2P_Step[4][INFLECT_POINT_MAX];

u8 g_P2P_InterVal=3;	//动态调整认为峰值是正确的间隔值，当心率较快时，该值较小，当心率较慢时，该值较大

u8 g_FirstTestValue=1;	//是不是第一次测出数据

//type 0:digital 1:sim
//判断检测出的P2P值是否正确
unsigned char CheckP2P(u8 type)
{
	short i,j;
	unsigned char bFind[4];
	unsigned char minnum=0xff;
	unsigned short interval[4]={0,0,0,0};
	unsigned short threshold;
	for(i=0;i<4;i++)
	{
		Step_Idx_Start[i]=0xff;
		Step_Idx_End[i]=0x00;
	}
	
	//return 0;
//找出最小的峰值个数	
	for(i=0;i<4;i++)
	{
		if(P2P_Num[i]<minnum)
		{
			minnum=P2P_Num[i];
		}
	}
	if(minnum<2)
	{
		return 0;
	}
	if(minnum>2)
	{
		for(i=0;i<4;i++)
		{
			bFind[i]=0;
			for(j=P2P_Num[i]-2;j>=1;j--)
			{
				if((GetValueInterval(P2P_Step[i][j]-P2P_Step[i][j-1],P2P_Step[i][j+1]-P2P_Step[i][j])>g_P2P_InterVal))	//记录是之前的间隔距离，如果突然变化较大，有可能会不正确，导致找不到正确的峰峰值
				{
					if(bFind[i])
					{
						break;
					}
				}
				else
				{
					if(Step_Idx_Start[i]==0xff)
					{
						Step_Idx_End[i]=j+1;	
						bFind[i]=1;
					}
					Step_Idx_Start[i]=j-1;				
				}
			}
		}	
	}
	else
	{
		for(i=0;i<4;i++)
		{
			bFind[i]=0;
			for(j=0;j<P2P_Num[i]-1;j++)
			{
				interval[i]+=P2P_Step[i][j+1]-P2P_Step[i][j];
			}
			interval[i]=interval[i]/(P2P_Num[i]-1);
		}
		threshold=interval[1]/20+1;
		for(i=0;i<4;i++)
		{				
			if(GetValueInterval(interval[i],interval[1])<threshold)
			{
				bFind[i]=1;
			}
			Step_Idx_Start[i]=0;
			Step_Idx_End[i]=P2P_Num[i]-1;
		}
	}
	for(i=0;i<4;i++)
	{
		if(bFind[i]==0)		//如果有某一路找不到，则返回0
		{
			return 0;
		}
	}	
	return FindOptimalP2P(type);	
}

unsigned char FindOptimalP2P(u8 type)
{
	short i,j,m,n,count;
	unsigned char tmp_p2p_interval;
	unsigned short bak_interval=0;		//以红外的峰谷值为标准
	unsigned short interval=0;
	unsigned char find[4]={0,0,0,0};
	unsigned short BIdx,EIdx;
	unsigned char idx=0;

	for(i=Step_Idx_Start[1]+1;i<=Step_Idx_End[1];i++)
	{
		bak_interval+=(P2P_Step[1][i]-P2P_Step[1][i-1]);
	}
	bak_interval/=(Step_Idx_End[1]-Step_Idx_Start[1]);
	tmp_p2p_interval=(bak_interval/20)+1;

	SpO2_P2P_Num[1]=Step_Idx_End[1]-Step_Idx_Start[1]+1;
	for(n=Step_Idx_Start[1];n<=Step_Idx_End[1];n++)
	{
		SpO2_P2P_Value[1][idx]=P2P_Value[1][n];
		SpO2_P2P_Step[1][idx]=P2P_Step[1][n];
		idx+=1;
	}

	for(i=0;i<4;i++)
	{
		if(i!=1)
		{
			find[i]=0;
			interval=0;
			{
				for(j=Step_Idx_Start[i]+1;j<=Step_Idx_End[i];j++)
				{
					interval+=(P2P_Step[i][j]-P2P_Step[i][j-1]);					
				}
				interval/=(Step_Idx_End[i]-Step_Idx_Start[i]);
				if(GetValueInterval(bak_interval,interval)>tmp_p2p_interval)		//判断间距是否相等
				{
					continue;
				}
				else
				{
					if((i%2)==0)
					{
						//仅与红外波谷比较范围，波峰范围应该在波谷范围之内，如果有1个值在此范围之内，即认为是正确的
						count=0;
						BIdx=0xff;
						EIdx=0x00;
						for(m=Step_Idx_Start[i];m<=Step_Idx_End[i];m++)
						{
							if((P2P_Step[i][m]>P2P_Step[1][Step_Idx_Start[1]])&&(P2P_Step[i][m]<P2P_Step[1][Step_Idx_End[1]]))
							{
								if(BIdx==0xff)
								{
									BIdx=m;
								}
								EIdx=m;
								count++;
							}
						}
					}
					else
					{
						//仅与红外波谷比较范围，波峰范围应该在波谷范围之内，如果有1个值在此范围之内，即认为是正确的
						count=0;
						BIdx=0xff;
						EIdx=0x00;
						for(m=Step_Idx_Start[i];m<=Step_Idx_End[i];m++)
						{
							if((P2P_Step[i][m]>P2P_Step[1][Step_Idx_Start[1]]-tmp_p2p_interval)&&(P2P_Step[i][m]<P2P_Step[1][Step_Idx_End[1]]+tmp_p2p_interval))
							{
								if(BIdx==0xff)
								{
									BIdx=m;
								}
								EIdx=m;
								count++;
							}
						}
					}
					if(count>=1)
					{
						idx=0;
						SpO2_P2P_Num[i]=EIdx-BIdx+1;
						for(n=BIdx;n<=EIdx;n++)
						{
							SpO2_P2P_Value[i][idx]=P2P_Value[i][n];
							SpO2_P2P_Step[i][idx]=P2P_Step[i][n];
							idx+=1;
						}
						find[i]=1;
					}
				}
			}
		}
	}
		
	if(find[0]&&find[2]&&find[3])
	{
		if(PR_Process(type))
		{
			return SpO2_Process(type);
		}			
	}
	return 0;
}

//从整系数据中查找出来的峰峰值，再在FIR数据中查找，重新定位
u8 AgainSearchP2PFromInt(void)
{
	u16 i,j,k;
	u16 bIdx,eIdx;
	s32* pData;
	s32 max,min;
	u8 idx;
	for(i=0;i<4;i++)
	{
		if(i<2)
		{
			pData=FirOxiSignalBuf[0];
		}
		else
		{
			pData=FirOxiSignalBuf[1];
		}
		idx=0;
		for(j=0;j<SpO2_P2P_Num[i];j++)
		{
			bIdx=(SpO2_P2P_Step[i][j]>10)?SpO2_P2P_Step[i][j]-10:0;
			eIdx=(SpO2_P2P_Step[i][j]<SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE-10)?SpO2_P2P_Step[i][j]+10:SAMPLE_TIMER_SECOND*AFE4490_SAMPLE_RATE;
			if(i%2==0)
			{
				max=pData[bIdx];
				for(k=bIdx+1;k<eIdx;k++)
				{
					if(pData[k]>max)
					{
						max=pData[k];
						SpO2_P2P_Step[i][idx]=k;
						SpO2_P2P_Value[i][idx]=max;					
					}
				}			
			}
			else
			{
				min=pData[bIdx];
				for(k=bIdx+1;k<eIdx;k++)
				{
					if(pData[k]<min)
					{
						min=pData[k];
						SpO2_P2P_Step[i][idx]=k;
						SpO2_P2P_Value[i][idx]=min;					
					}
				}			
			}
			idx++;
		}
		SpO2_P2P_Num[i]=idx;
	}
	return 1;
}

void PI_Process(float* pibuffer,u8 len)
{
	u8 PI;//灌注指数 perfusion index (PI)	
	float pi=CalcRValue(pibuffer,len);
	g_PI_Bak[g_PI_Idx]=pi;
	if(g_PI_Num<SIZE_SPO2)
	{
		g_PI_Num++;
	}
	g_PI_Idx++;
	if(g_PI_Idx==SIZE_SPO2)
	{
			g_PI_Idx=0;
	}
	PI=(u8)CalcRValue(g_PI_Bak,g_PI_Num);	
	if(PI>50)
	{
		g_CurrentPILevel=15;
		g_MaxPleth=100;
	}
	else if(PI>40)
	{
		g_CurrentPILevel=13;
		g_MaxPleth=90;
	}
	else if(PI>30)
	{
		g_CurrentPILevel=11;
		g_MaxPleth=80;
	}	
	else if(PI>20)
	{
		g_CurrentPILevel=9;
		g_MaxPleth=70;
	}	
	else if(PI>15)
	{
		g_CurrentPILevel=8;
		g_MaxPleth=60;
	}	
	else if(PI>10)
	{
		g_CurrentPILevel=7;
		g_MaxPleth=50;
	}	
	else if(PI>5)
	{
		g_CurrentPILevel=6;
		g_MaxPleth=40;
	}	
	else if(PI>3)
	{
		g_CurrentPILevel=5;
		g_MaxPleth=30;
	}
	else if(PI>2)
	{
		g_CurrentPILevel=4;
		g_MaxPleth=25;
	}	
	else if(PI>1)
	{
		g_CurrentPILevel=3;
		g_MaxPleth=20;
	}		
	else
	{
		g_CurrentPILevel=2;
		g_MaxPleth=15;
	}	
}

u8 SpO2_Process(u8 type)
{	
	float r_now[INFLECT_POINT_MAX];
	float tmp_buffer_rline[SIZE_SPO2];
	float tmp_PI[INFLECT_POINT_MAX];				
	unsigned short i,j;
	unsigned char num=0;
	//unsigned char idx=0;	
	unsigned short IR_T_Idx=0;
	unsigned short IR_B_Idx=0;
	unsigned short R_T_Idx=0;
	unsigned short R_B_Idx=0;
	unsigned char find[4]={0,0,0,0};

	for(i=0;i<SpO2_P2P_Num[1];i++)
	{
		IR_B_Idx=i;
		find[0]=1;
		find[1]=0;
		find[2]=0;
		find[3]=0;
		for(j=0;j<SpO2_P2P_Num[0];j++)
		{
			if((SpO2_P2P_Step[0][j]<SpO2_P2P_Step[1][i])&&((SpO2_P2P_Step[1][i]-SpO2_P2P_Step[0][j])<aver_step[type]))
			{
				find[1]=1;
				IR_T_Idx=j;
				break;	
			}
		}
		if(find[1]==0)
		{
			continue;
		}
		for(j=0;j<SpO2_P2P_Num[2];j++)
		{
			if(GetValueInterval(SpO2_P2P_Step[0][IR_T_Idx],SpO2_P2P_Step[2][j])<5)			
			{
				find[2]=1;	
				R_T_Idx=j;
				break;	
			}
		}
		if(find[2]==0)
		{
			continue;
		}
		for(j=0;j<SpO2_P2P_Num[3];j++)
		{								
			if((SpO2_P2P_Step[2][R_T_Idx]<SpO2_P2P_Step[3][j])&&((SpO2_P2P_Step[3][j]-SpO2_P2P_Step[2][R_T_Idx])<aver_step[type]))
			{
				find[3]=1;	
				R_B_Idx=j;
				break;	
			}
		}
		if(find[3]==0)
		{
			continue;
		}
		if((GetValueInterval(SpO2_P2P_Step[0][IR_T_Idx],SpO2_P2P_Step[2][R_T_Idx])>5)||(GetValueInterval(SpO2_P2P_Step[1][IR_B_Idx],SpO2_P2P_Step[3][R_B_Idx])>5))		
		{
			continue;
		}
#if(SPO2_METHOD_AREA==1)
		if((IR_B_Idx+1<=SpO2_P2P_Num[1])&&(R_B_Idx+1<=SpO2_P2P_Num[3]))
		{
			if(CalcRValueArea(SpO2_P2P_Step[1][IR_B_Idx],SpO2_P2P_Step[1][IR_B_Idx+1],SpO2_P2P_Step[3][R_B_Idx],SpO2_P2P_Step[3][R_B_Idx+1],&r_now[num]))
			{
				num++;
			}
		}
#else		
//		if(SpO2_P2P_Value[1][IR_B_Idx]>0&&SpO2_P2P_Value[3][R_B_Idx]>0)
		{			
			r_now[num]=(float)(SpO2_P2P_Value[2][R_T_Idx]-SpO2_P2P_Value[3][R_B_Idx])/(SpO2_P2P_Value[0][IR_T_Idx]-SpO2_P2P_Value[1][IR_B_Idx])*((float)SpO2_P2P_Value[1][IR_B_Idx]/SpO2_P2P_Value[3][R_B_Idx]);
			//tmp_PI[num]=((float)(SpO2_P2P_Value[2][R_T_Idx]-SpO2_P2P_Value[3][R_B_Idx])*1000/SpO2_P2P_Value[3][R_B_Idx]+(float)(SpO2_P2P_Value[0][IR_T_Idx]-SpO2_P2P_Value[1][IR_B_Idx])*1000/SpO2_P2P_Value[1][IR_B_Idx])/2;
			//因为目前红外的pi值计算误差较大，目前仅使用红光，pi=(AC/DC)*100*0.9
			tmp_PI[num]=(float)(SpO2_P2P_Value[2][R_T_Idx]-SpO2_P2P_Value[3][R_B_Idx])*900/SpO2_P2P_Value[3][R_B_Idx];
		}
		if(IR_B_Idx<SpO2_P2P_Num[1]-1)
		{	
			Bak_Bot_Step_IR[1][0]=SpO2_P2P_Step[1][IR_B_Idx];
			Bak_Bot_Step_IR[1][1]=SpO2_P2P_Step[1][IR_B_Idx+1];		
		}
		Bak_Bot1_Num_IR[1]=2;		
		num++;
#endif		
	} 
	if(num==0)
	{
		return 0;
	}
	
	PI_Process(tmp_PI,num);	
	
	g_R_Value[type]=CalcRValue(r_now,num);
		
	if((g_Bak_R_Value[type]>0.4)&&(g_R_Value[type]-g_Bak_R_Value[type]>0.2))//当SpO2变化较大时，为了尽快反应出数据，故增加这一段代码
	{
		bak_buffer_rline[type][g_Bak_R_V_Idx[type]]=g_R_Value[type];
		g_Bak_R_V_Idx[type]=(g_Bak_R_V_Idx[type]+1)%BAK_SIZE_SPO2;
		g_Bak_R_V_Count[type]++;
		if(g_Bak_R_V_Count[type]>BAK_SIZE_SPO2)
		{
			g_Bak_R_V_Count[type]=BAK_SIZE_SPO2;
			g_R_V_Idx[type]=0;
			g_R_V_Count[type]=BAK_SIZE_SPO2;
			for(i=0;i<g_R_V_Count[type];i++)   //copy array
		 	{
		   	buffer_rline[type][i]=bak_buffer_rline[type][i];
		 	} 
		}
		else
		{
			g_PR=g_BakPR;
			g_SpO2=SpO2LookupTable(g_R_Value[type]);
			return SpO2IsValid(g_SpO2);				
		}
	}
	else
	{			
		g_Bak_R_Value[type]=g_R_Value[type];
		g_Bak_R_V_Count[type]=0;
		g_Bak_R_V_Idx[type]=0;	
		buffer_rline[type][g_R_V_Idx[type]]=g_R_Value[type];
		g_R_V_Idx[type]=(g_R_V_Idx[type]+1)%SIZE_SPO2;
		if(g_R_V_Count[type]<SIZE_SPO2)
		{
			g_R_V_Count[type]++;
		}
	}		
////////////////////////////////////////
	memcpy(tmp_buffer_rline,buffer_rline[type],g_R_V_Count[type]*sizeof(float));
	g_R_Value[type]=CalcRValue(tmp_buffer_rline,g_R_V_Count[type]);

/*	if(g_R_Value[type]<=0.5)
	{
		g_SpO2=100;
	}
	else
	{
		g_SpO2=(unsigned char)(COEFF_Z-COEFF_X*g_R_Value[type]*g_R_Value[type]-COEFF_Y*g_R_Value[type]);
	}		
	g_SpO2=CheckSpO2(g_SpO2);*/
	g_PR=g_BakPR;
	g_SpO2=SpO2LookupTable(g_R_Value[type]);
	return SpO2IsValid(g_SpO2);
	//return 1;
}

void CheckPulseBeat(u8 type)
{
	u8 i;
	u16 laststep;
	if(g_Bak_PR_Value[type]>200)
	{
		laststep=520;			
	}
	else if(g_Bak_PR_Value[type]>150)
	{
		laststep=500;			
	}
	else if(g_Bak_PR_Value[type]>100)
	{
		laststep=450;			
	}	
	else if(g_Bak_PR_Value[type]>40)
	{
		laststep=400;			
	}		
	else
	{
		laststep=300;
	}
	for(i=0;i<4;i++)
	{		
		if(P2P_Step[i][Step_Idx_End[i]]<laststep)
		{
			g_PRStepInterval=0;
		}	
	}
}

u8 PR_Process(u8 type)
{
	u16 tmp_pr[SIZE_PR];
	u16 pr;
	float Interval[4];	
	unsigned char i,j;
	unsigned char num=0;
	aver_step[type]=0.0;
	for(i=0;i<4;i++)
	{
		Interval[i]=0.0;
		if(SpO2_P2P_Num[i]>1)
		{
			for(j=0;j<SpO2_P2P_Num[i]-1;j++)
			{
				Interval[i]+=(SpO2_P2P_Step[i][j+1]-SpO2_P2P_Step[i][j]);
			}
			Interval[i]=(Interval[i]/(SpO2_P2P_Num[i]-1));
			aver_step[type]+=Interval[i];
			num++;
		}
	}
	aver_step[type]=(aver_step[type]/num);
	pr=(unsigned short)((AFE4490_SAMPLE_RATE*60)/aver_step[type]+0.5);

	if(pr<PR_MIN||pr>PR_MAX)
	{
		return 0;
	}
	if(g_FirstTestValue)
	{
		g_FirstTestValue=0;
		if((pr<60)||(pr>120))
		{
			return 0;
		}
	}
	if(g_Bak_PR_Value[type]!=0xffff&&(abs(pr-g_Bak_PR_Value[type])>10))		//当PR变化较大时，为了尽快反应出数据，故增加这一段代码
	{
		bak_buffer_pr[type][g_Bak_PR_V_Idx[type]]=pr;
		g_Bak_PR_V_Idx[type]=(g_Bak_PR_V_Idx[type]+1)%BAK_SIZE_PR;	
		if(g_Bak_PR_V_Count[type]<BAK_SIZE_PR)
		{
			/*if(g_Bak_PR_V_Count[type]>BAK_SIZE_PR-4)
			{
				g_PRStepInterval=0;		//怀疑找不到正确的峰值，暂时不播放脉搏音
			}*/
			CheckPulseBeat(type);
			g_Bak_PR_V_Count[type]++;
			return 1;
		}
		else
		{
			g_Bak_PR_V_Count[type]=BAK_SIZE_PR;
			g_Bak_PR_Value[type]=pr;
			g_PR_V_Idx[type]=BAK_SIZE_PR;
			g_PR_V_Count[type]=BAK_SIZE_PR;
			memcpy(buffer_pr[type],bak_buffer_pr[type],BAK_SIZE_PR);
		}
	}	
	else
	{
		g_PRStepInterval=(aver_step[type]+1);
		g_Bak_PR_V_Count[type]=0;
		g_Bak_PR_V_Idx[type]=0;	
		g_Bak_PR_Value[type]=pr;
		buffer_pr[type][g_PR_V_Idx[type]]=pr;
		g_PR_V_Idx[type]=(g_PR_V_Idx[type]+1)%SIZE_PR;
		if(g_PR_V_Count[type]<SIZE_PR)
		{
			g_PR_V_Count[type]++;
		}
	}		
	g_P2P_InterVal=aver_step[type]/4;
	
	switch(g_AverCycle)
	{
	//case 0:		//1 beat 担心出错率太高
		//break;
	case 1:			//4 beat
		if(g_PR_V_Count[type]>=4)
		{
			memcpy(tmp_pr,&(buffer_pr[type][g_PR_V_Count[type]-4]),4*sizeof(u16));	
			g_BakPR=CalcPR(tmp_pr,g_PR_V_Count[type]-4);		
		}
		else
		{
			memcpy(tmp_pr,buffer_pr[type],g_PR_V_Count[type]*sizeof(u16));	
			g_BakPR=CalcPR(tmp_pr,g_PR_V_Count[type]);	
		}
		break;
	case 2:			//8 beat
		if(g_PR_V_Count[type]>=8)
		{
			memcpy(tmp_pr,&(buffer_pr[type][g_PR_V_Count[type]-8]),8*sizeof(u16));	
			g_BakPR=CalcPR(tmp_pr,g_PR_V_Count[type]-8);		
		}
		else
		{
			memcpy(tmp_pr,buffer_pr[type],g_PR_V_Count[type]*sizeof(u16));	
			g_BakPR=CalcPR(tmp_pr,g_PR_V_Count[type]);	
		}
		break;
	//case 3:
	default:		//16 beat
		memcpy(tmp_pr,buffer_pr[type],g_PR_V_Count[type]*sizeof(u16));	
		g_BakPR=CalcPR(tmp_pr,g_PR_V_Count[type]);		
		break;
	}
	return PRIsValid(g_BakPR);
}

u8 PRSpO2_Process(u8 type)
{
	if(g_PRStepInterval>=40)
	{
		g_P2P_InterVal=(g_PRStepInterval/5+1);		
	}
	else
	{
		g_P2P_InterVal=5;
	}	
	
#if(THRESHOLD_ERROR_P2P==1)	
	if(g_CalcDataType==1)
	{
		if(!ThresholdErrorP2P(type))		
		{
			return 0;
		}
	}
#endif
	return CheckP2P(type);	
}

