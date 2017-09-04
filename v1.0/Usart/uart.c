#include "global.h"
//#include "uart.h"
//uint8_t BmpSoundflag1=0;
uint8_t uart1buffer[30]={0};
volatile uint8_t Tx_L=0;
volatile uint8_t Tx_Index=0;
uint8_t pos=0;
uint8_t BmpSoundflag1=0;
uint8_t pakege0,pakege1,pakege2,pakege3,pakege4,pakege5,pakege6,Sp=0,Pr=0,Spold=0,Prold=0,SensorOffold;
uint8_t fixpulse[3]={60,25,60};
void UART3_Send_Str(unsigned char *s)
{
        unsigned char i=0; 

        while(s[i]!='\0')  
        {
                USART_SendData(USART1,s[i]);        
                while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);  

                i++;                
        }

}
void UART3_Send_bit(char value)
{
								USART_SendData(USART1,value);        
                while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
}
void Convert_Str( unsigned int num, unsigned char *str )

{
 
    unsigned int tempnum ;
	  char j=0;
	  //tempnum=num;
   if ( num >= 5000000 )
   {
       num=0 ;
    }	
    tempnum=num;
	  
	  while ( tempnum )          // ??????
	  {
		    tempnum = tempnum / 10;
		    j++;
	  }
	  
	  str[j]=0;
    
    while ( num )                // ?????????  
	  {  
		    j--;
		    str[j] = num % 10 + '0'; //???
		    num = num / 10;
    }
}//end of void Convert_Str(word num, char *str);
void bci_send()
{
	//Pulse=75;
	uint8_t searchbmp=0,searchtimeout=0;
	static uint16_t time10s=0;
        if(SensorOff==1)
        {
          time10s=0;
        }
        else
        {
          time10s++;
        }
        if(time10s>0&&time10s<900)
        {
          if(Pulse==0)
          {
            searchbmp=1;
          }
        }
        if(time10s>=900)//15s?????
        {
          time10s=900;
          if(Pulse==0||SpO2h==0)
          {
            searchtimeout=1;
          }
        }
//        if(time10s>=0&&time10s<=150)
//        {
//          StickFigure=0;
//          NormalRatioBuffer[79]=0;
//          //SpO2h=0;
//        }        
//        if(time10s>=0&&time10s<=300)
//        {
//
//          SpO2h=0;
//        }
//	if(Pulse==0||SpO2h==0)
//	{
////		Pulse=0;
////		SpO2=0;
//		if(SensorOff==0)
//		{
//			searchbmp=1;
//			time10s++;
//		}
//		if(time10s>=1000)
//		{
//			searchtimeout=1;
//		}
//		if(time10s>=0&&time10s<=100)
//		{
//			StickFigure=0;
//			NormalRatioBuffer[79]=0;
//			//SpO2h=0;
//		}
//                if(time10s>=0&&time10s<=400)
//		{
//			
//			SpO2h=0;
//		}
//	}
//	if(searchbmp==0)
//	{
//		time10s=0;
//	}
	if(Pulse==0)
	{
		//NormalRatioBuffer[20]=0;
		//StickFigure=0;
	//	SpO2h=0;
		BmpSound=0;
	}
	if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5))==1||isFake==true)//||isFake2==true
	{
		NormalRatioBuffer[79]=0;
		StickFigure=0;
		SpO2h=0;
		Pulse=0;
		BmpSound=0;
                SensorOff=1;
                searchbmp=0;
	}
//	if(SpO2h<40)
//	{SpO2h=0;}
//	else if(SpO2h<50)
//	{SpO2h=50;}
	if(BmpSoundflag1==1)
	{
		BmpSound=1;
		BmpSoundflag1=0;
	}
	if(StickFigure==0)
	{
		BmpSound=0;
	}
        if(Pulse!=0)//SpO2h!=0&&
        {
          Sp=SpO2h;
          Pr=Pulse;
          
        }
        else
        {
          Sp=0;
          Pr=0;
          PerI=0;
        }
        if(IsChangeMin==true)
        {
          StickFigure=0;
          NormalRatioBuffer[79]=0;
        }
//        if(MaxwindowAmp>15)MaxwindowAmp=15;
//       Signal_Quality=MaxwindowAmp;
//       Pr=Pulse;
//       Sp=SpO2h;
       if(Pr==0||Sp==0)
       {
        BmpSound=0;
       }
       if(Covxy<=0)Covxy=0;
       //??????,????????,???????,?????
       if(IsChangeLight==true)
       {
        SensorOff=SensorOffold;
        Pr=Prold;
        Sp=Spold;
       }
       else
       {
        SensorOffold=SensorOff;
        Prold=Pr;
        Spold=Sp;
       }
       if(IsChangeMin==true)
       {
        SensorOffold=1;
        Prold=0;
        Spold=0;
       }
       //PerI=Covxy*100;
//       if(PerI<3&&Pr<=45)
//       {
//        BmpSound=0;
//       }
	//pakege0=0x80|(BmpSound<<6)|(searchtimeout<<4)|Signal_Quality;//||PB5 Ч'м?'ъ±н'??D?чк?·?б??у
	pakege0=0x80|((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)<<5)&0x020)|(BmpSound<<6)|(searchtimeout<<4)|Signal_Quality;//||PB5 Ч'м?'ъ±н'??D?чк?·?б??у
	pakege1=0x00|NormalRatioBuffer[79];//-20*bmpsoundflag
	pakege2=0x00|(((uint8_t)Pr&0x80)>>1)|(SensorOff<<4)|(StickFigure&0x0f)|(searchbmp<<5);
	pakege3=0x00|(uint8_t)(Pr&0x07f);//0x00|(((uint8_t)Pulse&0x7f));
	pakege4=0x00|(uint8_t)Sp;
        pakege5=0x00|(((uint8_t)PerI>>1)&0x7f);
        pakege6=0x00|((uint8_t)PerI&0x01)<<7;
        
        Fillbuffer2();
}
void bci_send2()
{
  
  //UART_HAL_Putchar(g_uartBaseAddr[HW_UART1],uart1buffer[Tx_Index]);
  USART_SendData(USART1,uart1buffer[Tx_Index]);
  Tx_Index++;
  if(Tx_Index==35)
  {
    Tx_Index=0;
  }
  Tx_L--;
  if(Tx_L==0)USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//BW_UART_C2_TIE(g_uartBaseAddr[HW_UART1], 0);//??????
  //if()
}

void Fillbuffer2()
{
  
  uart1buffer[pos++]=pakege0;
  uart1buffer[pos++]=pakege1;
  uart1buffer[pos++]=pakege2;
  uart1buffer[pos++]=pakege3;
  uart1buffer[pos++]=pakege4;
  uart1buffer[pos++]=pakege5;  
  uart1buffer[pos++]=pakege6;
  if(pos==35)
  {
    pos=0;
  }
  Tx_L=Tx_L+7;
  //BW_UART_C2_TIE(g_uartBaseAddr[HW_UART1], 1);//?????
	USART_ITConfig(USART1,USART_IT_TXE,ENABLE);  
  
  
}
