#include "threshold.h"
#include "parameterprocess.h"
#include "spo2tools.h"
#include <string.h>

int Ir_base,R_base;

void Calc_Baseline(void)
{
	int sum=0;
	int i;
	for(i=0;i<Bak_P2P_Num[1];i++)
	{
		sum+=Bak_P2P_Value[1][i];
	}
	Ir_base=sum/Bak_P2P_Num[1];

	sum=0;
	for(i=0;i<Bak_P2P_Num[3];i++)
	{
		sum+=Bak_P2P_Value[3][i];
	}
	R_base=sum/Bak_P2P_Num[3];
}

#define THRESHOLD_RATE	(6) 

void FilterP2P(void)
{
	int interval;
	int i,j,k;

	memcpy(Bak_P2P_Num,P2P_Num,sizeof(P2P_Num));
	memcpy(Bak_P2P_Step,P2P_Step,sizeof(P2P_Step));
	memcpy(Bak_P2P_Value,P2P_Value,sizeof(P2P_Value));

	i=0;
	k=0;

	while(i<Bak_P2P_Num[1]-1)
//	for(i=0;i<Bak_P2P_Num[1];i++)
	{
		for(k=0;k<Bak_P2P_Num[0];k++)
		{
			if(Bak_P2P_Step[1][i]<Bak_P2P_Step[0][k])
			{
				break;
			}
		}
		interval=Bak_P2P_Value[0][k]-Ir_base;

		if(Bak_P2P_Value[1][i]>interval/THRESHOLD_RATE+Ir_base)
		{
			for(j=0;j<Bak_P2P_Num[1]-i;j++)
			{
				Bak_P2P_Step[1][i+j]=Bak_P2P_Step[1][i+j+1];
				Bak_P2P_Value[1][i+j]=Bak_P2P_Value[1][i+j+1];
			}
			Bak_P2P_Num[1]--;
			continue;
		}
		i++;
	}
//最后一个bottom值
	interval=Bak_P2P_Value[0][Bak_P2P_Num[0]-1]-Ir_base;
	if(Bak_P2P_Value[1][Bak_P2P_Num[1]-1]>interval/THRESHOLD_RATE+Ir_base)
	{
		Bak_P2P_Num[1]--;
	}

	i=0;
	k=0;
	while(i<Bak_P2P_Num[3]-1)
//	for(i=0;i<Bak_P2P_Num[1];i++)
	{
		for(k=0;k<Bak_P2P_Num[2];k++)
		{
			if(Bak_P2P_Step[3][i]<Bak_P2P_Step[2][k])
			{
				break;
			}
		}
		interval=Bak_P2P_Value[2][k]-R_base;

		if(Bak_P2P_Value[3][i]>interval/THRESHOLD_RATE+R_base)
		{
			for(j=0;j<Bak_P2P_Num[3]-i;j++)
			{
				Bak_P2P_Step[3][i+j]=Bak_P2P_Step[3][i+j+1];
				Bak_P2P_Value[3][i+j]=Bak_P2P_Value[3][i+j+1];
			}
			Bak_P2P_Num[3]--;
			continue;
		}
		i++;
	}
//最后一个bottom值
	interval=Bak_P2P_Value[2][Bak_P2P_Num[2]-1]-R_base;
	if(Bak_P2P_Value[3][Bak_P2P_Num[3]-1]>interval/THRESHOLD_RATE+R_base)
	{
		Bak_P2P_Num[3]--;
	}

	for(i=0;i<Bak_P2P_Num[1];i++)
	{
		for(j=0;j<Bak_P2P_Num[0]-1;j++)
		{
			if(Bak_P2P_Step[0][i]>Bak_P2P_Step[1][j]&&Bak_P2P_Step[0][i]<Bak_P2P_Step[1][j+1])
			{
				break;
			}
		}
		for(j=i;j<Bak_P2P_Num[0]-1;j++)
		{
			Bak_P2P_Step[0][j]=Bak_P2P_Step[0][j+1];
			Bak_P2P_Value[0][j]=Bak_P2P_Value[0][j+1];
		}
		Bak_P2P_Num[0]--;
	}

	for(i=0;i<Bak_P2P_Num[3];i++)
	{
		for(j=0;j<Bak_P2P_Num[2]-1;j++)
		{
			if(Bak_P2P_Step[2][i]>Bak_P2P_Step[3][j]&&Bak_P2P_Step[2][i]<Bak_P2P_Step[3][j+1])
			{
				break;
			}
		}
		for(j=i;j<Bak_P2P_Num[2]-1;j++)
		{
			Bak_P2P_Step[2][j]=Bak_P2P_Step[2][j+1];
			Bak_P2P_Value[2][j]=Bak_P2P_Value[2][j+1];
		}
		Bak_P2P_Num[2]--;
	}
}

