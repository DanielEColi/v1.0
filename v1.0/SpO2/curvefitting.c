#include "curvefitting.h"
#include <math.h>
#include <string.h>

//傅立叶逼近  级数如何确定
//关键是如何把a,b,w系数反求出来，如使用前300个点反推算出系数

//弧度角度转换
//90°＝90×π/180 ＝π/2 弧度 

#define CHANNEL	2
#define XBUFFER_SIZE	2
#define Y1BUFFER_SIZE	2
#define Y2BUFFER_SIZE	1

#define PI	3.1415927

#define CONVERT	(PI/180)

#define a0		(0.5945)
#define a1		(-590.4)
#define b1		(-51.9)
#define a2		(-166.7)
#define b2		(372)
#define w			(0.07851)
/*
General model Fourier2:
       f(x) =  a0 + a1*cos(x*w) + b1*sin(x*w) + 
               a2*cos(2*x*w) + b2*sin(2*x*w)
Coefficients (with 95% confidence bounds):
       a0 =      0.5945  (-11.78, 12.97)
       a1 =      -590.4  (-608, -572.9)
       b1 =       -51.9  (-77.01, -26.79)
       a2 =      -166.7  (-195.2, -138.2)
       b2 =         372  (351.8, 392.2)
       w =     0.07851  (0.07847, 0.07855)

Goodness of fit:
  SSE: 1.032e+008
  R-square: 0.8012
  Adjusted R-square: 0.8006
  RMSE: 253.4*/

float a[2]={-590.4,-166.7};
float b[2]={-51.9,372};
	
/*	
s32 Curve_Fitting(s32 x)
{
	return a0+a1*cos(x*w*CONVERT)+b1*sin(x*w*CONVERT)+a2*cos(2*x*w*CONVERT)+b2*sin(2*x*w*CONVERT);
}*/


volatile double FittingXInTmp[CHANNEL][XBUFFER_SIZE]={0};			//输入缓冲
volatile double FittingY1OutTmp[CHANNEL][Y1BUFFER_SIZE]={0};	//输出缓冲
volatile double FittingY2OutTmp[CHANNEL][Y2BUFFER_SIZE]={0};	//实际输出
vu16 FittingXInTmpPos[CHANNEL]={1,1};				//当前输入缓冲位置
volatile u16 FittingY1OutTmpPos[CHANNEL]={1,1};		//当前输出缓冲位
volatile u16 FittingY2OutTmpPos[CHANNEL]={0,0};		//当前输出缓冲位置

void ResetCurveFitting(void)
{
	memset((void*)FittingXInTmp,0,sizeof(FittingXInTmp));
	memset((void*)FittingY1OutTmp,0,sizeof(FittingY1OutTmp));
	memset((void*)FittingY2OutTmp,0,sizeof(FittingY2OutTmp));
	memset((void*)FittingXInTmpPos,0,sizeof(FittingXInTmpPos));
	memset((void*)FittingY1OutTmpPos,0,sizeof(FittingY1OutTmpPos));
	memset((void*)FittingY2OutTmpPos,0,sizeof(FittingY2OutTmpPos));
}

/*
s = 0;
for k = 1:n    
	s = s + a(k)*cos((k-1)*w*x) + b(k)*sin((k-1)*w*x);
end*/

s32 Curve_Fitting(s32 x,u8 type)
{
	u8 i;
	double result=0.0;
	FittingXInTmp[type][0]=FittingXInTmp[type][1];
	FittingXInTmp[type][1]=x;
	
	for(i=0;i<2;i++)
	{
		result=result+a[i]*cos(i*w*FittingXInTmp[type][i]*CONVERT)+b[i]*sin(i*w*FittingXInTmp[type][i]*CONVERT);
	}
	return (s32)(result);
}



