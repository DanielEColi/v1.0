#include "bsp.h"
#include "datatype.h"
#include "afe4490.h"

/*******************************************************************************
* Function Name  : RCC_Configuration 
* Description    :  RCC配置(使用外部8MHz晶振)
* Input            : 无
* Output         : 无
* Return         : 无
*******************************************************************************/
void RCC_Configuration(void)
{
  static u32 HSEStartUpStatus;
  /*将外设RCC寄存器重设为缺省值 */
  RCC_DeInit();

  /*设置外部高速晶振（HSE）*/
  RCC_HSEConfig(RCC_HSE_ON);   //RCC_HSE_ON——HSE晶振打开(ON)

  /*等待HSE起振*/
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  while(HSEStartUpStatus != SUCCESS)	  //SUCCESS：HSE晶振稳定且就绪
  {
     HSEStartUpStatus = RCC_WaitForHSEStartUp();
     //设置AHB时钟（HCLK） 
	}
  RCC_HCLKConfig(RCC_SYSCLK_Div1);  //RCC_SYSCLK_Div1——AHB时钟 = 系统时钟 120M
  // 设置低速AHB时钟（PCLK1） 
  RCC_PCLK1Config(RCC_HCLK_Div4);	//RCC_HCLK_Div1——APB2时钟 = HCLK    30M
  //设置高速AHB时钟（PCLK2）    
	RCC_PCLK2Config(RCC_HCLK_Div2);	 //RCC_HCLK_Div2——APB1时钟 = HCLK / 2  60M	   
  //设置FLASH存储器延时时钟周期数
  FLASH_SetLatency(FLASH_Latency_2);    //FLASH_Latency_2  2延时周期
  /* 设置PLL时钟源及倍频系数了PLLCLK = 8MHz * 9 = 72 MHz */
#if(USE_AFE4490_EVM==1)	//8m
  RCC_PLLConfig(RCC_PLLSource_HSE ,8,240,2,5); 
#else										//4m
 RCC_PLLConfig(RCC_PLLSource_HSE ,8,240,2,5); //RCC_PLLConfig(RCC_PLLSource_HSE ,15,270,2,3);//RCC_PLLConfig(RCC_PLLSource_HSE ,20,240,2,2); // 
//  RCC_PLLConfig(RCC_PLLSource_HSE ,4,240,2,4); //4mff
#endif	
  //使能PLL 
  RCC_PLLCmd(ENABLE); 

  //检查指定的RCC标志位(PLL准备好标志)设置与否   
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)	  
  {
  }

  //设置系统时钟（SYSCLK）  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
	//RCC_SYSCLKSource_PLLCLK——选择PLL作为系统时钟

  // PLL返回用作系统时钟的时钟源
  while(RCC_GetSYSCLKSource() != 0x08)	  //0x08：PLL作为系统时钟
  { 
  }
	
	SystemCoreClockUpdate();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE); 
}

