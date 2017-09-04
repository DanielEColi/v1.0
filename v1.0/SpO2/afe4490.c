#include "afe4490.h"
#include "global.h"
#include "bciprotocol.h"
#include "spo2tools.h"
#include "algorithm.h"
#include "plethwave.h"
#include "piprocess.h"
#include "ledcurrent.h"
#include "ambientcancellationdac.h"
#include "ledcurrent.h"
#include "CalibrationRoutine.h"
bool IsChangeLight=false;
bool IsChangeMin=false;
uint8_t thrflag2=0;
u32 AFE44xx_Current_Register_Settings[64];

s32 Afe4490DataBuf[6];		//Ô­Ê¼²ÉÑùÊý¾Ý£¬6¸ö²»Í¬ÐÅºÅ

u8 SPI1_SendByte(u8 byte)
{
 	u8 data=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
	{
		;
	}
  SPI_I2S_SendData(SPI1,byte);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET)
	{
		;
	}
  data=SPI_I2S_ReceiveData(SPI1);
	//DelayUS(1);	
	return data;
}	

void EnableWrite(u8 flag)
{
	u8 i=0;
	u8 *pDat;
	u32 data;
	if(g_AFE4490_DIAG_EN)
	{
		data=(flag?0x04:0x05);
	}
	else
	{
		data=(flag?0x00:0x01);
	}
	pDat=((u8 *)&data);

//adr:0x00 data:0x00 at first
	AFE4490_CS_LOW;
	for(i=0;i<4;i++)
	{
		SPI1_SendByte((*(pDat+(3-i))));		//MSB First 	
	}	
	AFE4490_CS_HIGH;
}

void Enable_Diagnostic(u8 flag)
{
	u8 i=0;
	u8 *pDat;
	u32 data;
	EnableWrite(1);
	if(flag)
	{
		data=0x04;	
		g_AFE4490_DIAG_EN=1;
	}
	else
	{
		data=0x00;	
		g_AFE4490_DIAG_EN=0;
	}
	pDat=((u8 *)&data);

//adr:0x00 data:0x00 at first
	AFE4490_CS_LOW;
	for(i=0;i<4;i++)
	{
		SPI1_SendByte((*(pDat+(3-i))));		//MSB First 	
	}	
	AFE4490_CS_HIGH;	
}

void Enable_Stage2Gain(u8 enable,u8 gain)
{
	if(enable)
	{
		WriteCommand(0x21,0x4000|(gain<<8));
	}
	else
	{
		WriteCommand(0x21,0x00);		
	}
}

void WriteCommand(u8 cmd,u32 data)
{
	u8 i=0;
	u8 *pDat;
	data=((cmd<<24)|(data&0x00ffffff));
	pDat=((u8 *)&data);

	EnableWrite(1);
	
//adr:cmd data: at first
	AFE4490_CS_LOW;
	for(i=0;i<4;i++)
	{
		SPI1_SendByte((*(pDat+(3-i))));		//MSB First 	
	}	
	AFE4490_CS_HIGH;
}

u32 ReadCommand(u8 cmd)
{
	u8 i=0;
	u32 result=0;
	
	EnableWrite(0);

	AFE4490_CS_LOW;
	SPI1_SendByte(cmd);
	for(i=0;i<3;i++)
	{
		result|=(SPI1_SendByte(0x00)<<(8*(2-i)));		//MSB First 	
	}	
	AFE4490_CS_HIGH;	
	
	return result;
}

//AFE_PDN
void AFE4490_PowerDown(u8 flag)
{
	if(flag==0)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	SystickDelay1ms(200);
}

//AFE_Reset
void AFE4490_Hardware_Reset(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	SystickDelay1ms(20);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}

//Enable AFE4490£¬AFE_PDN pin
void AFE4490_Enable(u8 flag)
{
	if(flag)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);	
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);	
	}
	SystickDelay1ms(5);
}

//input current,unit mA
void SetTxReferenceVoltage(u8 Led1Current,u8 Led2Current)
{
	u8 maxcurrent=((Led1Current>Led2Current)?Led1Current:Led2Current);
	if(maxcurrent>75)
	{
		WriteCommand(0x23,0x40000);		//Control Register 2 TX_REF 1.0V		
	}
	else if(maxcurrent>50)
	{
		WriteCommand(0x23,0x00000);		//Control Register 2 D17:18 Tx reference voltage 0.75V
	}
	else	
	{
		WriteCommand(0x23,0x20000);		//Control Register 2 D17:18 Tx reference voltage 0.5V
	}
}

