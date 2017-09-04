/**
  ******************************************************************************
  * @file    SysTick/SysTick_Example/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "global.h"
#include "bsp.h"
#include "oxisample.h"
#include "bciprotocol.h"
#include "afe4490.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SysTick_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

__asm void wait(void)
 {
       BX LR
 }
 
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	
	wait();
	
 /* while (1)
  {
  }*/
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	if(g_delay_tick)
	{
		g_delay_tick--;
	}
}

void TIM1_CC_IRQHandler(void)
{
	vu32 IC2Value = 0;
	vu32 Frequency=0;
	if(TIM_GetITStatus(TIM1,TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
#if(DIGITAL_SPO2==1)
		IC2Value=TIM_GetCapture2(TIM1); 
		if(g_CaptureIdx>=MAX_CAPTURE_TIMES)
   	{
			g_CaptureIdx=0;
   	}
		if(g_CaptureCount<MAX_CAPTURE_TIMES)
		{
			g_CaptureCount++;
		}		
		else
		{
			TIM_ITConfig(TIM1,TIM_IT_CC2,DISABLE);
			return;
		}
		Frequency=12000000/(IC2Value+1);
		
/*			if(Frequency>MAX_FREQUENCY)
				{
						Frequency=MAX_FREQUENCY;	
				}*/
/*		else if(Frequency<MIN_FREQUENCY)
		{
			Frequency=MIN_FREQUENCY;
		}	*/
		g_CaptureBuffer[g_CaptureIdx]=Frequency;    
		g_CaptureIdx++;	 
#endif				
	}
}

void TIM2_IRQHandler(void)
{
 	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)       	//判断中断来源
  {	
   	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);            //清除中断标志
//		g_T1Flag++;
  }  
}

void TIM3_IRQHandler(void)
{
	static vu16 fastcount=0,slowcount=0;
 	if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)       	//判断中断来源
  {	
   	TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);            //清除中断标志
#if(DIGITAL_SPO2==1)
//	if(g_HaveSignal)
/*			{
				FastSignalSample(fastcount++);
				if(fastcount>=20)
				{
					fastcount=0;
				}
			}*/
/*			else
			{
				SlowSignalSample(slowcount++);
				if(slowcount>=100)
				{
					slowcount=0;
				}
			}*/
#endif			
  	}  
}

void TIM4_IRQHandler(void)
{
 	if(TIM_GetITStatus(TIM4,TIM_IT_CC1)!=RESET)       	//判断中断来源
 	{	
   	TIM_ClearITPendingBit(TIM4,TIM_IT_CC1);            //清除中断标志
	} 	  
}

void TIM5_IRQHandler(void)
{	
 	if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)       	//判断中断来源
 	{	
   	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);            //清除中断标志
//		WorkLed();
#if(DIGITAL_SPO2==1)
		FastSignalSample(g_100us_times++);
#endif		
	} 	  
}

void USART1_IRQHandler(void)
{
// 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
// 	{
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
////		g_Rx0Buffer[g_Rx0WrIdx++]=USART_ReceiveData(USART1);
//		g_Rx0WrIdx&=(RX0_BUFFER_SIZE-1);		
//  }
//  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
//  {
//		USART_ClearITPendingBit(USART1,USART_IT_TXE);
////#if(USE_AFE4490_EVM==0)	
////		if(g_BciBufferRdIdx!=g_BciBufferWrIdx)
////		{
////			USART_SendData(USART1,g_BciBuffer[g_BciBufferRdIdx++]);
////			g_BciBufferRdIdx&=(BCI_BUFFER_SIZE-1);			
////		}
////		else
////#endif			
////		{
////			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
////		}		
// 	}	 
	bci_send2();
}

void USART2_IRQHandler(void)
{
//	static vu8 data=0;

}

void USART3_IRQHandler(void)
{
 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
 	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
  }
  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
  {
		USART_ClearITPendingBit(USART3,USART_IT_TXE);
	}
}

void UART4_IRQHandler(void)
{

}

void UART5_IRQHandler(void)
{
 	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
 	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
  }
  if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
  {
		USART_ClearITPendingBit(UART5,USART_IT_TXE);
  }	 
}

void USART6_IRQHandler(void)
{

}

void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3)!= RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3);
#if(USE_AFE4490_EVM==0)	
		g_100us_times=0;
		
		dataready++;
//		WorkLed();
#endif
	}
}

void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4)!= RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4);
#if(USE_AFE4490_EVM==1)	
		g_100us_times=0;
		AFE4490_Sampling();
//		WorkLed();
#endif
	}
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