void TIM_Configuration(void)//TIM初始化函数
{ 
/*	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定时器初始化结构
//Timer 1	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//TIM1初始化	频120M/(119+1)/10000=100=10ms	 
 	TIM_TimeBaseStructure.TIM_Period = 10000-1;    //周期0～FFFF
 	TIM_TimeBaseStructure.TIM_Prescaler = 120-1;          //时钟分频
 	TIM_TimeBaseStructure.TIM_ClockDivision = 0;      //时钟分割
 	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//模式
 	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);   //基本初始化
 	//TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);//打开中断，中断需要这行代码
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);      */
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;  	
#if(USE_AFE4490_EVM==1)	
//PE2,work led  cpu指示灯-----------------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
 	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4; 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;// GPIO_OType_PP;//
 	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   
 	GPIO_Init(GPIOC, &GPIO_InitStructure); 	
#else
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;					//sensor not connect pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
#endif	
//mcs测试afe4490发光
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//PC9 	AFE_PDN
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		
//PA2 ADC_Reset	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
//PC6 PD_ALM PC7 LED_ALM PC8 DIAG_END
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
//PA3 ADC_RDY	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;// GPIO_OType_PP;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   
  GPIO_Init(GPIOA, &GPIO_InitStructure); 	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void EXTI_Configuration(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure; 

	/*配置EXTI线0上出现下降沿，则产生中断*/
		//SPI_DRDY脚	用GPIO_Mode_AIN无法得到输入结果 ,下降沿中断
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
	
#if(USE_AFE4490_EVM==1)	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//允许EXTI4中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	EXTI_GenerateSWInterrupt(EXTI_Line4);
#else	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

/*	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);		
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);		*/
	//允许EXTI3中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	EXTI_GenerateSWInterrupt(EXTI_Line3);
#endif	
}

//串口1后添加
void Usart_Init(void)
{
	GPIO_InitTypeDef  	 GPIO_InitStructure;  	
  USART_InitTypeDef	 USART_InitStructure;

//#if(USE_AFE4490_EVM==1)	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

//	GPIO_PinAFConfig(GPIOC,GPIO_Pin_6,GPIO_AF_USART6); 			//------------------------
//	GPIO_PinAFConfig(GPIOC,GPIO_Pin_7,GPIO_AF_USART6);

//   	/* Configure USART6 Rx (PC7) as input floating                             */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//  	/* Configure USART6 Tx (PC6) as alternate function push-pull                */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;

//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  USART_DeInit(USART6);
//#if(TRANSMISSION_ORIGINAL_DATA==0)		
//	USART_InitStructure.USART_BaudRate            = 4800;
//  USART_InitStructure.USART_Parity              = USART_Parity_Odd;	
//	USART_InitStructure.USART_WordLength     = USART_WordLength_9b;
//#else	
//	USART_InitStructure.USART_BaudRate            = 115200;
//  USART_InitStructure.USART_Parity              = USART_Parity_No;
//	USART_InitStructure.USART_WordLength     = USART_WordLength_8b;
//#endif
//  //USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
//  //USART_InitStructure.USART_Parity              = USART_Parity_No;
//  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
//  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//  USART_InitStructure.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
//  USART_Init(USART6, &USART_InitStructure);
//	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
//  USART_Cmd(USART6, ENABLE);
//  USART_ClearFlag(USART6,USART_FLAG_TXE);
////usart2
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

//	GPIO_PinAFConfig(GPIOA,GPIO_Pin_2,GPIO_AF_USART2); 			//------------------------
//	GPIO_PinAFConfig(GPIOA,GPIO_Pin_3,GPIO_AF_USART2);
//  /* Configure USART2 Rx (PA3) as input floating                             */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  /* Configure USART2 Tx (PA2) as alternate function push-pull                */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  USART_DeInit(USART2);
//#if(TRANSMISSION_ORIGINAL_DATA==1)
//	USART_InitStructure.USART_BaudRate       = 115200;
//  USART_InitStructure.USART_Parity         = USART_Parity_No;
//	USART_InitStructure.USART_WordLength     = USART_WordLength_8b;
//#else
//	USART_InitStructure.USART_BaudRate       = 4800;
//  USART_InitStructure.USART_Parity              = USART_Parity_Odd;	
//	USART_InitStructure.USART_WordLength     = USART_WordLength_9b;
//#endif	
//  //USART_InitStructure.USART_WordLength     = USART_WordLength_8b;
//  //USART_InitStructure.USART_Parity         = USART_Parity_No;
// 	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
// 	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// 	USART_InitStructure.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
// 	USART_Init(USART2, &USART_InitStructure);
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//  USART_Cmd(USART2, ENABLE);
//  USART_ClearFlag(USART2,USART_FLAG_TXE);
//#else
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_Pin_9,GPIO_AF_USART1); 			//------------------------
	GPIO_PinAFConfig(GPIOA,GPIO_Pin_10,GPIO_AF_USART1);

 	/* Configure USART1 Rx (PA10) as input floating                             */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate            = 4800;//115200;//38400;////////////////4800;//4800;////38400;//
  USART_InitStructure.USART_WordLength          = USART_WordLength_9b;//USART_WordLength_8b;//
  USART_InitStructure.USART_Parity              = USART_Parity_Odd;//;USART_Parity_No;//
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  USART_ClearFlag(USART1,USART_FLAG_TXE);
	
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
//	GPIO_PinAFConfig(GPIOC,GPIO_Pin_10,GPIO_AF_UART4); 			//------------------------
//	GPIO_PinAFConfig(GPIOC,GPIO_Pin_11,GPIO_AF_UART4);
 	/* Configure USART4 Rx (PC11) as input floating                             */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
//	/* Configure USART4 Tx (PC10) as alternate function push-pull */
//  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);

//  USART_DeInit(UART4);
//#if(TRANSMISSION_ORIGINAL_DATA==1)
//	USART_InitStructure.USART_BaudRate            = 57600;
//  USART_InitStructure.USART_Parity         = USART_Parity_No;
//	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
//#else
//	USART_InitStructure.USART_BaudRate            = 4800;
//  USART_InitStructure.USART_Parity              = USART_Parity_Odd;//USART_Parity_No;
//	USART_InitStructure.USART_WordLength          = USART_WordLength_9b;
//#endif 
////  USART_InitStructure.USART_Parity              = USART_Parity_No;
//  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
//  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//  USART_InitStructure.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;
//  USART_Init(UART4, &USART_InitStructure);
//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
//  USART_Cmd(UART4, ENABLE);
//  USART_ClearFlag(UART4,USART_FLAG_TXE);	
//#endif
}