void FindTopP(void)
{
	unsigned char find;
	int max,step;
	unsigned char i,j,k;

	memcpy(P2P_Num,Bak_P2P_Num,sizeof(P2P_Num));
	memcpy(P2P_Step,Bak_P2P_Step,sizeof(P2P_Step));
	memcpy(P2P_Value,Bak_P2P_Value,sizeof(P2P_Value));

	for(i=1;i<4;i+=2)
	{		
		P2P_Num[i-1]=0;
		for(j=0;j<Bak_P2P_Num[i]-1;j++)
		{
			find=0;
			for(k=0;k<Bak_P2P_Num[i-1];k++)
			{
				if(Bak_P2P_Step[i-1][k]>Bak_P2P_Step[i][j]&&Bak_P2P_Step[i-1][k]<Bak_P2P_Step[i][j+1])
				{
					if(find==0)
					{
						find=1;
						max=Bak_P2P_Value[i-1][k];
						step=Bak_P2P_Step[i-1][k];
					}
					else
					{
						if(Bak_P2P_Value[i-1][k]>=max)
						{
							max=Bak_P2P_Value[i-1][k];
							step=Bak_P2P_Step[i-1][k];
						}
					}
				}
			}
			if(find)
			{
				P2P_Value[i-1][P2P_Num[i-1]]=max;
				P2P_Step[i-1][P2P_Num[i-1]]=step;
				P2P_Num[i-1]++;	
			}
		}
	}	
}

//检测是否存在错误的峰值，阀值判断法
unsigned char ThresholdErrorP2P(unsigned char type)
{
	int interval;
	int ir_min,r_min,ir_max,r_max;
	unsigned char i,j,min;

	memcpy(Bak_P2P_Num,P2P_Num,sizeof(P2P_Num));
	memcpy(Bak_P2P_Step,P2P_Step,sizeof(P2P_Step));
	memcpy(Bak_P2P_Value,P2P_Value,sizeof(P2P_Value));

	MaxMinSn_S32(Bak_P2P_Value[0],Bak_P2P_Num[0]);
	ir_max=s32_value_max;
	MaxMinSn_S32(Bak_P2P_Value[1],Bak_P2P_Num[1]);
	ir_min=s32_value_min;
	MaxMinSn_S32(Bak_P2P_Value[2],Bak_P2P_Num[2]);
	r_max=s32_value_max;
	MaxMinSn_S32(Bak_P2P_Value[3],Bak_P2P_Num[3]);
	r_min=s32_value_min;
	min=GetMin(Bak_P2P_Num[0],Bak_P2P_Num[1]);
	if(min>=2)
	{
		interval=ir_max-ir_min;
		i=0;
		while(i<Bak_P2P_Num[1])
		{
			if(Bak_P2P_Value[1][i]>(ir_min+interval/5))
			{
				for(j=0;j<Bak_P2P_Num[1];j++)
				{
					Bak_P2P_Step[1][i+j]=Bak_P2P_Step[1][i+j+1];
					Bak_P2P_Value[1][i+j]=Bak_P2P_Value[1][i+j+1];
				}
				Bak_P2P_Num[1]--;
				continue;
			}
			i++;
		}
		interval=r_max-r_min;
		i=0;
		while(i<Bak_P2P_Num[3])
		{
			if(Bak_P2P_Value[3][i]>(r_min+interval/5))
			{
				for(j=0;j<Bak_P2P_Num[3];j++)
				{
					Bak_P2P_Step[3][i+j]=Bak_P2P_Step[3][i+j+1];
					Bak_P2P_Value[3][i+j]=Bak_P2P_Value[3][i+j+1];
				}
				Bak_P2P_Num[3]--;
				continue;
			}
			i++;
		}
	}
	//Calc_Baseline();
	//FilterP2P();
	FindTopP();
	
	return 1;
}

