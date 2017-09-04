#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "datatype.h"

#define NULL	0

//两个峰值相差3个以上，认为是错误的



#define SIZE_SAMPLE_FRQ 100
#define TIMER_SECOND            6

#if(ANIMAL==1)
#define COUNT_POINT_MIN 				7
#define COUNT_POINT_MAX 				150
#else
#define COUNT_POINT_MIN 				19
#define COUNT_POINT_MAX 				119
#endif

#define INFLECT_POINT_MAX       42		//最大峰值个数 
#define INFLECT_POINT_MIN       2

/*#define SIZE_SPO2               10
#define SIZE_PR                 10
#define BAK_SIZE_SPO2			5
#define BAK_SIZE_PR				5*/

#define SIZE_SPO2               20
#define SIZE_PR                 20
#define BAK_SIZE_SPO2			10
#define BAK_SIZE_PR				10

#define   COEFF_X                 25.91337830195463
#define   COEFF_Y                 (-2.545238961423)
#define   COEFF_Z                 105.368139340531		//+1测试后发现较大部分数据比实际数据要小一点，故改为+2

extern vu16 Bak_Bot_Step_IR[2][INFLECT_POINT_MAX];	//备份的波谷值，用于计算脉搏音位置
extern vu8 Bak_Bot1_Num_IR[2];

extern vu8 g_PR_V_Idx[2];
extern vu8 g_PR_V_Count[2];

extern vu8 g_R_V_Idx[2];
extern vu8 g_R_V_Count[2];

extern float g_R_Value[2];//, g_R_Value_bak;
extern float buffer_rline[2][SIZE_SPO2];

extern u16 g_Bak_PR_Value[2];
extern u16 bak_buffer_pr[2][BAK_SIZE_PR];
extern vu8 g_Bak_PR_V_Idx[2];
extern vu8 g_Bak_PR_V_Count[2];

extern float g_Bak_R_Value[2];
extern float bak_buffer_rline[2][SIZE_SPO2];
extern vu8 g_Bak_R_V_Idx[2];
extern vu8 g_Bak_R_V_Count[2];

extern u16 buffer_pr[2][SIZE_PR];


extern float aver_step[2];

extern u8 OXI_Handle(u16 len,u8 type);
extern u8 GetP2PValue(u16 len,u8 type);

u8 AddOriginalData(s32 data,u8 type);

s32 GetFilterData(const s32* in, double* b,u16 order);
//u32 GetFilterData(const u32* in, short* b,u16 order);

void ClearProcessOxiData(u8 type);
void ClearParameterData(u8 type);

#if(SPO2_METHOD_AREA==1)
u8 CalcRValueArea(u16 bot_ir_step1,u16 bot_ir_step2,u16 bot_r_step1,u16 bot_r_step2,float* r);
#endif

#endif