void InitAfe4490(void)
{
	AFE4490_Enable(1);
//8000:500hz 40000:100hz		
	WriteCommand(0x00,0x08);		//Software reset
	SystickDelay1ms(200);
	WriteCommand(0x00,0x04);
//example	
//test 100hz	
/*LED2STC R/W 01 1 0 0 0 0 0 0 0 0 LED2STC[15:0]
LED2ENDC R/W 02 2 0 0 0 0 0 0 0 0 LED2ENDC[15:0]
LED2LEDSTC R/W 03 3 0 0 0 0 0 0 0 0 LED2LEDSTC[15:0]
LED2LEDENDC R/W 04 4 0 0 0 0 0 0 0 0 LED2LEDENDC[15:0]
ALED2STC R/W 05 5 0 0 0 0 0 0 0 0 ALED2STC[15:0]
ALED2ENDC R/W 06 6 0 0 0 0 0 0 0 0 ALED2ENDC[15:0]
LED1STC R/W 07 7 0 0 0 0 0 0 0 0 LED1STC[15:0]
LED1ENDC R/W 08 8 0 0 0 0 0 0 0 0 LED1ENDC[15:0]
LED1LEDSTC R/W 09 9 0 0 0 0 0 0 0 0 LED1LEDSTC[15:0]
LED1LEDENDC R/W 0A 10 0 0 0 0 0 0 0 0 LED1LEDENDC[15:0]
ALED1STC R/W 0B 11 0 0 0 0 0 0 0 0 ALED1STC[15:0]
ALED1ENDC R/W 0C 12 0 0 0 0 0 0 0 0 ALED1ENDC[15:0]
LED2CONVST R/W 0D 13 0 0 0 0 0 0 0 0 LED2CONVST[15:0]
LED2CONVEND R/W 0E 14 0 0 0 0 0 0 0 0 LED2CONVEND[15:0]
ALED2CONVST R/W 0F 15 0 0 0 0 0 0 0 0 ALED2CONVST[15:0]
ALED2CONVEND R/W 10 16 0 0 0 0 0 0 0 0 ALED2CONVEND[15:0]
LED1CONVST R/W 11 17 0 0 0 0 0 0 0 0 LED1CONVST[15:0]
LED1CONVEND R/W 12 18 0 0 0 0 0 0 0 0 LED1CONVEND[15:0]
ALED1CONVST R/W 13 19 0 0 0 0 0 0 0 0 ALED1CONVST[15:0]
ALED1CONVEND R/W 14 20 0 0 0 0 0 0 0 0 ALED1CONVEND[15:0]
ADCRSTSTCT0 R/W 15 21 0 0 0 0 0 0 0 0 ADCRSTCT0[15:0]
ADCRSTENDCT0 R/W 16 22 0 0 0 0 0 0 0 0 ADCRENDCT0[15:0]
ADCRSTSTCT1 R/W 17 23 0 0 0 0 0 0 0 0 ADCRSTCT1[15:0]
ADCRSTENDCT1 R/W 18 24 0 0 0 0 0 0 0 0 ADCRENDCT1[15:0]
ADCRSTSTCT2 R/W 19 25 0 0 0 0 0 0 0 0 ADCRSTCT2[15:0]
ADCRSTENDCT2 R/W 1A 26 0 0 0 0 0 0 0 0 ADCRENDCT2[15:0]
ADCRSTSTCT3 R/W 1B 27 0 0 0 0 0 0 0 0 ADCRSTCT3[15:0]
ADCRSTENDCT3 R/W 1C 28 0 0 0 0 0 0 0 0 ADCRENDCT3[15:0]
PRPCOUNT R/W 1D 29 0 0 0 0 0 0 0 0 PRPCT[15:0]*/	
//²ÉÑùÊ±¼ä400us 1600clk£¬LEDµãÁÁºóÑÓÊ±80clk
	WriteCommand(0x01,30080);		//led2²ÉÑù¿ªÊ¼
	WriteCommand(0x02,31680);		//led2²ÉÑù½áÊø
	WriteCommand(0x03,30000);		//led2 led´ò¿ª£¬ÌáÇ°ÔÚ²ÉÑù¿ªÊ¼80¸öÊ±ÖÓ
	WriteCommand(0x04,31700);		//led2 led¹Ø±Õ£¬ÑÓºóÔÚ²ÉÑù½áÊø20¸öÊ±ÖÓ
	WriteCommand(0x05,80);			//led2»·¾³¹â²ÉÑù¿ªÊ¼£¬´ËÊ±led2Ó¦¸Ã¹Ø±Õ
	WriteCommand(0x06,1680);		//led2»·¾³¹â²ÉÑù½áÊø£¬´ËÊ±led2Ó¦¸Ã¹Ø±Õ
	WriteCommand(0x07,10080);		//led1²ÉÑù¿ªÊ¼	
	WriteCommand(0x08,11680);		//led1²ÉÑù½áÊø
	WriteCommand(0x09,10000);		//led1 led´ò¿ª£¬ÌáÇ°ÔÚ²ÉÑù¿ªÊ¼80¸öÊ±ÖÓ
	WriteCommand(0x0a,11700);		//led1 led¹Ø±Õ£¬ÑÓºóÔÚ²ÉÑù½áÊø20¸öÊ±ÖÓ
	WriteCommand(0x0b,20080);		//led1»·¾³¹â²ÉÑù¿ªÊ¼£¬´ËÊ±ºì¹âledÓ¦¸Ã¹Ø±Õ
	WriteCommand(0x0c,21680);		//led1»·¾³¹â²ÉÑù½áÊø£¬´ËÊ±ºì¹âledÓ¦¸Ã¹Ø±Õ
	WriteCommand(0x0d,80);			//LED2 ×ª»»¿ªÊ¼£¬
	WriteCommand(0x0e,1680);		//LED2 ×ª»»½áÊø
	WriteCommand(0x0f,10080);		//LED2»·¾³¹â ×ª»»¿ªÊ¼	
	WriteCommand(0x10,11680);		//LED2»·¾³¹â ×ª»»½áÊø
	WriteCommand(0x11,20080);		//LED1 ×ª»»¿ªÊ¼
	WriteCommand(0x12,21680);		//LED1 ×ª»»½áÊø
	WriteCommand(0x13,30080);		//LED1»·¾³¹â ×ª»»¿ªÊ¼
	WriteCommand(0x14,31680);		//LED1»·¾³¹â ×ª»»½áÊø
	WriteCommand(0x15,0);				//ADC Reset 0 ¿ªÊ¼
	WriteCommand(0x16,60);			//ADC Reset 0 ½áÊø
	WriteCommand(0x17,10000);		//ADC Reset 1 ¿ªÊ¼	
	WriteCommand(0x18,10060);		//ADC Reset 1 ½áÊø
	WriteCommand(0x19,20000);		//ADC Reset 2 ¿ªÊ¼	
	WriteCommand(0x1a,20060);		//ADC Reset 2 ½áÊø	
	WriteCommand(0x1b,30000);		//ADC Reset 3 ¿ªÊ¼
	WriteCommand(0x1c,30060);		//ADC Reset 3 ½áÊø	
	WriteCommand(0x1d,39999);	
	
	/*WriteCommand(0x01,30000);					//led2²ÉÑù¿ªÊ¼
	WriteCommand(0x02,39998);					//led2²ÉÑù½áÊø
	WriteCommand(0x03,30000);					//led2 led´ò¿ª£¬ÌáÇ°ÔÚ²ÉÑù¿ªÊ¼5¸öÊ±ÖÓ
	WriteCommand(0x04,39999);					//led2 led¹Ø±Õ£¬ÑÓºóÔÚ²ÉÑù½áÊø5¸öÊ±ÖÓ
	WriteCommand(0x05,0);							//led2»·¾³¹â²ÉÑù¿ªÊ¼£¬´ËÊ±led2Ó¦¸Ã¹Ø±Õ
	WriteCommand(0x06,9998);					//led2»·¾³¹â²ÉÑù½áÊø£¬´ËÊ±led2Ó¦¸Ã¹Ø±Õ
	WriteCommand(0x07,10000);					//led1²ÉÑù¿ªÊ¼
	WriteCommand(0x08,19998);					//led1²ÉÑù½áÊø
	WriteCommand(0x09,10000);					//led1 led´ò¿ª£¬ÌáÇ°ÔÚ²ÉÑù¿ªÊ¼5¸öÊ±ÖÓ
	WriteCommand(0x0a,19999);					//led1 led¹Ø±Õ£¬ÑÓºóÔÚ²ÉÑù½áÊø5¸öÊ±ÖÓ
	WriteCommand(0x0b,20000);					//led1»·¾³¹â²ÉÑù¿ªÊ¼£¬´ËÊ±ºì¹âledÓ¦¸Ã¹Ø±Õ
	WriteCommand(0x0c,29998);					//led1»·¾³¹â²ÉÑù½áÊø£¬´ËÊ±ºì¹âledÓ¦¸Ã¹Ø±Õ
	WriteCommand(0x0d,2);					//LED2 ×ª»»¿ªÊ¼£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±10us£¬40¸öclk
	WriteCommand(0x0e,9999);					//LED2 ×ª»»½áÊø£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±400us£¬1600¸öclk £¬ÑÓºóÑÓ20¸öclk
	WriteCommand(0x0f,10002);						//LED2»·¾³¹â ×ª»»¿ªÊ¼£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±10us£¬40¸öclk
	WriteCommand(0x10,19999);					//LED2»·¾³¹â ×ª»»½áÊø£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±400us£¬1600¸öclk £¬ÑÓºóÑÓ20¸öclk
	WriteCommand(0x11,20002);					//LED1 ×ª»»¿ªÊ¼£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±10us£¬40¸öclk
	WriteCommand(0x12,29999);					//LED1 ×ª»»½áÊø£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±400us£¬1600¸öclk £¬ÑÓºóÑÓ20¸öclk
	WriteCommand(0x13,30002);					//LED1»·¾³¹â ×ª»»¿ªÊ¼£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±10us£¬40¸öclk
	WriteCommand(0x14,39999);					//LED1»·¾³¹â ×ª»»½áÊø£¬½Ï²ÉÑù¿ªÊ¼ÑÓÊ±400us£¬1600¸öclk £¬ÑÓºóÑÓ20¸öclk
	WriteCommand(0x15,0);							//ADC Reset 0 ¿ªÊ¼
	WriteCommand(0x16,0);							//ADC Reset 0 ½áÊø
	WriteCommand(0x17,10000);					//ADC Reset 1 ¿ªÊ¼	
	WriteCommand(0x18,10000);					//ADC Reset 1 ½áÊø
	WriteCommand(0x19,20000);					//ADC Reset 2 ¿ªÊ¼	
	WriteCommand(0x1a,20000);					//ADC Reset 2 ½áÊø	
	WriteCommand(0x1b,30000);					//ADC Reset 3 ¿ªÊ¼	
	WriteCommand(0x1c,30000);					//ADC Reset 3 ½áÊø	
	WriteCommand(0x1d,39999);*/
	
	//WriteCommand(0x1f,0x000); //reserved
	
	WriteCommand(0x1e,0x107);	//Control Register 1 8hz aver
	//WriteCommand(0x20,0x0026);		//Transimpedance Amplifier Gain Setting Register/  30pf 1M
	//WriteCommand(0x21,0x0026);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register
	

	//WriteCommand(0x20,0x0081);		//Transimpedance Amplifier Gain Setting Register/		//155pf,250k o		R*C<Sample Time/10  155*10(-12)*250*10(-3)=39.525us  sampletime=500us
	//WriteCommand(0x21,0x0081);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register
	
	SetGain(0,4);//³ÉÈËÔöÒæ3
	SetGain(1,4);
//	WriteCommand(0x20,0x00fb);		//Transimpedance Amplifier Gain Setting Register/		//155pf,250k o		R*C<Sample Time/10  155*10(-12)*250*10(-3)=39.525us  sampletime=500us
//	WriteCommand(0x21,0x00fb);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register

	//WriteCommand(0x20,0x8081);		//Transimpedance Amplifier Gain Setting Register/		
	//WriteCommand(0x21,0x0060);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register	

	//WriteCommand(0x20,0x05);		//Transimpedance Amplifier Gain Setting Register/ 10pf 250k  
	//WriteCommand(0x21,0x05);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register 10pf 250k  

	//WriteCommand(0x20,0x45);		//Transimpedance Amplifier Gain Setting Register/		//50pf,10k o
	//WriteCommand(0x21,0x45);		//Transimpedance Amplifier and Ambient Cancellation Stage Gain Register //50pf,10k o

	//WriteCommand(0x21,0x50000);	

	//WriteCommand(0x23,0x00000);		//Control Register 2 D17:18 Tx reference voltage 0.75V
	WriteCommand(0x23,0x40000);		//Control Register 2 TX_REF 1.0V		50mA
	
	//WriteCommand(0x23,0x20000);		//Control Register 2 D17:18 Tx reference voltage 0.5V
	
	WriteCommand(0x24,0x00);	//reserved
	WriteCommand(0x25,0x00);	//reserved
	WriteCommand(0x26,0x00);	//reserved
	WriteCommand(0x27,0x00);	//reserved
	WriteCommand(0x28,0x00);	//reserved
	WriteCommand(0x22,0x20000);
	SetLedCurrent(0,255);
	SetLedCurrent(1,255);
	
	//WriteCommand(0x29,0x00);	//Alarm Register
	WriteCommand(0x29,0x80);	//Alarm Register
	/*WriteCommand(0x2a,0x00);	//LED2 Digital Sample Value Register
	WriteCommand(0x2b,0x00);	//Ambient LED2 Digital Sample Value Register
	WriteCommand(0x2c,0x00);	//LED1 Digital Sample Value Register
	WriteCommand(0x2d,0x00);	//Ambient LED1 Digital Sample Value Register
	WriteCommand(0x2e,0x00);	//LED2-Ambient LED2 Digital Sample Value Register
	WriteCommand(0x2f,0x00);	//LED1-Ambient LED1 Digital Sample Value Register*/
	WriteCommand(0x30,0x00);	//Diagnostics Flag Register
	
	//Enable_Stage2Gain(1,4);
	
	//WriteCommand(0x00,0x05);
	WriteCommand(0x00,0x01);
	
	//Enable_Diagnostic(1);					
	
	AFE4490_Read_All_Register();
}