//RTC配置与NVIC_init为后添加
void NVIC_init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	
/*	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;*/
	NVIC_Init(&NVIC_InitStructure);	 		
	
#if(USE_AFE4490_EVM==1)	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //0;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
      
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //0;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);		
#else
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //0;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);		
	
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //0;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);			
#endif
}

void WWDG_Configuration(void)
{
 	/* WWDG clock counter = (PCLK1/4096)/8 = 244 Hz (~4 ms)  */
 	WWDG_SetPrescaler(WWDG_Prescaler_8); 
 	/* Set Window value to 65 */
 	WWDG_SetWindowValue(65);
 	/* Enable WWDG and set counter value to 127, WWDG timeout = ~4 ms * 64 = 262 ms */
 	WWDG_Enable(127);   
 	/* Clear EWI flag */
 	WWDG_ClearFlag();
 	/* Enable EW interrupt */
 	WWDG_EnableIT();  
}

void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  /* Set counter reload value to 349 */
  //该参数允许取值范围为0 – 0x0FFF 
  IWDG_SetReload(3750);  //3s
	
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void SysTickConfig(void)
{
	if(SysTick_Config(SystemCoreClock/1000))
  { 	/* Capture error */     	
   	while(1)
		{
			;
		}
  }
}

void Spi_Init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
#if(SIMULATION_SPO2==0||SIMULATION_SPO2_TYPE==1)
//#if(SIMULATION_SPO2==0)
  SPI_InitTypeDef  SPI_InitStructure; 
  /* Enable the SPI clock */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
  /* Enable the GPIOA clock */ 
	//SPI1_CS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4); 	   
	
  // Connect PA5 to SPI1_SCK  
  GPIO_PinAFConfig1(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); 
  // Connect PA6 to SPI1_MISO  
  GPIO_PinAFConfig1(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); 
  // Connect PA7 to SPI1_MOSI  
  GPIO_PinAFConfig1(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); 
 
  /* Configure SPI1 pins as alternate function (No need to 
     configure PA4 since NSS will be managed by software) */         
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
 
  /* SPI configuration *****************************************/ 
 
  SPI_Cmd(SPI1, DISABLE); //先禁能,才能改变MODE模式
  SPI_InitStructure.SPI_Direction =  SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //SPI_FirstBit_LSB;//
  SPI_InitStructure.SPI_CRCPolynomial = 0; 
  SPI_Init(SPI1, &SPI_InitStructure); 
 
  SPI_Cmd(SPI1, ENABLE); 
#else
  /* Enable the GPIOA clock */ 
	//SPI1_CS
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4); 	   
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

void BSP_Init(void)
{
	RCC_Configuration();
	//TIM_Configuration(); 
  GPIO_Config();
 	Usart_Init();	//后面加串口
	SysTickConfig();
#if(WATCH_DOG==1)
	IWDG_Configuration();
#endif	
	//IWDG_Init();
	//WWDG_Configuration();
//UART3_Send_bit('b');
//SPO2 ------------------------------------------------------------------------- 
	Spi_Init();
#if(SIMULATION_SPO2_TYPE==1)
//	AFE4490_PowerDown(0);
//	AFE4490_Hardware_Reset();
	InitAfe4490();
#endif	
 	NVIC_init();
	EXTI_Configuration();
//----------------------------------------------------------------------------
}


