#ifndef _DEALDATA_H_
#define _DEALDATA_H_

#include "datatype.h"

extern u32 sn_min,sn_max;

extern s32 s32_value_min,s32_value_max;

s32 GetMin(s32 a,s32 b);
s32 GetMax(s32 a,s32 b);

u32  CalcAverFreq(vu32* pbuf,u16 len);
void SortFloat(float* pBuf,u16 len);
void SortU8(u8* pBuf,u16 len);
void SortU16(u16 *vf_buf, u16 len_data);
void SortVU32(vu32 *pBuf,u16 len);
void sort_dunk_float(float *vf_buf, u8 len_data);

u32 Sum_VU32(vu32 *vf_buf,u16 len);

void MaxMinSn_S32(s32 *vf_buf,u16 len);

void sort_dunk_byte(u8 *vf_buf, u8 len_data);

u8 PRIsValid(u16 value);
u8 SpO2IsValid(u8 value);

u32 GetValueInterval(u32 a,u32 b);
s32 GetS32ValueInterval(s32 a,s32 b);

u16 CalcPR(u16* pr,u8 len);
u8 CalcSpO2(u8*spo2,u8 len);
float CalcRValue(float* rvalue,u8 len);
u8 CheckSpO2(u8 spo2);

s32 FindMin(s32* pBuf,u16 len);
s32 FindMax(s32* pBuf,u16 len);

#endif

