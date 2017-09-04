#include "otheralgorithm.h"
#include "algorithm.h"
#include "global.h"
#include "afe4490.h"
#include "spo2tools.h"
#include <string.h>

//角度转弧度
#define ANGLE_TO_RADIAN(x)	(3.1415926*(x)/180)

//type 0:digital 1:sim
//第二方案，仅判断红外光，红光参考红外光数据
//num:min p num
u8 Second_PR_Process(u8 type)
{
	u16 tmp_pr[SIZE_PR];
	u16 pr;

	g_ClearTimes[type]++;
	if(valid_num[type]==2)
	{		
		g_ClearTimes[type]=0;
		pr=((AFE4490_SAMPLE_RATE*60)/aver_step[type]+0.5);
		if(pr<PR_MIN||pr>PR_MAX)
		{
			return 0;
		}
		g_PR[type]=pr;
		buffer_pr[type][g_PR_V_Idx[type]]=pr;
		g_PR_V_Idx[type]=(g_PR_V_Idx[type]+1)%SIZE_PR;
	  if(g_PR_V_Count[type]<SIZE_PR)
	  {
			g_PR_V_Count[type]++;
		}
    memcpy(tmp_pr,buffer_pr[type],g_PR_V_Count[type]*sizeof(u16));	
		g_PR[type]=CalcPR(tmp_pr,g_PR_V_Count[type]); 
	}
	return 1;
}
//type 0:digital 1:sim
//第二方案，仅判断红外光，红光参考红外光数据
//num:min p num
u8 Second_SpO2_Process(u8 type)
{
	float tmp_buffer_rline[SIZE_SPO2];
	float r_now[30];
	float k=0.0;
	float r;
	u16 i,j;
	u8 num=0;	
	u16 ired_top_idx=0;
	u16 ired_bottom_idx=0;
	u16 red_top_idx=0;
	u16 red_bottom_idx=0;
	u8 find[4]={0,0,0,0};

	for(i=ired_top_start[type];i<=ired_top_end[type];i++)
	{
		ired_top_idx=i;
		find[0]=1;
		find[1]=0;
		for(j=ired_bottom_start[type];j<=ired_bottom_end[type];j++)
		{
			if(Top_Step_IR[type][i]<Bot_Step_IR[type][j]&&(Bot_Step_IR[type][j]-Top_Step_IR[type][i])<top_ired_aver_step[type])
			{
				find[1]=1;
				ired_bottom_idx=j;
				break;	
			}
		}
		if(find[1]==0)
		{
			continue;
		}
		//r_now[num]=(float)((Afe4490_Top_Value_R[red_top_idx]-Afe4490_Bot_Value_R[red_bottom_idx])*Afe4490_Bot_Value_IR[ired_bottom_idx])/((Afe4490_Top_Value_IR[i]-Afe4490_Bot_Value_IR[ired_bottom_idx])*Afe4490_Bot_Value_R[red_bottom_idx]);
		r_now[num]=(float)(Top_Value_R[type][red_top_idx]-Bot_Value_R[type][red_bottom_idx])/(Top_Value_IR[type][ired_top_idx]-Bot_Value_IR[type][ired_bottom_idx])*((float)Bot_Value_IR[type][ired_bottom_idx]/Bot_Value_R[type][red_bottom_idx]);
		num++;
	} 

	if(num)
  {
		sort_dunk_float(r_now,num);
//普通			
		if(num>=8)
		{
			for(i=3;i<num-3;i++)
			{
				k+=r_now[i];
			}
			g_R_Value[type]=(float)k/(num-6);
		}
		else if(num>=6)
		{
			for(i=2;i<num-2;i++)
			{
				k+=r_now[i];
			}
			g_R_Value[type]=(float)k/(num-4);
		}
		else if(num>2)
		{
			for(i=1;i<num-1;i++)
			{
				k+=r_now[i];
			}
			g_R_Value[type]=(float)k/(num-2);
		}
		else
		{
			for(i=0;i<num;i++)
			{
				k+=r_now[i];
			}
			g_R_Value[type]=(float)k/num;
		}
////////////////////////////////////////
		if((g_Bak_R_Value[type]>0.4)&&(g_R_Value[type]-g_Bak_R_Value[type]>0.2))
		{
			bak_buffer_rline[type][g_Bak_R_V_Idx[type]]=g_R_Value[type];
			g_Bak_R_V_Idx[type]++;
			g_Bak_R_V_Count[type]++;
			if(g_Bak_R_V_Idx[type]>=BAK_SIZE_SPO2)
			{
				g_Bak_R_V_Idx[type]=0;
			}
			if(g_Bak_R_V_Count[type]>BAK_SIZE_SPO2)
			{
				g_Bak_R_V_Count[type]=BAK_SIZE_SPO2;
//				g_Bak_R_Value=g_R_Value;
				g_R_V_Idx[type]=BAK_SIZE_SPO2;
				g_R_V_Count[type]=BAK_SIZE_SPO2;
				for(i=0;i<g_R_V_Count[type];i++)   //copy array
		    {
		     	buffer_rline[type][i]=bak_buffer_rline[type][i];
		    } 
			}
			else
			{
				return 1;
			}
		}
		else
		{			
			g_Bak_R_V_Count[type]=0;
			g_Bak_R_V_Idx[type]=0;	
			buffer_rline[type][g_R_V_Idx[type]++]=g_R_Value[type];
		  g_R_V_Count[type]++;
		}
////////////////////////////////////////
	  if(g_R_V_Idx[type]>=SIZE_SPO2)
	  {
	   	g_R_V_Idx[type]=0;
	  }
	  if(g_R_V_Count[type]>=SIZE_SPO2)
	  {
	   	g_R_V_Count[type]=SIZE_SPO2;
		}
	 	for(i=0;i<g_R_V_Count[type];i++)   //copy array
	 	{
	   	tmp_buffer_rline[i]=buffer_rline[type][i];
	 	} 
		sort_dunk_float(tmp_buffer_rline,g_R_V_Count[type]);
	  r=0.0;
	  if(g_R_V_Count[type]>=SIZE_SPO2)
	  {
	   	for(i=3;i<7;i++)
	    {
	     	r+=tmp_buffer_rline[i];
	    }
	    g_R_Value[type]=r/4;
	  }
	  else if(g_R_V_Count[type]>=8)
	  {
     	for(i=3;i<g_R_V_Count[type]-3;i++)   //remove max & min value
     	{
       	r+=tmp_buffer_rline[i];
     	}
     	g_R_Value[type]=r/(g_R_V_Count[type]-6);
		}
	  else if(g_R_V_Count[type]>=6)
	  {
     	for(i=2;i<g_R_V_Count[type]-2;i++)   //remove max & min value
     	{
     		r+=tmp_buffer_rline[i];
     	}
     	g_R_Value[type]=r/(g_R_V_Count[type]-4);
		}
	  else if(g_R_V_Count[type]>=3)
	  {
	   	for(i=1;i<g_R_V_Count[type]-1;i++)   //remove max & min value
	    {
	     	r+=tmp_buffer_rline[i];
	   	}
	   	g_R_Value[type]=r/(g_R_V_Count[type]-2);
	  }
	  else
	  {
	   	for(i=0;i<g_R_V_Count[type];i++)
	   	{
	   		r+=tmp_buffer_rline[i];
	   	}
	   	g_R_Value[type]=r/g_R_V_Count[type];
	  }		   
		if(g_R_Value[type]<=1.8)
		{
			if(g_R_Value[type]<=0.5)
			{
				g_SpO2[type]=SPO2_MAX;
			}
			else
			{
				g_SpO2[type]=(u8)(COEFF_Z-COEFF_X*g_R_Value[type]*g_R_Value[type]-COEFF_Y*g_R_Value[type]);
	    }
		}
	  else if(g_R_Value[type]<2.3)
	  {
	   	g_SpO2[type]=(u8)(150-(720*g_R_Value[type])/11);
		}
	  else
	  {
			g_SpO2[type]=0x7f;
		}
		if(g_SpO2[type]>=SPO2_MAX)
		{
			g_SpO2[type]=SPO2_MAX-1;
		}
	  else if(g_SpO2[type]<SPO2_MIN)   
	  {
			g_SpO2[type]=0x7f;
	  }
		g_Bak_R_Value[type]=g_R_Value[type];
		return 1;
	}	
	return 0;
}


