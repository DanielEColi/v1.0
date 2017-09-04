#include "processpacket.h"
#include "global.h"
#include "ledcurrent.h"
#include "bciprotocol.h"

u8 g_PatientType=0;		//0:����  1:��ͯ  2:������
u8 g_AverCycle=3;			//0:1 beat 1:4 beat 2:8 beat 3:16 beat

void FillRspPacket(u8 cmd)
{
		u8 validcmd=1,i;
		u8 buff[8];
		switch(cmd)
		{
		case 'E':		//�鿴��ǰ��Ϣ �����汾�ţ�����ģʽ���������õ�
				break;
		case '1':		//��������		1 beat					
				g_AverCycle=0;
				break;
		case '2':		//��������		4 beat		
				g_AverCycle=1;
				break;
		case '3':		//��������		8 beat		
				g_AverCycle=2;
				break;
		case '4':		//��������		16 beat		
				g_AverCycle=3;
				break;
		case 'N':		//������ģʽ
				g_PatientType=2;		
				break;
		case 'P':		//��ͯģʽ
				g_PatientType=1;
				break;
		case 'M':		//����ģʽ
				g_PatientType=0;
				break;
		default:
				validcmd=0;
				break;
		}	
#if(LED_CURRENT_ADJUST==2)
		switch(g_PatientType)
		{
		case 0:
			g_LedCurrentLevel[0]=4;	//4,2,0  50ma 30ma 10ma
			g_LedCurrentLevel[1]=4;	
			SetGain(0,DEAULT_LED1_GAIN_LEVEL-g_PatientType);
			SetGain(1,DEAULT_LED1_GAIN_LEVEL-g_PatientType);
			break;
		case 1:
			g_LedCurrentLevel[0]=2;	//4,2,0  50ma 30ma 10ma
			g_LedCurrentLevel[1]=2;	
			SetGain(0,2);
			SetGain(1,2);
			break;
		case 2:
			g_LedCurrentLevel[0]=0;	//4,2,0  50ma 30ma 10ma
			g_LedCurrentLevel[1]=0;			
			SetGain(0,DEAULT_LED1_GAIN_LEVEL-g_PatientType);
			SetGain(1,DEAULT_LED1_GAIN_LEVEL-g_PatientType);
			break;
		default:
			break;
		}
		SetLedCurrent(0);
		SetLedCurrent(1);
#endif
		if(validcmd)
		{
			buff[0]=0x80;
			buff[1]=0xff;
			buff[2]=(g_PatientType|(g_AverCycle<<2));
			buff[3]=(CODE_VERSION/100);					//�汾�Ÿ�λ
			buff[4]=(CODE_VERSION%100);					//�汾��С����λ
			buff[5]=0;					//����λ
			for(i=0;i<6;i++)
			{
				g_BciBuffer[g_BciBufferWrIdx++]=buff[i];
				g_BciBufferWrIdx&=(BCI_BUFFER_SIZE-1);
			}
		}
}

void AnalyseUart1Packet(void)
{
	while(g_Rx0RdIdx!=g_Rx0WrIdx)
	{
//		FillRspPacket(g_Rx0Buffer[g_Rx0RdIdx]);
		g_Rx0RdIdx++;
		g_Rx0RdIdx&=(RX0_BUFFER_SIZE-1);			
	}		
}

