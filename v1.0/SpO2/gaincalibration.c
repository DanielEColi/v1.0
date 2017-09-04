#include "gaincalibration.h"
#include "afe4490.h"
#include "global.h"

/*********************************************************************************
*File: gain calibration 
*Description:  adjust gain to appropriate ����У׼�����ʵ�λ��
*Author:	Mike
*Others:  
**********************************************************************************/ 

/*
ILED_CURR_MAX = 3 mA
ILED_CURR_MIN = 1 mA
TARGET_THR = 50%
LOW_THR = 10%
HYS_PERCENT = 1%
HIGH_THR = 90%
BLOCK_SIZE = 16 samples
TIME_WAIT = 20 ms
TFREEZE = 1 sec

Derived Variables:
LOW_THR_HYS_LOW = LOW_THR �C HYS_PERCENT = 9%
LOW_THR_HYS_HIGH = LOW_THR + HYS_PERCENT = 11%
HIGH_THR_HYS_LOW = HIGH_THR �C HYS_PERCENT = 89%
HIGH_THR_HYS_HIGH = HIGH_THR + HYS_PERCENT = 91%
*/

void GetMeasDC(void)
{
}

void DeviceReadyData(void)
{
/*	
	do
	{				
		Wait for TFREEZE;				//17
		MEAS_DC =  peak value in a sample set of size=BLOCK_SIZE	//18      
	}while( IS (DIS_LOW_CHECK=1 and MEAS_DC<LOW_THR_HYS_LOW)OR (DIS_HIGH_CHECK=1 and MEAS_DC>HIGH_THR_HYS_HIGH)?)		//19
	if(IS MEAS_DC > HIGH_THR_HYS_HIGH)	//20
	{
		goto�ڶ���
	}
	else
	{
		if(IS MEAS_DC < LOW_THR_HYS_LOW)	//21
		{
			RF=1 MOhm; //22
			goto �ڶ���
		}
		else
		{
			goto��17��
		}
	}		*/		
}

void GainCalibration(void)
{
//reset device	
	AFE4490_Hardware_Reset();
	AFE4490_Enable(1);
	WriteCommand(0x00,0x08);		//Software reset
//withings settings
/*PRF=100Hz CF=5pF Duty_cycle=20% tx_ref=0.5V Led_range=50mA
	*/	
//����ʱ��400us 1600clk��LED��������ʱ80clk
	WriteCommand(0x01,30080);		//led2������ʼ
	WriteCommand(0x02,31680);		//led2��������
	WriteCommand(0x03,30000);		//led2 led�򿪣���ǰ�ڲ�����ʼ80��ʱ��
	WriteCommand(0x04,31700);		//led2 led�رգ��Ӻ��ڲ�������20��ʱ��
	WriteCommand(0x05,80);			//led2�����������ʼ����ʱled2Ӧ�ùر�
	WriteCommand(0x06,1680);		//led2�����������������ʱled2Ӧ�ùر�
	WriteCommand(0x07,10080);		//led1������ʼ	
	WriteCommand(0x08,11680);		//led1��������
	WriteCommand(0x09,10000);		//led1 led�򿪣���ǰ�ڲ�����ʼ80��ʱ��
	WriteCommand(0x0a,11700);		//led1 led�رգ��Ӻ��ڲ�������20��ʱ��
	WriteCommand(0x0b,20080);		//led1�����������ʼ����ʱ���ledӦ�ùر�
	WriteCommand(0x0c,21680);		//led1�����������������ʱ���ledӦ�ùر�
	WriteCommand(0x0d,80);			//LED2 ת����ʼ��
	WriteCommand(0x0e,1680);		//LED2 ת������
	WriteCommand(0x0f,10080);		//LED2������ ת����ʼ	
	WriteCommand(0x10,11680);		//LED2������ ת������
	WriteCommand(0x11,20080);		//LED1 ת����ʼ
	WriteCommand(0x12,21680);		//LED1 ת������
	WriteCommand(0x13,30080);		//LED1������ ת����ʼ
	WriteCommand(0x14,31680);		//LED1������ ת������
	WriteCommand(0x15,0);				//ADC Reset 0 ��ʼ
	WriteCommand(0x16,60);			//ADC Reset 0 ����
	WriteCommand(0x17,10000);		//ADC Reset 1 ��ʼ	
	WriteCommand(0x18,10060);		//ADC Reset 1 ����
	WriteCommand(0x19,20000);		//ADC Reset 2 ��ʼ	
	WriteCommand(0x1a,20060);		//ADC Reset 2 ����	
	WriteCommand(0x1b,30000);		//ADC Reset 3 ��ʼ
	WriteCommand(0x1c,30060);		//ADC Reset 3 ����	
	WriteCommand(0x1d,39999);		
	
	WriteCommand(0x1e,0x107);	//Control Register 1 8hz aver
//1	
	WriteCommand(0x20,0x008006);
	WriteCommand(0x21,0x000006); 	
	WriteCommand(0x23,0x20000);		//Control Register 2 TX_REF 0.5V	
//2	
	WriteCommand(0x22,0x010000|((256*1/50-1)<<8)|((256*1/50-1)<<8));//50mA 	
//3	
	SystickDelay1ms(20);
//4	
/*	MEAS_DC =  peak value in a sample set of size=BLOCK_SIZE

//5
	if(MEAS_DC>TARGET_THR)
	{
		if(RF > 10KOhm)			//12		
		{
			Decrement Rf;			//13
		}
		else
		{
			if(MEAS_DC>HIGH_THR_HYS_LOW)	//14
			{
				DISABLE_HIGH_CHECK=1;				//15
			}
			Wait for TIME_WAIT;			//16
			do
			{				
				Wait for TFREEZE;				//17
				MEAS_DC =  peak value in a sample set of size=BLOCK_SIZE	//18      
			}while( IS (DIS_LOW_CHECK=1 and MEAS_DC<LOW_THR_HYS_LOW)OR (DIS_HIGH_CHECK=1 and MEAS_DC>HIGH_THR_HYS_HIGH)?)		//19
			if(IS MEAS_DC > HIGH_THR_HYS_HIGH)	//20
			{
				goto�ڶ���
			}
			else
			{
				if(IS MEAS_DC < LOW_THR_HYS_LOW)	//21
				{
				 RF=1 MOhm; //22
				 goto �ڶ���
				}
				else
				{
					DeviceReadyData();//17
				}
			}			
		}
	}
	else
	{
//6		
		ILED = MIN(TARGET_THR*ILED_CURR_MIN/MEAS_DC, ILED_CURR_MAX)
    MEAS_DC_EST=MEAS_DC*ILED/ILED_CURR_MIN
//
		if(IS MEAS_DC_EST< LOW_THR_HYS_HIGH) //7
		{
//8			
			STAGE2_EN = ��1�� 
			STAGE2_GAIN = MIN(The owest gain when multiplied by MEAS_DC_EST gives valuereater than TARGET_THR, 4)
//9
			MEAS_DC_EST = MEAS_DC_EST*STAGE2_GAIN			
			if(IS MEAS_DC_EST< LOW_THR_HYS_HIGH)	//10
			{
				DISABLE_LOW_CHECK=1	//11
			}
		}
		Wait for TIME_WAIT;			//16
		DeviceReadyData();			//17
	}*/
}