void AFE4490_Read_All_Register(void)
{
	u8 i;
	for(i=0;i<0x30;i++)
	{
		AFE44xx_Current_Register_Settings[i]=ReadCommand(i);
	}
	//SystickDelay1ms(20);	
}

u8 AFE4490_Check_SensorNotConnect(void)
{
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5))
	{
		return 2;
	}	
	return 0;
}

#define SENSOR_OFF_NUMBER	5

s32 SampleData[2][SENSOR_OFF_NUMBER];		
u8 SampleDataNum=0;		
u8 SampleDataIdx=0;

void AddSampleData(void)
{
	SampleData[0][SampleDataIdx]=Afe4490DataBuf[0];
	SampleData[1][SampleDataIdx]=Afe4490DataBuf[2];
	SampleDataIdx=(SampleDataIdx+1)%SENSOR_OFF_NUMBER;
	if(SampleDataNum<SENSOR_OFF_NUMBER)
	{
		SampleDataNum++;
	}
}

u8 CheckFingerOff(void)
{
	u32 sum[2]={0,0};
	u8 i;
	if(SampleDataNum<SENSOR_OFF_NUMBER)
	{
		return g_SenserOff&0x01;
	}
	for(i=0;i<SampleDataNum;i++)
	{
		sum[0]+=SampleData[0][i];
		sum[1]+=SampleData[1][i];
	}
	sum[0]=sum[0]/SampleDataNum;
	sum[1]=sum[1]/SampleDataNum;
	if(sum[0]>0x1d0000||sum[1]>0x1d0000)
	{
		return 0x01;
	}
	return 0x00;
}

