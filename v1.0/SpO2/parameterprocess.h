#ifndef __PARAMETER_PROCESS_H__
#define __PARAMETER_PROCESS_H__

#include "datatype.h"
#include "algorithm.h"

extern u8 P2P_Num[4];		//0:ired top 1:ired bot 2:red top 3:red bot
extern s32 P2P_Value[4][INFLECT_POINT_MAX];
extern u16 P2P_Step[4][INFLECT_POINT_MAX];//0:ired top 1:ired bot 2:red top 3:red bot

extern u8 Bak_P2P_Num[4];
extern s32 Bak_P2P_Value[4][INFLECT_POINT_MAX];
extern u16 Bak_P2P_Step[4][INFLECT_POINT_MAX];

extern u8 g_FirstTestValue;	//是不是第一次测出数据

extern u8 g_P2P_InterVal;	//动态调整认为峰值是正确的间隔值，当心率较快时，该值较小，当心率较慢时，该值较大

u8 PRSpO2_Process(u8 type);
u8 PR_Process(u8 type);
u8 SpO2_Process(u8 type);

unsigned char CheckP2P(u8 type);
unsigned char FindOptimalP2P(u8 type);

#endif

