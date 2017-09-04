#include "rtable.h"
#include "spo2tools.h"

/*
70%
72%
74%
76%
78%
80%
82%
84%
86%
88%
90%
92%
94%
96%
98%
100%*/

//70-100%
float RTable[]=
{
	1.240,
	1.217,			//69%
	1.190,			//70%
	1.176,			//71%
	1.154,			//72%
	1.133,			//73%
	1.115,			//74%
	1.101,			//75%	
	1.080,			//76%
	1.065,			//78%
	1.040,			//79%
	1.015,			//80%
	1.009,			//81%
	0.975,			//82%
	0.950,			//83%
	0.923,			//84%
	0.900,			//85%
	0.882,			//86%
	0.860,			//87%
	0.844,			//88%
	0.810,			//89%
	0.779,			//90%
	0.750,			//91%
	0.722,			//92%
	0.690,			//93%
	0.668,			//94%
	0.630,			//95%
	0.600,			//96%
	0.570,			//97%
	0.543,			//98%
	0.500,			//99%
	0.000,			//100%
};

//return spo2 value
u8 SpO2LookupTable(float rvalue)
{
	u8 spo2;
	u8 i;
	for(i=0;i<sizeof(RTable)/sizeof(float)-1;i++)
	{
		if(rvalue>RTable[i+1]&&rvalue<RTable[i])
		{
			return 70+i;
		}
	}		
	spo2=(u8)(COEFF_Z-COEFF_X*rvalue*rvalue-COEFF_Y*rvalue);
	if(SpO2IsValid(spo2))
	{
		return spo2;
	}
	return 0xff;
}