//½ö¼ì²âºìÍâ¹âÊÇ²»ÊÇ³¬¹ýÌ½Í·ÍÑÂäµÄ·§Öµ
u8 AFE4490_Check_Sensor(void)
{	
#if(LED_CURRENT_ADJUST==1)		
	s32 adcvalue[2];
#endif	
	u8 currentsensoroff=0;

	AddSampleData();
	
	currentsensoroff|=CheckFingerOff();
	currentsensoroff|=AFE4490_Check_SensorNotConnect();	
	
	if(g_SenserOff!=currentsensoroff)
	{						
		g_SenserOff=currentsensoroff;
	//	ClearProcessOxiData(SIMULATION_OXI_IDX);		
	}
	
	//calibrationLED1(Afe4490DataBuf[2]);
	//calibrationLED2(Afe4490DataBuf[0]);

	if(g_SenserOff==0)
	{
//		AddOriginalData(Afe4490DataBuf[AFE4490_IR_INDEX],IR_IDX);
	//	AddOriginalData(Afe4490DataBuf[AFE4490_R_INDEX],R_IDX);
	}
	else
	{
//		AddSimulationWaveData(0,0);
//		AddSimulationWaveData(1,0);
	}
	
#if(AMBIENT_CANCELLATION_DAC==1)		
	AmbientCancellationDac(Afe4490DataBuf[1],Afe4490DataBuf[3]);
#endif		
		
#if(LED_CURRENT_ADJUST==1)		
	adcvalue[0]=Afe4490DataBuf[2];
	adcvalue[1]=Afe4490DataBuf[0];
	LedCurrentControl(adcvalue);
#endif		
	
	return g_SenserOff;
	//return 0;
}
//Êý¾Ý²ÉÑù
void AFE4490_Sampling(void)
{
	static uint8_t flag=0;
	uint8_t i;
	static uint8_t current=255;
	static uint8_t gain=5;
        static uint8_t times100=0;
        static bool ischangeflag=true;
        times100++;
        if(times100==20)times100=0;
        if(SensorOff==1)
        {
          if(times100<2)
          {
            ischangeflag=false;
            SetLedCurrent(1,50);
            //SetLedCurrent(1,2);
          }
          else
          {
            ischangeflag=true;
            SetLedCurrent(1,0);
            //SetLedCurrent(1,255);
          }
        }
//	GPIO_SetBits(GPIOE,GPIO_Pin_2); 
//  Afe4490DataBuf[0]=(ReadCommand(0x2A)/*&0x3fffff*/);  //read RED Data
//  Afe4490DataBuf[1]=(ReadCommand(0x2B)/*&0x3fffff*/);  //read Ambient data
//  Afe4490DataBuf[2]=(ReadCommand(0x2C)/*&0x3fffff*/);  //read IR Data
//  Afe4490DataBuf[3]=(ReadCommand(0x2D)/*&0x3fffff*/);  //read Ambient Data
  Afe4490DataBuf[4]=(ReadCommand(0x2E)/*&0x3fffff*/);  //read RED - Ambient Data
  Afe4490DataBuf[5]=(ReadCommand(0x2F)/*&0x3fffff*/);  //read IR - Ambient Data	
   IsChangeLight=false;
   IsChangeMin=false;
	if(Afe4490DataBuf[5]<300000&&gain<=1)
	{
		current=255;
		gain=5;
                thrflag2=0;
                
                IsChangeLight=true;
	}
	else if((Afe4490DataBuf[5]>=1000000&&thrflag2==0)||(Afe4490DataBuf[5]>=1800000&&thrflag2==1))
	{
		IsChangeLight=true;
                //gain--;
                current=current-40;
		if(current<175)
		{
                  gain--;
                  //gain=1;
                  current=255;//-(123-current);
                  //current=current-40;
                  if(gain<=1)//?ù?Yì?í·×?éíµ?í?1a?èDT????ò?·??§
                  {
                    gain=1;
                    thrflag2=0;
                    //current=130;
                    IsChangeMin=true;
                    //IsChangeLight=true;//????????????,?????
                  }
		}
	}
        else if(Afe4490DataBuf[5]<1000000&&Afe4490DataBuf[5]>=300000)
        {
          //thrflag2=1;
        }
//	if(current<=130)
//	{
//		current=130;
//	}
//        IsChangeLight=false;
//        IsChangeMin=false;
	//current=175;
        
	//gain=4;
        SetLedCurrent(0,current);
        if(SensorOff==0)
        {
          
          SetLedCurrent(1,current);
 
         
        }
				SetGain(0,gain);
         SetGain(1,gain);
		
//AFE4490_Read_All_Register();

	//AFE4490_Check_Sensor();

}




