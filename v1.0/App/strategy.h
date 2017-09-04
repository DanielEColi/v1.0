#ifndef __STRATEGY_H__
#define __STRATEGY_H__
#include "fft.h"

#define MYORDER 101
#define LP    111
#define LP2  41//41                         //121--25~95
#define FFTLEN 256

typedef struct {
    float x;  /* state */
    float A;  /* x(n)=A*x(n-1)+u(n),u(n)~N(0,q) */
    float H;  /* z(n)=H*x(n)+w(n),w(n)~N(0,r)   */
    float q;  /* process(predict) noise convariance */
    float r;  /* measure noise convariance */
    float p;  /* estimated error convariance */
    float gain;
} kalman1_state;
extern kalman1_state SpO2state;
extern uint8_t thrflag;
extern complex fftbuff[FFTLEN];
extern float Amfft[FFTLEN];
extern uint16_t MaxwindowAmp;

extern float Covxy;
extern bool isFake;
extern bool isFake2;
extern float Bx[MYORDER];
extern uint32_t A[MYORDER];
extern float PerI;
extern int32_t Output;
extern int32_t Output2;
extern int32_t Output2old;
extern int32_t NorOutput;
extern int32_t NorOutput2;
extern uint8_t BmpSound;
extern uint8_t bmpsound[3];
extern uint8_t bmpsoundflag;
extern float Rvalue;
extern float Rvalue2;
extern float Rvalue3;
extern float ErrorR;
extern float ErrorR1;
extern float SpO2;
extern float SpO2h;
extern uint16_t Bmp;
extern float Pulse;
//extern int32_t Rbuff[400];
//extern int32_t IRbuff[400];
extern int32_t Rpmax1;
extern int32_t Rpmax2;
extern int16_t Tcycle;
extern float Xfreq[512];
extern uint16_t Freq;
extern uint16_t Freq2;
extern uint32_t IRpulse;
//extern  int32_t Ampmax;
extern int32_t LMSout;
extern int32_t LMSout2;
extern float e;
//extern int32_t Envelope[512];
//extern int32_t Envelope2[512];//¡ã¨¹????
//extern int32_t Envelope3[512];
//extern int32_t Envelope4[512];//¡ã¨¹????
//extern int32_t Output2buff[500];
//extern int32_t Outputbuff[500];
extern uint8_t SensorOff;
extern int32_t RLSout;
extern int32_t RLSout2;
extern int32_t dst_ref;
extern uint16_t testtemp;
extern float DCRatio;
extern float ACRatio2;
extern int8_t NormalRatio;

extern int8_t NormalRatioBuffer[80];
extern uint8_t StickFigure;
extern uint8_t Signal_Quality;
extern uint16_t pulsebuff[15];

/************function************/
void bpfilter();
void found(int32_t indexR,int32_t indexIR);
//void fft();
int brinv(double a[], int n);
void GetEnvelope();
void GetEnvelope2();
void Adaptivelms();
void Adaptive();
void Adaptive2();
uint16_t FreqCompare(uint16_t index,uint16_t index2);
void GetRefsignal();
uint16_t pulseprocess(uint16_t p);
uint16_t spo2process(uint16_t p);
uint8_t getnormal_PR(int index);
void select_window(int32_t index);
double RoXY(int x[],int y[],uint8_t n);



                 

extern void kalman1_init(kalman1_state state, float init_x, float init_p);
extern float kalman1_filter(kalman1_state state, float z_measure);

#endif