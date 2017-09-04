#include "ambientcancellationdac.h"
#include "afe4490.h"

//为节省ADC动态范围可使用模拟方式的环境光消减，即使用Ambient-Cancellation DAC来消减环境光。
//扩展ADC范围

vu8 g_AmbientCancellationDacLevel=0;		//取消环境光DAC级别

void AmbientCancellationDac(s32 amb_ir,s32 amb_r)
{	
	s32 Array[10]={50000,100000,200000,300000,400000,500000,600000,700000,800000,850000};
	s8 i;
/*	if(amb_ir<-10000)
	{
		WriteCommand(0x21,0x0000);
	}*/
/*	if(amb_ir<0)	//cancel 1ua
	{
		WriteCommand(0x21,0x0100);
	}	
	if(amb_ir>800000||amb_r>800000)	//cancel 10ua
	{
		WriteCommand(0x21,0x0A00);
	}	
	if(amb_ir>800000||amb_r>800000)	//cancel 10ua
	{
		WriteCommand(0x21,0x0A00);
	}	*/
	
	for(i=10;i>0;i--)
	{
		if((amb_ir>Array[i])||(amb_r>Array[i]))
		{
			if(g_AmbientCancellationDacLevel!=i)
			{
				WriteCommand(0x21,i<<16);
				g_AmbientCancellationDacLevel=i;
			}
			break;
		}
	}
}
