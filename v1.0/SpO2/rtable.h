#ifndef __RTABLE_H__
#define __RTABLE_H__

#include "datatype.h"

#define   COEFF_X                 25.91337830195463
#define   COEFF_Y                 (-2.545238961423)
#define   COEFF_Z                 105.368139340531		//+1测试后发现较大部分数据比实际数据要小一点，故改为+2

u8 SpO2LookupTable(float rvalue);

#endif

