#include "bsp.h"
#include "timer.h"
#include "afe4490.h"
//#include "afe4490oxiprocess.h"
//#include "digitaloxiprocess.h"
#include "global.h"
//#include "bciprotocol.h"
//#include "processpacket.h"
//#include "CalibrationRoutine.h"
//#inlcude "uart.h"
//u8 testwatchtimes=0;
#define GETADVALUE 0



int main(void)
{
//	testwatchtimes=0;
//	unsigned char hanzi[]=" 字符串";
	uint8_t time1s=0;
	int i=1000;
	RCC_ClocksTypeDef clock;
	//BSP_Init();
	//RCC_GetClocksFreq(&clock);
	for(i=0;i<MYORDER;i++)
	{
		Bx[i]=10000*Bx[i];
	}
	i=test();
	//InitCurrentGainIndicators();
	//while(i--);
//	
//   UART3_Send_Str(hanzi);
//	while(1);
//UART3_Send_bit('b');
	while(1)
	{		
/*		if(g_Simulation_Process_Flag)
		{
			testwatchtimes++;
		}*/
		//UART3_Send_bit('b');
	//	UART3_Send_bit('a');
		if(dataready>0)
		{
			dataready=0;
			//EXTI->IMR&=~(1<<3);
			GPIOC->ODR = GPIOC->ODR ^ (0x01<<10);
		//	UART3_Send_bit('R');
			AFE4490_Sampling();
			//Convert_Str(((Afe4490DataBuf[5]/1000)),str);
			bpfilter();
			time1s++;
			if(time1s==100)
			{
				time1s=0;
			}
			if(time1s%2==1||time1s%10==0)
			{
//			{	time1s=0;}
			  //SpO2=99;
			  bci_send();
			}
			else
			{
				if(BmpSound==1)
				{
					//BmpSoundflag1=1;
				}
				
			}
//bci_send();
			//UART3_Send_Str(str);
//			UART3_Send_bit('R');
//			Convert_Str((Output2+2000000),str);//发送红光value
//			UART3_Send_Str(str);
//			//UART3_Send_bit('S');
//			Convert_Str((SpO2h+2000000),str);//发送红光value
//			UART3_Send_Str(str);
//			Convert_Str((Output+2000000),str);//发送红光value
//			UART3_Send_Str(str);
//			UART3_Send_bit('S');
//			Convert_Str((Output+2000000),str);//发送红外光value
//			UART3_Send_Str(str);
//			Convert_Str((RLSout+2000000),str);//发送上包络线值-Envelope[200]
//			UART3_Send_Str(str);
//		//	UART3_Send_bit('S');
//			Convert_Str((dst_ref+2000000),str);//发送红外光-红光值Output2buff[165]-
//			UART3_Send_Str(str);		
//		//	UART3_Send_bit('T');
////			Convert_Str((Output2-Output+2000000),str);//发送红外光值
////			UART3_Send_Str(str);	
////			
//			Convert_Str((Pulse+100),str);//发送脉率值
//			UART3_Send_Str(str);
//			Convert_Str(((uint16_t)(SpO2)+100),str);//发送R值
//			UART3_Send_Str(str);		
//			Convert_Str((SensorOff+10),str);//发送脉搏音值
//	   	UART3_Send_Str(str);	
//			Convert_Str((RLSout2+2000000),str);//发送下包络线值
//			UART3_Send_Str(str);
////			Convert_Str((Output2-Output+2000000),str);//发送红光值
////			UART3_Send_Str(str);
//	//		Convert_Str((Output2+2000000),str);
//		//	Convert_Str(5000,str);
//	
//		//	UART3_Send_Str(str);
				//EXTI->IMR|=1<<3;
			
		}
#if(GETADVALUE==1)
		Afe4490_Oxi_Handle();
		AnalyseUart1Packet();
#endif
#if(WATCH_DOG==1)	
		//if(testwatchtimes<20)
		IWDG_ReloadCounter();	//喂狗
#endif		
	}
}
