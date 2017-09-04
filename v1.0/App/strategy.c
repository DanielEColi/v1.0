#include "global.h"
#include "math.h"

kalman1_state SpO2state;

int NormalRatioOld=0;
uint8_t thrflag=0;
int Findmaxbuff[300]={0};
int MaxOutput2=0;
int Tvalue=0;
uint8_t Wfix=0;//窗口 1 中心偏移量
float Covxy=0;
int32_t Rmodelbuff[LP];
bool isFake=false;
bool isFake2=false;
uint8_t windowN=0;
uint8_t bmpsoundbuff1[150]={0};
uint8_t bmpsoundbuff2[150]={0};
uint8_t Pulsecount=0;
bool freqfirstflag1=true;//只能用一个标志
bool freqfirstflag2=true;
float SpO2old=0;
bool SpO2firstflag=true;
uint8_t SpO2count=0;
bool SpO2readyflag=false;
uint8_t ADCdatacount=0;

uint32_t Rdata[MYORDER]={0};
uint32_t IRdata[MYORDER]={0};
//uint32_t Outdata[MYORDER]={0};
//uint32_t Out2data[MYORDER]={0};
uint32_t Afe4490Rmax=0;
uint32_t Afe4490IRmax=0;
float PerI=0;
//const int BL = 301;

complex fftbuff[FFTLEN];
float Amfft[FFTLEN]={0};
uint16_t MaxwindowAmp=0;


float Bx[MYORDER] = {
 -0.01167936602915, -0.01165240177897, -0.01091661316242,-0.009550801747614,                //55
  -0.007712831306003,-0.005624566529168,-0.003548565840969,-0.001758882809299,
  -0.0005091182626805,-1.323415780965e-006,-0.000359417449469,-0.001610428005151,
  -0.003676113570621, -0.00637645205013, -0.00944518865536, -0.01255626591635,
   -0.01535866050215, -0.01751607469941, -0.01874720473795, -0.01886202931199,
   -0.01778977916372,   -0.015594958844, -0.01247893654626,-0.008766089035499,
  -0.004875138209062,-0.001277970906622, 0.001550285404275,  0.00318501862208,
   0.003304144703592, 0.001732706220345,-0.001525111552653,-0.006270645641489,
   -0.01211465563851, -0.01850113978983, -0.02474972944386, -0.03011336727622,
   -0.03384628323954, -0.03527603901642, -0.03387275773676,  -0.0293086742741,
   -0.02150184862903, -0.01063922644531,  0.00282391377585,  0.01818889285117,
    0.03456138818909,  0.05091824458932,  0.06618767499981,  0.07933581337929,
    0.08945186792245,  0.09582414730966,             0.098,  0.09582414730966,
    0.08945186792245,  0.07933581337929,  0.06618767499981,  0.05091824458932,
    0.03456138818909,  0.01818889285117,  0.00282391377585, -0.01063922644531,
   -0.02150184862903,  -0.0293086742741, -0.03387275773676, -0.03527603901642,
   -0.03384628323954, -0.03011336727622, -0.02474972944386, -0.01850113978983,
   -0.01211465563851,-0.006270645641489,-0.001525111552653, 0.001732706220345,
   0.003304144703592,  0.00318501862208, 0.001550285404275,-0.001277970906622,
  -0.004875138209062,-0.008766089035499, -0.01247893654626,   -0.015594958844,
   -0.01778977916372, -0.01886202931199, -0.01874720473795, -0.01751607469941,
   -0.01535866050215, -0.01255626591635, -0.00944518865536, -0.00637645205013,
  -0.003676113570621,-0.001610428005151,-0.000359417449469,-1.323415780965e-006,
  -0.0005091182626805,-0.001758882809299,-0.003548565840969,-0.005624566529168,
  -0.007712831306003,-0.009550801747614, -0.01091661316242, -0.01165240177897,
   -0.01167936602915
//    0.0002730580092674,-0.0002526664518887,-0.001676579637494,-0.003832863338109,//65
//  -0.006425459352181,-0.009071089181056, -0.01135871529051, -0.01291642747088,
//   -0.01347511375191, -0.01291831030203,    -0.01130938406,-0.008890475787543,
//  -0.006051957900202,-0.003275890745495,-0.001061351058417, 0.000157161977442,
//  8.534534897118e-005,-0.001373869817646,-0.004092143884961,-0.007722622337784,
//   -0.01174282993033, -0.01552880763852, -0.01845050071896, -0.01997446998167,
//   -0.01975803635064, -0.01771938061868, -0.01407095631854,-0.009308513262088,
//   -0.00415441582048,0.0005391320359686, 0.003914899354981, 0.005246458804626,
//   0.004065343935864,0.0002599645639319,-0.005870266114193, -0.01361653883185,
//   -0.02192147180544, -0.02949431726741, -0.03496772469256, -0.03707672807939,
//    -0.0348365909262, -0.02769524029954, -0.01563847846585, 0.000768269445166,
//    0.02040992600482,  0.04172235423337,  0.06285303183687,  0.08186303297044,
//    0.09694567921193,   0.1066345690421,   0.1099746602681,   0.1066345690421,
//    0.09694567921193,  0.08186303297044,  0.06285303183687,  0.04172235423337,
//    0.02040992600482, 0.000768269445166, -0.01563847846585, -0.02769524029954,
//    -0.0348365909262, -0.03707672807939, -0.03496772469256, -0.02949431726741,
//   -0.02192147180544, -0.01361653883185,-0.005870266114193,0.0002599645639319,
//   0.004065343935864, 0.005246458804626, 0.003914899354981,0.0005391320359686,
//   -0.00415441582048,-0.009308513262088, -0.01407095631854, -0.01771938061868,
//   -0.01975803635064, -0.01997446998167, -0.01845050071896, -0.01552880763852,
//   -0.01174282993033,-0.007722622337784,-0.004092143884961,-0.001373869817646,
//  8.534534897118e-005, 0.000157161977442,-0.001061351058417,-0.003275890745495,
//  -0.006051957900202,-0.008890475787543,    -0.01130938406, -0.01291831030203,
//   -0.01347511375191, -0.01291642747088, -0.01135871529051,-0.009071089181056,
//  -0.006425459352181,-0.003832863338109,-0.001676579637494,-0.0002526664518887,
//  0.0002730580092674
//-0.002300035762636,-0.005499801097726,-0.009033485022722, -0.01203497613096,  //82
//   -0.01373769977842, -0.01368144113944, -0.01184846767583,-0.008689940996708,
//  -0.005030422735521,-0.001869509823514,-0.0001273858171009,-0.0003979976563027,
//  -0.002774774645585,-0.006798239050902, -0.01154572552908, -0.01584757799616,
//   -0.01858052595415, -0.01896622560979, -0.01679757795752, -0.01252968568485,
//  -0.007203573949635,-0.002212006930336, 0.001042073588909, 0.001513248597305,
//   -0.00118563794336,-0.006631738386389, -0.01363898745782, -0.02050589032365,
//   -0.02541610507636, -0.02690170127407, -0.02425674717931, -0.01779299196361,
//  -0.008860437272682,0.0003924190939284,  0.00748228802301,  0.01018542513685,
//   0.007130893291918,  -0.0017486098564, -0.01502676262668, -0.02992090193887,
//     -0.042752582719, -0.04965384361375, -0.04738157433656, -0.03407013681772,
//  -0.009756387127277,  0.02344799718022,  0.06159220611859,   0.0995010143281,
//     0.1316587892754,   0.1531935861907,   0.1607672977972,   0.1531935861907,
//     0.1316587892754,   0.0995010143281,  0.06159220611859,  0.02344799718022,
//  -0.009756387127277, -0.03407013681772, -0.04738157433656, -0.04965384361375,
//     -0.042752582719, -0.02992090193887, -0.01502676262668,  -0.0017486098564,
//   0.007130893291918,  0.01018542513685,  0.00748228802301,0.0003924190939284,
//  -0.008860437272682, -0.01779299196361, -0.02425674717931, -0.02690170127407,
//   -0.02541610507636, -0.02050589032365, -0.01363898745782,-0.006631738386389,
//   -0.00118563794336, 0.001513248597305, 0.001042073588909,-0.002212006930336,
//  -0.007203573949635, -0.01252968568485, -0.01679757795752, -0.01896622560979,
//   -0.01858052595415, -0.01584757799616, -0.01154572552908,-0.006798239050902,
//  -0.002774774645585,-0.0003979976563027,-0.0001273858171009,-0.001869509823514,
//  -0.005030422735521,-0.008689940996708, -0.01184846767583, -0.01368144113944,
//   -0.01373769977842, -0.01203497613096,-0.009033485022722,-0.005499801097726,
//  -0.002300035762636
//   0.0003104064167818,-0.0006571005749855,-0.003252205718323,-0.006872694637198,   //85
//    -0.0105966400734, -0.01342807004252, -0.01457522659016, -0.01368466984305,
//    -0.0109632504511,-0.007148153613876,-0.003325849517702,-0.0006431366009076,
//  1.416309145643e-005,-0.001713622390406,-0.005528383620159, -0.01052038081289,
//   -0.01538921795173, -0.01878779315367, -0.01970021663715, -0.01775011672432,
//   -0.01334899412521,-0.007633209584866,-0.002193545491486, 0.001341154155776,
//    0.00176057285227,-0.001353964755799,-0.007448970720738, -0.01507479602196,
//   -0.02221664450789, -0.02680970888037,  -0.0273125272535, -0.02318753026476,
//   -0.01515100752874,-0.005105267770151,  0.00425730428679,  0.01010363979408,
//    0.01021663122887, 0.003673843592581,-0.008731506910973, -0.02445614886257,
//   -0.03960148025436, -0.04967791496746, -0.05058587351702, -0.03959884100479,
//   -0.01612058016682,  0.01797125031486,  0.05849671967358,  0.09964896214147,
//     0.1350473295536,   0.1589536978757,   0.1673953845959,   0.1589536978757,
//     0.1350473295536,  0.09964896214147,  0.05849671967358,  0.01797125031486,
//   -0.01612058016682, -0.03959884100479, -0.05058587351702, -0.04967791496746,
//   -0.03960148025436, -0.02445614886257,-0.008731506910973, 0.003673843592581,
//    0.01021663122887,  0.01010363979408,  0.00425730428679,-0.005105267770151,
//   -0.01515100752874, -0.02318753026476,  -0.0273125272535, -0.02680970888037,
//   -0.02221664450789, -0.01507479602196,-0.007448970720738,-0.001353964755799,
//    0.00176057285227, 0.001341154155776,-0.002193545491486,-0.007633209584866,
//   -0.01334899412521, -0.01775011672432, -0.01970021663715, -0.01878779315367,
//   -0.01538921795173, -0.01052038081289,-0.005528383620159,-0.001713622390406,
//  1.416309145643e-005,-0.0006431366009076,-0.003325849517702,-0.007148153613876,
//    -0.0109632504511, -0.01368466984305, -0.01457522659016, -0.01342807004252,
//    -0.0105966400734,-0.006872694637198,-0.003252205718323,-0.0006571005749855,
//  0.0003104064167818
};
/********lms??2¨2?êy*********/

float delta=0.001;
int32_t LMSout=0;
int32_t LMSout2=0;
float e=0;
float W[256]={0};
//int32_t Noise[256]={0};
//int32_t Noise2[256]={0};
//int32_t Envelope[512]={0};//??°ü????
//int32_t Envelope2[512]={0};//é?°ü????

//int32_t Envelope3[512]={0};//??°ü????
//int32_t Envelope4[512]={0};//é?°ü????
/********RLS??2¨2?êy*********/
uint8_t pn=1;//rls??2¨?×êy
float cfloat=0.5;//3?ê??μ??D??òDD
double Pmatrix[1][1]={2};
double Wmatrix[1][1]={0};
double Gmatrix[1][1]={0};
int32_t RLSout=0;
double Pmatrix2[1][1]={2};
double Wmatrix2[1][1]={0};
double Gmatrix2[1][1]={0};
int32_t RLSout2=0;



uint32_t A[MYORDER]={0};
int32_t Output=0;//oì1aê?3??μ
int32_t Output2=0;//oìía1aê?3??μ
int32_t Outputbuff[3]={0};
int32_t Output2buff[3]={0};
int32_t Output2old=0;
int32_t NorOutput=0;
int32_t NorOutput2=0;
//int32_t Output2buff[500]={0};
//int32_t Outputbuff[500]={0};
//int32_t Outputdeltabuff[256]={0};
int32_t Outputarray[LP]={0};//ó?óú?D????2?ò?
int32_t Output2array[LP2]={0};//ó?óú?D????2?ò?
uint8_t BmpSound=0;
uint8_t bmpsound[3]={0};//??2?ò?
uint8_t bmpsoundflag=1;
float Rvalue;//R?μ
float Rvalue2;//R?μ
float Rvalue3;//R?μ
float OutRvalue;//R?μ
float lastRvalue;//R?μ
float lastRvalue2;//R?μ

float SpO2=0;
float SpO2h=0;
uint16_t Bmp;//???ê?μ
float Pulse=0;
float Pulse1=0;
int32_t ORbuff[200]={0};
int32_t OIRbuff[200]={0};
int32_t ORpmax=0;
int32_t ORpmin=0;
int32_t OIRpmax=0;
int32_t OIRpmin=0;

int32_t FRbuff[200]={0};
int32_t FIRbuff[200]={0};
int32_t FRpmax=0;
int32_t FRpmin=0;
int32_t FIRpmax=0;
int32_t FIRpmin=0;

//int32_t Rbuff[400]={0};
//int32_t IRbuff[400]={0};
int32_t Rpmax1=0;
int32_t Rpmax2=0;
int32_t Rpmin1=0;
int32_t Rpmin2=0;
int16_t Rt1=0;
int16_t Rt2=0;

int32_t IRpmax1=0;
int32_t IRpmax2=0;
int32_t IRpmin1=0;
int32_t IRpmin2=0;
int16_t IRt1=0;
int16_t IRt2=0;
uint16_t time2560ms=0;
int16_t Tcycle=0;

//float X1real[512]={0};
//float X2real[512]={0};
//float X1image[512]={0};
//float X2image[512]={0};
//float XAmp[512]={0};
float Ampmax=0;
uint16_t Freq=0;
uint16_t Freqold=0;
uint16_t freqbuff[5]={0};
uint8_t Freqrefreshflag=0;
uint8_t Freqrefreshflag2=0;
uint16_t Freq2=0;
uint16_t Freqold2=0;
uint16_t freq2buff[5]={0};
uint16_t Pulse1buff[10]={0};
uint16_t Pulse2buff[10]={0};
uint32_t IRpulse=0;
//uint32_t pvalue[4]={0};//·??μ
//uint16_t Nposition[4]={0};//·??μ????
//float u=0.000007;
uint8_t extremefind=0;
uint16_t interpoint=55;
uint8_t extremefind2=0;
uint16_t interpoint2=31;
//int32_t env[2]={0};
//int32_t env2[2]={0};
//float dif1=0;
//float dif2=0;
uint8_t SensorOff=1;
uint8_t Debounce=0;//ó?óú????
uint16_t lmsN=256;
uint8_t PPchange=0;//?D??2¨·?2¨1èê?·???ì?
uint16_t checkFreq=55;//±￡?¤??ò?′?μ?2¨1è?ò??2¨·?2?3????ú2¨1èμ????ücheckFreq=3600/Freq;
uint8_t checkcount=0;
uint8_t Findminflag=0;
uint8_t Findmaxflag=0;

uint8_t extremefind3=0;
uint8_t extremefind4=0;
uint16_t interpoint3=55;
uint16_t interpoint4=55;
//int32_t env3[2]={0};
//int32_t env4[2]={0};
//float dif3=0;
//float dif4=0;
uint8_t PPchange2=0;
uint8_t Findminflag2=0;
uint8_t Findmaxflag2=0;

//int16_t dst_buff[120][50]={0};
int32_t dst_ref=0;
uint16_t testtemp=0;
uint8_t tempflag=0;
int32_t MinVar=10000000;
uint16_t pulsebuff[15]={0};
uint16_t spo2buff[15]={0};
//	 float Rvaluebuff[10]={0};
   int8_t countR=0;
	 float DCRatio;
   float DCRatio2;
	 float ACRatio2;
	 int8_t NormalRatio=0;
	int8_t NormalRatioBuffer[80]={0};
	 uint8_t StickFigure=0;
    uint8_t firstflag=1;
	 uint8_t Signal_Quality=0;
         
         
         
  // float Rvaluebuff2[50]={0};	
/*******************′?í¨??2¨?÷*******************************/
/*?é?????-1yá?′???2¨???±á÷D?o??ü31μ×μ???3y*/
/*******************′?í¨??2¨?÷*******************************/
void bpfilter()
{
  
  uint16_t n;
	//uint16_t j;
//static int32_t Outputmax=0,Output2max=0;
  int32_t sum=0,sum2=0;
  //uint32_t te=0;
  if(IsChangeLight==true)
  {
    for(n=0;n<MYORDER;n++)
    {
      Rdata[n]=0;
      IRdata[n]=0;
    }
     ADCdatacount=0;
  }
  else
  {
    ADCdatacount++;
  }
  if(ADCdatacount>=MYORDER)
  {
    ADCdatacount=MYORDER;
    thrflag2=1;
  }
  for(n=0;n<MYORDER-1;n++)
  {
    Rdata[n]=Rdata[n+1];
    IRdata[n]=IRdata[n+1];
//    Outdata[n]=Outdata[n+1];
//    Out2data[n]=Out2data[n+1];
  }
  Rdata[MYORDER-1]=Afe4490DataBuf[4];
  IRdata[MYORDER-1]=Afe4490DataBuf[5];
  /*****************IR filter***************/
  //n=0;
  Output2old=Output2;
  if(ADCdatacount==MYORDER)
  {
//    for(n=0;n<2;n++)
//		{
//			Outputbuff[n]=Outputbuff[n+1];
//			Output2buff[n]=Output2buff[n+1];
//		}
		for(n=0;n<=MYORDER-1;n++)
    {
      //te=(uint32_t)(B[n]*Rdata[n]);
      sum=sum+(int)((int)Bx[n]*Rdata[n]);
      
    }
    Output=sum/10000;//R value
     // n=0;
      /*****************IR filter***************/
    for(n=0;n<=MYORDER-1;n++)
    {
      //te=(uint32_t)(B[n]*Rdata[n]);
      sum2=sum2+(int)((int)Bx[n]*IRdata[n]);
                  
      
    }
    Output2=sum2/10000;//IR value
		
//		Output=(Outputbuff[0]+Outputbuff[1]+Outputbuff[2])/3;
//		Output2=(Output2buff[0]+Output2buff[1]+Output2buff[2])/3;
  }
	
//	 Outdata[MYORDER-1]=Output;
//  Out2data[MYORDER-1]=Output2;
//	n=0;
//	sum=0;
//	sum2=0;
//  for(n=0;n<=MYORDER-1;n++)
//  {
//    //te=(uint32_t)(B[n]*Rdata[n]);
//    sum=sum+(int)((int)Bx[n]*Outdata[n]);
//    
//  }
//  NorOutput=sum/10000;//R value
//    n=0;
//    /*****************IR filter***************/
//  for(n=0;n<=MYORDER-1;n++)
//  {
//    //te=(uint32_t)(B[n]*Rdata[n]);
//    sum2=sum2+(int)((int)Bx[n]*Out2data[n]);
//		
//    
//  }
//	NorOutput2=sum2/10000;//IR value
  //NorOutput2=Output2;
//	found(Output,Output2);
	//SensorOff=1;
	
//	if(Output>Outputmax)Outputmax=Output;
//	if(Output2>Output2max)Output2max=Output2;
//	Output=Output-Outputmax;
//	Output2=Output2-Output2max;
	if((Afe4490DataBuf[5]<1800000&&thrflag==0)||(Afe4490DataBuf[5]<1800000&&thrflag==1))//if(Output2>-10000&&Output2<180000)
	{
		SensorOff=0;
                thrflag=1;
	//	if(Afe4490Rmin=0)
		if(Debounce==0)//′??D?÷ê?·?í???????′|àí
		{
			Debounce=30;
		}
                //select_window(NorOutput2);
		//GetEnvelope();
		GetEnvelope2();
	//	GetRefsignal();
//		Adaptive();
//		Adaptive2();
		//found(Output,Output2);
                if(ADCdatacount==MYORDER)
                {
                  found(RLSout,RLSout2);
                }
                if(SpO2readyflag==true)
                {
                  SpO2readyflag=false;
                  //SpO2h=SpO2;
                  if(SpO2>100)SpO2=100;
                  SpO2h=spo2process(SpO2);
                  //SpO2h=SpO2;
                }
                if(Freqrefreshflag2==1)
                {
//                  Pulse=Freq2;
//                  bmpsoundflag=2;
//                  Freqrefreshflag2=0;
                    Pulse1=FreqCompare(Freq,Freq2);

			
                    if(Pulse1>255)Pulse1=255;
                    if(Pulse1<30)Pulse1=0;
                    Pulse=pulseprocess(Pulse1);
                    
                    //SpO2h=spo2process(SpO2);
                    Freqrefreshflag=0;
                    Freqrefreshflag2=0;
                }
//		if(Freqrefreshflag==1&&Freqrefreshflag2==1)	
//		{
//			Pulse1=FreqCompare(Freq,Freq2);
//
//			
//			if(Pulse1>255)Pulse1=255;
//			if(Pulse1<30)Pulse1=0;
//                        Pulse=pulseprocess(Pulse1);
//                        
//			//SpO2h=spo2process(SpO2);
//			Freqrefreshflag=0;
//			Freqrefreshflag2=0;
//		}
		//BmpSound=bmpsound[2];
			if(bmpsoundflag==1)
			{
                          if((uint16_t)(6000/(Freq+1)-LP/2)>=0&&(uint16_t)(6000/(Freq+1)-LP/2)<150)//选择窗口
                          {BmpSound=bmpsoundbuff1[149-(uint16_t)(6000/(Freq+1)-LP/2)];}
			}
			else if(bmpsoundflag==2)
			{
			 if((uint16_t)(6000/(Freq2+1)-20)>=0&&(uint16_t)(6000/(Freq2+1)-20)<150)	
                          BmpSound=bmpsoundbuff2[149-(uint16_t)(6000/(Freq2+1)-20)];
                          //BmpSound=bmpsoundbuff2[149-(uint16_t)(6000/(Freq+1)-LP/2)];
			}
			else if(bmpsoundflag==0)
			{
				BmpSound=0;
			}
	}
	if((Afe4490DataBuf[5]>=1800000&&thrflag==0)||(Afe4490DataBuf[5]>=1800000&&thrflag==1)||(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5))==1||isFake==true)//||||isFake2==true
	{
		
		thrflag=0;
                thrflag2=0;
                NormalRatioOld=0;
                if(Debounce<=0)
		{
			SensorOff=1;
		}
		else
		{
			Debounce--;
		}
		Afe4490Rmax=0;
		Afe4490IRmax=0;
		countR=0;
		Freq=0;
		Freq2=0;
		Pulse=0;
		SpO2=0;
                SpO2h=0;
                Pulse=0;
                Pulse1=0;
                Signal_Quality=0;
                PerI=0;
		NormalRatio=0;
		StickFigure=0;
		extremefind=0;
		extremefind2=0;
		extremefind3=0;
		extremefind4=0;
//		checkFreq=55;
//		interpoint=checkFreq+1;
//		interpoint2=31;
//		interpoint3=checkFreq+1;
//		interpoint4=31;
		Findmaxflag2=0;
		Findminflag2=0;
		tempflag=0;
		MinVar=10000000;
		firstflag=1;
                Pulsecount=0;
                Freqold=0;
                Freqold2=0;
                freqfirstflag1=true;
                freqfirstflag2=true;
                SpO2count=0;
                SpO2old=0;
                SpO2firstflag=true;
                SpO2readyflag=false;
                //isFake2=false;
                for(n=0;n<15;n++)
                {
                  pulsebuff[n]=0;
                  spo2buff[n]=0;
                  if(n<5)
                  {
                    freqbuff[n]=0;
                    freq2buff[n]=0;
                  }
                }
                for(n=0;n<400;n++)
                {
//                  Rbuff[n]=0;
//                  IRbuff[n]=0;
                  ORbuff[n]=0;
                  OIRbuff[n]=0;
		
                  FRbuff[n]=0;
                  FIRbuff[n]=0;
                  if(n<200)
                  {
                    Findmaxbuff[n]=0;
                  }
                  if(n<150)
                  {
                    bmpsoundbuff1[n]=0;
                    bmpsoundbuff2[n]=0;
                  }
                  if(n<10)
                  {
                    Pulse1buff[n]=0;
                    Pulse2buff[n]=0;
                  }
                }
                time2560ms=0;
                NormalRatioOld=0;
                
                
	}
        if((Afe4490DataBuf[5]>=1800000&&thrflag==0)||(Afe4490DataBuf[5]>=1800000&&thrflag==1))
        {
           ADCdatacount=0;
        }
		
}
/*function:?üD?RbuffoíIRbuffá???êy×é￡?fft±???￡??μóò·???
*
*
*
*/
void found(int32_t indexR,int32_t indexIR)
{
	uint16_t n=0;
	uint16_t j=0;
	uint16_t k=0,Pw=150;
	uint32_t temp=0;
	int32_t sum=0;
        float x;
        static float y=0;

//static float
  float Sum1=0,Sum2=0;
float ErrorR=0;
float ErrorR1=0;
	for(n=0;n<149;n++)
	{
//		Rbuff[n]=Rbuff[n+1];
//		IRbuff[n]=IRbuff[n+1];
		ORbuff[n]=ORbuff[n+1];
		OIRbuff[n]=OIRbuff[n+1];
		
		FRbuff[n]=FRbuff[n+1];
		FIRbuff[n]=FIRbuff[n+1];
	}
//	Rbuff[149]=indexR;
//	IRbuff[149]=indexIR;
	ORbuff[149]=Afe4490DataBuf[4];
	OIRbuff[149]=Afe4490DataBuf[5];
	
	FRbuff[149]=Output;
	FIRbuff[149]=Output2;
	time2560ms++;
        isFake2=false;
	if(time2560ms==150)
	{
		time2560ms=0;
//		Rpmax1=Rbuff[0];
//		//Rpmax2=Rbuff[0];
//		Rpmin1=Rbuff[0];
		//Rpmin2=Rbuff[0];
		
//		IRpmax1=IRbuff[0];
//		//IRpmax2=IRbuff[0];
//		IRpmin1=IRbuff[0];
		//IRpmin2=IRbuff[0];
		
		ORpmax=ORbuff[0];
		ORpmin=ORbuff[0];
		
		
		OIRpmax=OIRbuff[0];
		OIRpmin=OIRbuff[0];
		
		FRpmax=FRbuff[0];
		FRpmin=FRbuff[0];
		
		
		FIRpmax=FIRbuff[0];
		FIRpmin=FIRbuff[0];
		if(Pulse>80&&Pulse<150)
		{
			Pw=100;
		}
		else if(Pulse>=150&&Pulse<200)
		{
			Pw=60;
		}
		else if(Pulse>=200&&Pulse<250)
		{
			Pw=40;
		}
		else if(Pulse<=80)
		{
			Pw=150;
		}
		
		
		k=0;
		for(n=151-Pw;n<149;n++)
		{
			//if(n>=100)
			{
//				if((Rbuff[n]>=Rbuff[n-1])&&(Rbuff[n]>=Rbuff[n+1])&&(Rbuff[n]>Rpmax1))
//				{
//					Rpmax1=Rbuff[n];
//					//Rt1=n;
//				}
//				if((Rbuff[n]<=Rbuff[n-1])&&(Rbuff[n]<=Rbuff[n+1])&&(Rbuff[n]<=Rpmin1))
//				{
//					Rpmin1=Rbuff[n];
//				}
//				if((IRbuff[n]>=IRbuff[n-1])&&(IRbuff[n]>=IRbuff[n+1])&&(IRbuff[n]>IRpmax1))
//				{
//					IRpmax1=IRbuff[n];
//					//Rt1=n;
//				}
//				if((IRbuff[n]<=IRbuff[n-1])&&(IRbuff[n]<=IRbuff[n+1])&&(IRbuff[n]<=IRpmin1))
//				{
//					IRpmin1=IRbuff[n];
//				}
	//			/******************************/
				if((ORbuff[n]>=ORbuff[n-1])&&(ORbuff[n]>=ORbuff[n+1])&&(ORbuff[n]>ORpmax))
				{
					ORpmax=ORbuff[n];
					//Rt1=n;
				}
				if((ORbuff[n]<=ORbuff[n-1])&&(ORbuff[n]<=ORbuff[n+1])&&(ORbuff[n]<=ORpmin))
				{
					ORpmin=ORbuff[n];
				}
				if((OIRbuff[n]>=OIRbuff[n-1])&&(OIRbuff[n]>=OIRbuff[n+1])&&(OIRbuff[n]>OIRpmax))
				{
					OIRpmax=OIRbuff[n];
					//Rt1=n;
				}
				if((OIRbuff[n]<=OIRbuff[n-1])&&(OIRbuff[n]<=OIRbuff[n+1])&&(OIRbuff[n]<=OIRpmin))
				{
					OIRpmin=OIRbuff[n];
				}
		
			/************/
			/******************************/
			if((FRbuff[n]>=FRbuff[n-1])&&(FRbuff[n]>=FRbuff[n+1])&&(FRbuff[n]>FRpmax))
			{
				FRpmax=FRbuff[n];
				//Rt1=n;
			}
			if((FRbuff[n]<=FRbuff[n-1])&&(FRbuff[n]<=FRbuff[n+1])&&(FRbuff[n]<=FRpmin))
			{
				FRpmin=FRbuff[n];
			}
			if((FIRbuff[n]>=FIRbuff[n-1])&&(FIRbuff[n]>=FIRbuff[n+1])&&(FIRbuff[n]>FIRpmax))
			{
				FIRpmax=FIRbuff[n];
				//Rt1=n;
			}
			if((FIRbuff[n]<=FIRbuff[n-1])&&(FIRbuff[n]<=FIRbuff[n+1])&&(FIRbuff[n]<=FIRpmin))
			{
				FIRpmin=FIRbuff[n];
			}
			
                    }
		}
		
		//NormalRatio=NormalRatio;//127*(float)(FIRpmax-Output2)/(float)(FIRpmax-FIRpmin);
//		for(n=1;n<255;n++)
//		{
//			if((IRbuff[n]>=IRbuff[n-1])&&(IRbuff[n]>=IRbuff[n+1])&&k!=4&&((IRbuff[n]-IRpmin1)>(0.9*(float)(IRpmax1-IRpmin1))))
//			{
//				pvalue[k]=IRbuff[n];
//				Nposition[k]=n;
//				k++;
//			}
//		}
//		Freq=3000/(Nposition[1]-Nposition[0]);
//		if(Freq<30||Freq>300)
//		{Freq=Freqold;}
//		Freqold=Freq;

//		for(n=0;n<4;n++)
//		{
//			pvalue[n]=0;
//			Nposition[n]=0;
//		}
		
//		for(n=0;n<200;n++)
//		{
//			sum=sum+Envelope[n];
//		}
//		Rpmin1=sum/200;
//		sum=0;
//	 for(n=0;n<200;n++)
//		{
//			sum=sum+Envelope2[n];
//		}
//		Rpmax1=sum/200;
//			sum=0;
//	 for(n=0;n<200;n++)
//		{
//			sum=sum+Envelope3[n];
//		}
//		IRpmin1=sum/200;
//			sum=0;
//	 for(n=0;n<200;n++)
//		{
//			sum=sum+Envelope4[n];
//		}
//		IRpmax1=sum/200;
//  Rvalue=(float)((Rpmax1-Rpmin1)*(IRpmax1+IRpmin1))/(float)((IRpmax1-IRpmin1)*(Rpmax1+Rpmin1));

//	ACRatio2=(float)((Rpmax1-Rpmin1))/(float)((IRpmax1-IRpmin1));
////	if(abs(ACRatio2*100-testtemp)>5)
////	{
////		MinVar=10000000;
////	}
//	DCRatio=(float)((OIRpmax*0.1+OIRpmin*0.1)/(float)(ORpmax*0.1+ORpmin*0.1));
//	Rvalue=(float)DCRatio*testtemp*0.01;
//	Rvalue3=(float)DCRatio*ACRatio2;
        
	DCRatio2=(float)((OIRpmax*0.1+OIRpmin*0.1)/(float)(ORpmax*0.1+ORpmin*0.1));
        ACRatio2=(float)((float)(FRpmax-FRpmin)/(float)(FIRpmax-FIRpmin)); 
        
        if(SpO2firstflag==false)
        {
          if(SpO2count<=1)
          {
            kalman1_init(SpO2state, (float)ACRatio2, 1);
          }
          else
          {
            ACRatio2=kalman1_filter(SpO2state, (float)ACRatio2);
          }
        }
        
        Rvalue2=DCRatio2*ACRatio2;
	//Rvalue2=(float)((FRpmax-FRpmin)*(OIRpmax*0.1+OIRpmin*0.1))/(float)((FIRpmax-FIRpmin)*(ORpmax*0.1+ORpmin*0.1));
        
//	if(countR<10
       
//        if((FRpmax-FRpmin)>90000)
//        {
//          isFake2=true;
//        }
        x=2*(FIRpmax-FIRpmin)/(OIRpmax*0.1+OIRpmin*0.1);
        y = -0.5223*x*x + 10.443*x - 0.0178;
        //isFake2=false;
        if(y>30)
        {
          isFake2=true;
        }

//if(abs((-24.316*Rvalue*Rvalue+3.1365*Rvalue+103.36)-(-24.316*Rvalue3*Rvalue3+3.1365*Rvalue3+103.36))<2)
//{
//	if(Rvalue2<Rvalue)
//	{OutRvalue=Rvalue2;}
//	else OutRvalue=Rvalue;
//	OutRvalue=Rvalue;
//		//Rvalue=(float)((Rpmax1-Rpmin1)*(Afe4490DataBuf[5]))/(float)((IRpmax1-IRpmin1)*(Afe4490DataBuf[4]));
//		//Rvalue=(float)((Rpmax1-Rpmin1)*(Envelope4[200]+Envelope3[200]))/(float)((IRpmax1-IRpmin1)*(Envelope2[200]+Envelope[200]));
//	
//		SpO2=-24.316*OutRvalue*OutRvalue+3.1365*OutRvalue+103.36;//?ao??a???¨?èó?R?μêyμ?1??μoˉêy
//     firstflag=0;
//}
//else 
//{
//	MinVar=1000000;
//	if(firstflag==1)
//	{
//		SpO2=-24.316*Rvalue2*Rvalue2+3.1365*Rvalue2+103.36;
//	}
//}
//	if(Signal_Quality<12||Pulse>=180)
//	{
//	SpO2=-24.316*Rvalue2*Rvalue2+3.1365*Rvalue2+103.36;
//	}		
	SpO2=-30.363*Rvalue2*Rvalue2+11.962*Rvalue2+100.34;
//SpO2=-34.727*Rvalue2*Rvalue2+19.817*Rvalue2+97.449;
//SpO2=ACRatio2*100-50;
if((SpO2-(int32_t)SpO2)>=0.5)
		{SpO2=(int32_t)SpO2+1;}
		else
		{SpO2=(int32_t)SpO2;}
//		if(SpO2>100)
//		{SpO2=100;}
		if(SpO2<0)
		{SpO2=0;}

         if(isFake==false)
         {
          if(SpO2firstflag==true)
          {
            if(abs(SpO2-SpO2old)<=2&&(SpO2!=0))//前后误差在3以内则出数据
            {
              SpO2firstflag=false;
              SpO2readyflag=true;
            }
          }
          else
          {
            SpO2readyflag=true;
          }
         }
          SpO2old=SpO2;
		
	}
       
//	NorOutput=(FRpmax-Output);
//	NorOutput2=(FIRpmax-Output2);
       // NormalRatio=127*(float)(FIRpmax-NorOutput2)/(float)(600*sqrt(FIRpmax-FIRpmin));//?T?1???ˉ?éè?1éò??ˉ2¨D?
        NormalRatio=getnormal_PR(Output2);//getnormal_PR(Afe4490DataBuf[5]);//
       // NormalRatio=getnormal_PR(Afe4490DataBuf[5]);
	//NormalRatio=127*(float)(IRpmax1-indexIR)/(float)(400*sqrt(IRpmax1-IRpmin1));//óD?1???ˉ?éè?1éò??ˉ2¨D?
	if(FIRpmax-FIRpmin>8000)       Signal_Quality=15;
	else if(FIRpmax-FIRpmin>6000)Signal_Quality=12;
	else if(FIRpmax-FIRpmin>4000)Signal_Quality=9;
	else if(FIRpmax-FIRpmin>2000)Signal_Quality=6;
	else if(FIRpmax-FIRpmin>1500)Signal_Quality=3;
	else if(FIRpmax-FIRpmin>1300)Signal_Quality=2;
        else if(FIRpmax-FIRpmin>1100)Signal_Quality=1;
        else if(FIRpmax-FIRpmin<1000)Signal_Quality=0;
        
if(NormalRatio>=126)
NormalRatio=126;
if(NormalRatio<=0)NormalRatio=0;
y=y>20?20:y;
PerI=y;
//PerI=(PerI>15)?15:PerI;
if(PerI>=1)
{
if((PerI-(int32_t)PerI)>=0.5)
		{PerI=(int32_t)PerI+1;}
		else
		{PerI=(int32_t)PerI;}
}
//else PerI*=10;

Signal_Quality=PerI;
Signal_Quality=(Signal_Quality>15)?15:Signal_Quality;
PerI=(PerI*10);
if(PerI<10)
{
  if((PerI-(int32_t)PerI)>=0.5)
		{PerI=(int32_t)PerI+1;}
		else
		{PerI=(int32_t)PerI;}
}

for(n=0;n<79;n++)
	{
		NormalRatioBuffer[n]=NormalRatioBuffer[n+1];
	}
	NormalRatioBuffer[79]=NormalRatio;
	//if(bmpsoundflag)
StickFigure=NormalRatioBuffer[79]/8;//bmpsoundflag
if(StickFigure>=15)StickFigure=15;
if(StickFigure<=0)StickFigure=0;
	//fft();//test
}

/**************?ó?????ó**********************/
int brinv(double a[], int n)
   { int *is,*js,i,j,k,l,u,v;
    double d,p;
    is=malloc(n*sizeof(int));
    js=malloc(n*sizeof(int));
    for (k=0; k<=n-1; k++)
      { d=0.0;
        for (i=k; i<=n-1; i++)
        for (j=k; j<=n-1; j++)
          { l=i*n+j; p=fabs(a[l]);
            if (p>d) { d=p; is[k]=i; js[k]=j;}
          }
        if (d+1.0==1.0)
          { free(is); free(js); 
            return(0);
          }
        if (is[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=is[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (js[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+js[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        l=k*n+k;
        a[l]=1.0/a[l];
        for (j=0; j<=n-1; j++)
          if (j!=k)
            { u=k*n+j; a[u]=a[u]*a[l];}
        for (i=0; i<=n-1; i++)
          if (i!=k)
            for (j=0; j<=n-1; j++)
              if (j!=k)
                { u=i*n+j;
                  a[u]=a[u]-a[i*n+k]*a[k*n+j];
                }
        for (i=0; i<=n-1; i++)
          if (i!=k)
            { u=i*n+k; a[u]=-a[u]*a[l];}
      }
    for (k=n-1; k>=0; k--)
      { if (js[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=js[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (is[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+is[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
      }
    free(is); free(js);
    return(1);
  }
	/*
	?°?ò???μμ??￠2??μ
	
	*/
void GetEnvelope()
{
uint16_t n;	
uint16_t Lpp=0;

//uint16_t Lpp2=0;

	for(n=0;n<LP-1;n++)
	{	
		Outputarray[n]=Outputarray[n+1];
	}
	//Output2array[70]=Output2-Output;
	Outputarray[LP-1]=Output2;//-Output2old;
	if((Outputarray[LP/2+Wfix]<Outputarray[LP/2-1+Wfix]&&Outputarray[LP/2+Wfix]<=Outputarray[LP/2+1+Wfix]))
	{
		for(n=0;n<LP;n++)
		{
			if(Outputarray[LP/2+Wfix]<=Outputarray[n])
			{
				Findminflag++;
			}
		}
	}
/************??°ü???? **************/
	if(Findminflag==LP)//&&(IRpmax1-Output2array[1])>0.8*(IRpmax1-IRpmin1)(2*Output2array[1]-Output2array[0]-Output2array[2])>10
	{                                                                                                       //||(Output2array[1]<Output2array[0]&&Output2array[1]<Output2array[2])
	//	PPchange=1; 
                                                                                 //&&interpoint>=checkFreq&&interpoint2>=checkFreq/3&&PPchange==0
//                Covxy=RoXY(Rmodelbuff,Outputarray,LP);
//                for(n=0;n<LP;n++)
//                {
//                  Rmodelbuff[n]=Outputarray[n];
//                }
		bmpsound[1]=1;
		extremefind++;//è·?¨???μμ???êy
		
	}
	else if(extremefind==1)
	{
		bmpsound[1]=0;
		interpoint++;//è·?¨2??μ??êy
	}	
	Findminflag=0;
	
if(extremefind==2)
{
	Lpp=extremefind+interpoint;//2??μoó×ü3¤?è?￥???-D?o?3¤?è
	/*********???????ê************/
	Freq=6000/(Lpp-1);
	for(n=0;n<9;n++)
        {
          Pulse1buff[n]=Pulse1buff[n+1];
        }
        if(freqfirstflag1==true)
        {
          if(abs(6000/Freq-6000/Freqold)<=8)
          {
            Freqrefreshflag=1;
            freqfirstflag1=false;
            freqfirstflag2=false;
            Pulse1buff[9]=Freq;
            windowN=1;
          }
        }
        else
        {
          Freqrefreshflag=1;
          Pulse1buff[9]=Freq;
        }
        if(Freq<35||Freq>300)
	{Freq=Freqold;}
        Freqold=Freq;
    
    

	extremefind=1;	
	interpoint=0;
}
for(n=0;n<149;n++)
{
  bmpsoundbuff1[n]=bmpsoundbuff1[n+1];
}
bmpsoundbuff1[149]=bmpsound[1];

}
/*??è?oìía1aμ?°ü????
*
*/
void GetEnvelope2()
{
uint16_t n;	
uint16_t Lpp=0;
uint16_t Lpp2=0;
int deltaPP=0;
int max=0,min=0;
	uint16_t linepointUp=0,linepointDown=0;
	for(n=0;n<199;n++)
        {
          Findmaxbuff[n]=Findmaxbuff[n+1];
        }
        Findmaxbuff[199]=Output2;
        max=Findmaxbuff[0];
        min=Findmaxbuff[0];
        for(n=0;n<200;n++)
        {
          if(max<=Findmaxbuff[n])
          {
            max=Findmaxbuff[n];
          }
          if(min>=Findmaxbuff[n])
          {
            min=Findmaxbuff[n];
          }
        }
        //MaxOutput2=max;
	for(n=0;n<LP2-1;n++)
	{	
		Output2array[n]=Output2array[n+1];
	}
	//Output2array[70]=Output2-Output;
	Output2array[LP2-1]=Output2;//-Output2old;
	if((Output2array[(LP2-1)/2]<Output2array[(LP2-1)/2-1]&&Output2array[(LP2-1)/2]<=Output2array[(LP2-1)/2+1]))
	{
		for(n=0;n<LP2;n++)
		{
			if(Output2array[(LP2-1)/2]<=Output2array[n])//±￡?¤ê???′ó?ò????D??μ
			{
				Findminflag2++;
                                
			}

		}
	}

/************??°ü???? **************/
	if(Findminflag2==LP2)//&&(IRpmax1-Output2array[1])>0.8*(IRpmax1-IRpmin1)(2*Output2array[1]-Output2array[0]-Output2array[2])>10
	{                                                                                                       //||(Output2array[1]<Output2array[0]&&Output2array[1]<Output2array[2])
		
               deltaPP=max-Output2array[(LP2-1)/2];
               Tvalue=max-min;
                if(deltaPP>0.8*Tvalue)
                {
                  bmpsound[2]=1; 
                  extremefind3++;
                }
                else
                {
                  bmpsound[2]=0;
                  interpoint3++;
                }
                //Tvalue=deltaPP;
	//	if()
		//è·?¨???μμ???êy
               
		//Covxy=Roxy(Rmodelbuff,Output2array,LP2);
	}
	else if(extremefind3==1)
	{
		bmpsound[2]=0;
		interpoint3++;//è·?¨2??μ??êy
//		Findminflag=0;
	}	

	
		Findmaxflag2=0;
		Findminflag2=0;

if(extremefind3==2)
{
	Lpp2=extremefind3+interpoint3;//2??μoó×ü3¤?è?￥???-D?o?3¤?è
	/******???????ê************/
	Freq2=6000/(Lpp2-1);
        for(n=0;n<9;n++)
        {
          Pulse2buff[n]=Pulse2buff[n+1];
        }
        if(freqfirstflag2==true)
        {
          if(abs(6000/Freq2-6000/Freqold2)<=8)
          {
            Freqrefreshflag2=1;
            freqfirstflag1=false;
            freqfirstflag2=false;
            Pulse2buff[9]=Freq2;
            windowN=2;
          }
        }
        else
        {
          Freqrefreshflag2=1;
          Pulse2buff[9]=Freq2;
        }
        if(Freq2<35||Freq2>300)
	{Freq2=Freqold2;}
        Freqold2=Freq2;
	extremefind3=1;	
	interpoint3=0; 
}
for(n=0;n<149;n++)
{
  bmpsoundbuff2[n]=bmpsoundbuff2[n+1];
}
bmpsoundbuff2[149]=bmpsound[2];

}

/*?ùóúRLS??·¨μ?×?êêó|??2¨
*??oì1aμ?′|àí
*/
void Adaptive()
{
		uint16_t n=0;
		int32_t sum=0,sum2=0;	
	  double e0;
		//long long norm=0;
//		for(n=0;n<pn;n++)
//		{
//			Noise[n]=Noise[n+1];
//		}
//		Noise[pn-1]=(Envelope2[200]+Envelope[200])/2;
		Gmatrix[0][0]=Pmatrix[0][0]*dst_ref/(1+dst_ref*Pmatrix[0][0]*dst_ref);
		e0=NorOutput-dst_ref*Wmatrix[0][0];
		Wmatrix[0][0]=Wmatrix[0][0]+Gmatrix[0][0]*e0;
		Pmatrix[0][0]=Pmatrix[0][0]-Gmatrix[0][0]*dst_ref*Pmatrix[0][0];
		RLSout=e0;
		
}
/*?ùóúRLS??·¨μ?×?êêó|??2¨
*??oìía1aμ?′|àí
*/
void Adaptive2()
{
		uint16_t n=0;
		int32_t sum=0,sum2=0;	
	  double e0;
		//long long norm=0;
//		for(n=0;n<pn;n++)
//		{
//			Noise2[n]=Noise2[n+1];
//		}
		//Noise2[pn-1]=(Envelope4[200]+Envelope3[200])/2;
		Gmatrix2[0][0]=Pmatrix2[0][0]*dst_ref/(1+dst_ref*Pmatrix2[0][0]*dst_ref);
		e0=NorOutput2-dst_ref*Wmatrix2[0][0];
		Wmatrix2[0][0]=Wmatrix2[0][0]+Gmatrix2[0][0]*e0;
		Pmatrix2[0][0]=Pmatrix2[0][0]-Gmatrix2[0][0]*dst_ref*Pmatrix2[0][0];
		RLSout2=e0;
		
}
/*á??·?μ?êμ?±è??
*
*/
uint16_t FreqCompare(uint16_t index,uint16_t index2)
{
	uint8_t n=0,i=0,j=0;
	float sum=0,sum2=0;
        float wsum=0,wsum2=0;
	float Error=0,Error2=0;
        float wError=0,wError2=0;
        uint16_t seqbuff1[4]={0},seqbuff2[4]={0},temp1=0,temp2=0;
        Pulsecount++;
        if(Pulsecount>=5)
        {
           Pulsecount=5;
        }
	for(n=0;n<4;n++)
	{
		freqbuff[n]=freqbuff[n+1];
		freq2buff[n]=freq2buff[n+1];
	}
       
	//freqbuff[4]=index;
	freq2buff[4]=index2;
//        if(Pulsecount==5)
//        {
//          for(n=1;n<5;n++)//排序准备
//          {
//          //  seqbuff1[n-1]=freqbuff[n];
//            seqbuff2[n-1]=freq2buff[n];
//          }
//          for(i=0;i<3;i++)//冒泡排序
//          {
//            for(j=i+1;j<4;j++)
//            {
////              if(seqbuff1[j]>seqbuff1[i])
////              {
////                temp1=seqbuff1[i];
////                seqbuff1[i]=seqbuff1[j];
////                seqbuff1[j]=temp1;
////              }
//              if(seqbuff2[j]>seqbuff2[i])
//              {
//                temp2=seqbuff2[i];
//                seqbuff2[i]=seqbuff2[j];
//                seqbuff2[j]=temp2;
//              }
//            }
//          }
//          
//        }
        if(Pulsecount>=2)
        {
          for(n=(5-Pulsecount);n<5;n++)
          {
          //  wsum=wsum+freqbuff[n];//(float)(freqbuff[1]+freqbuff[2]+freqbuff[3]+freqbuff[4])/4;//freqbuff[0]+
            wsum2=wsum2+freq2buff[n];//(float)(freq2buff[1]+freq2buff[2]+freq2buff[3]+freq2buff[4])/4;//freq2buff[0]+
          }
         // wsum=wsum/Pulsecount;
          wsum2=wsum2/Pulsecount;
          for(n=(5-Pulsecount);n<5;n++)
          {
                 // wError=wError+abs(wsum-freqbuff[n]);
                  wError2=wError2+abs(wsum2-freq2buff[n]);
          }
          //wError=wError/Pulsecount;
          wError2=wError2/Pulsecount;
  //        wError=wError;
  //        wError2=wError2;
  //        sum=(seqbuff1[1]+seqbuff1[2])/2;
  //        sum2=(seqbuff2[1]+seqbuff2[2])/2;
  //        Error=abs(seqbuff1[1]-seqbuff1[2]);
  //        Error2=abs(seqbuff2[1]-seqbuff2[2]);
          /*******************±è??·?2?*****************/
  //	if(Error<Error2&&Error<=15)
  //	{
  //		if(sum<30)sum=30;
  //			if(sum>250)sum=250;
  //		return sum;
  //	}
  //	else if(Error>=Error2&&Error2<=15)
  //	{
  //		if(sum2<30)sum2=30;
  //			if(sum2>250)sum2=250;
  //return sum;
  //	}
         // return sum2;
  //        if(MaxwindowAmp>8)//窗口预估  算法复杂度过高，没调用
  //        {
  //          if(Error2<=60)
  //          {
  //            bmpsoundflag=2;
  //            return sum2;
  //          }
  //          else 
  //          {
  //            return Pulse1;
  //          }
  //        }
  //        if(Error<=3*(6000/sum-6000/(sum+1)))
  //        {
  //          return 1;
  //        }
  //        else
  //        {
  //          return Pulse1;
  //        }
          if(Pulsecount<5)
          {
            if(wError2<=3*(wsum2-6000/(6000/wsum2+1)))//′?′|óDbug?Y2?3?à′
            {
//                    if((abs(wsum-wsum2)>50))//||MaxwindowAmp>8)
//                    {
//                      if(wError2<=3*(wsum2-6000/(6000/wsum2+1)))
//                      {
//                        bmpsoundflag=2;
//                        return wsum2;
//                      }
//                      else return Pulse1;
//                    }
//                    else  
//                    {
//                      if(wError<=3*(wsum-6000/(6000/wsum+1)))
//                      {
//                        bmpsoundflag=1;
//                        return wsum;
//                      }
//                      else return Pulse1;
//                    }
//    //                else 
//    //                  return Pulse1;
              bmpsoundflag=2;
              return wsum2;
                      
            }
            else 
            {
                    //bmpsoundflag=0;
                    return Pulse1;
            }
          }
          if(Pulsecount==5)
          {
            if(wError2<=4)//′?′|óDbug?Y2?3?à′
            {
//                    if((abs(wsum-wsum2)>50))//||MaxwindowAmp>8)
//                    {
//                      if(wError2<=4)
//                      {
//                        bmpsoundflag=2;
//                        return wsum2;
//                      }
//                      else return Pulse1;
//                    }
//                    else  
//                    {
//                      if(wError<=4)
//                      {
//                        bmpsoundflag=1;
//                        return wsum;
//                      }
//                      else return Pulse1;
//                    }
//    //                else 
//    //                  return Pulse1;
                      bmpsoundflag=2;
                       return wsum2;
            }
            else 
            {
                    //bmpsoundflag=0;
                    return Pulse1;
            }
          }
        }
        else
        {
          return Pulse1;
        }

}
/*11?¨2???D?o?N'=Rd-IR*r ￡????Drê?óé0μ?1±??ˉμ??μêy
*
*/
//void GetRefsignal()
//{
//	uint16_t r=0,n=0,m=0;
//	static uint16_t count=0,count2=0,temp=0,timecount=0;
//	
//	//float Rv[101]={0};
//	uint8_t Ra=0;
//	//int32_t Es=0,En=0,Esn=0,Cov[101]={0};
//	float Error[120]={0};
//	int32_t Sum[120]={0};
//	//static int32_t A11=0,A12=0,A22=0;
//	//static int32_t dst_Rbuff[25]={0},dst_IRbuff[25]={0};
//	count++;
//	count2++;
//	timecount++;
//
//	/************êy?Y?üD?*************/
//	if(count==2)
//	{
//		count=0;
//		for(r=0;r<120;r++)
//		{
//			for(n=0;n<49;n++)
//			{
//				dst_buff[r][n]=dst_buff[r][n+1];
//			}
//			dst_buff[r][49]=(uint16_t)(0.2*((NorOutput-(NorOutput2)*r*0.01)));
//		}
//	}
//	/************?ó3??üá?×?D?μ?μ?r*************/
//	if(count2==100)//1???ó?üD?ò?′?r
//	{
//		count2=0;
//		for(r=0;r<120;r++)
//		{
//			for(n=0;n<50;n++)
//			{
//				Sum[r]=Sum[r]+dst_buff[r][n];
//			}
//			Sum[r]=Sum[r]/50;
//		}
//		for(r=0;r<120;r++)
//		{
//			for(n=0;n<50;n++)
//			{
//				Error[r]=abs(dst_buff[r][n]-Sum[r])+Error[r];
//			}
//		}
//	//	if(tempflag==0)
//		{
//			tempflag=1;
//			for(r=0;r<119;r++)
//			{
//				
//				if(Error[0]>Error[r+1])
//				{
//					Error[0]=Error[r+1];
//					
//			//		if(Error[0]<MinVar)           //±￡?¤rê????èê±μ??μ
//				//	if(abs(temp-r)>10)
//					{						
//						temp=r+1;                    //μ?μ?·?2?×?D?μ?μ?r?μ
//						MinVar=Error[0];            //
//					}
//				}
//			}
//			
//		}
//	
//
//	}
//	testtemp=temp;
//	dst_ref=NorOutput-NorOutput2*testtemp*0.01;
//}
uint16_t pulseprocess(uint16_t p)
{
  
  float result=0;
  uint8_t n=0;
  static uint8_t flag5s=0;
  flag5s++;
  
  for(n=0;n<4;n++)
  {
    pulsebuff[n]=pulsebuff[n+1];
  }
  pulsebuff[4]=p;
  if(pulsebuff[0]!=0&&pulsebuff[1]!=0&&pulsebuff[2]!=0&&pulsebuff[3]!=0)
  {
    if(flag5s>=2)
    {
      flag5s=0;
      result=(pulsebuff[0]+pulsebuff[1]+pulsebuff[2]+pulsebuff[3]+pulsebuff[4])/5;
//      if(pulsebuff[0]>=pulsebuff[1]&&pulsebuff[1]>=pulsebuff[2]&&pulsebuff[2]>=pulsebuff[3]&&pulsebuff[3]>=pulsebuff[4])
//      {
//        result=pulsebuff[2];
//      }
//      if(pulsebuff[0]<=pulsebuff[1]&&pulsebuff[1]<=pulsebuff[2]&&pulsebuff[2]<=pulsebuff[3]&&pulsebuff[3]<=pulsebuff[4])
//      {
//        result=pulsebuff[2];
//      }
    }
    else
      result=Pulse;
    
  }
  else
  {
    result=pulsebuff[4];
  }
   if((result-(int32_t)result)>=0.5)
    {result=(int32_t)result+1;}
  else
    {result=(int32_t)result;}
  return result;
  
}
uint16_t spo2process(uint16_t p)
{
  
  float result=0;
  uint8_t n=0;
  static uint8_t flag5s=0;
  float Error1=0,sum=0;
  flag5s++;
  //return p;
  
  
  SpO2count++;
  if(SpO2count>=5)
  {
    SpO2count=5;
  }
  
  for(n=0;n<9;n++)
  {
    spo2buff[n]=spo2buff[n+1];
  }
  spo2buff[9]=p;
  //if(SpO2count>=2)
  {
    for(n=(10-SpO2count);n<10;n++)
    {
      sum=sum+spo2buff[n];//(spo2buff[5]+spo2buff[6]+spo2buff[7]+spo2buff[8]+spo2buff[9])/5;
    }
    sum=sum/SpO2count;
    for(n=(10-SpO2count);n<10;n++)
    {
      Error1=Error1+abs(sum-spo2buff[n]);
    }
    Error1=Error1/SpO2count;
  //  if(spo2buff[0]!=0&&spo2buff[1]!=0&&spo2buff[2]!=0&&spo2buff[3]!=0)
  //  {
  //    if(Error1<=3)
  //    {
  //      //flag5s=0;
  //      
  //      result=(spo2buff[5]+spo2buff[6]+spo2buff[7]+spo2buff[8]+spo2buff[9])/5;
  ////      if(spo2buff[0]>=spo2buff[1]&&spo2buff[1]>=spo2buff[2]&&spo2buff[2]>=spo2buff[3]&&spo2buff[3]>=spo2buff[4])
  ////      {
  ////        result=spo2buff[2];
  ////      }
  ////      if(spo2buff[0]<=spo2buff[1]&&spo2buff[1]<=spo2buff[2]&&spo2buff[2]<=spo2buff[3]&&spo2buff[3]<=spo2buff[4])
  ////      {
  ////        result=spo2buff[2];
  ////      }
  //    }
  //    else 
  //      result=SpO2h;
  //    
  //  }
  //  else
  //  {
  //    result=spo2buff[9];
  //  }
    //result=spo2buff[4];
    if(Error1<=3)
    {
      result=sum;
//      if(result>=74||result<=66)
//      {
//        n=0;
//      }
    }
    else 
    {
      //return SpO2h;
      result=SpO2h;
    }
  }
//  else 
//  {
//    return SpO2h;
//  }
//  if(SpO2count<=1)
//  {
//    kalman1_init(SpO2state, (float)result, 1);
//  }
//  else
//  {
//    result=kalman1_filter(SpO2state, (float)result);
//  }
  if((result-(int32_t)result)>=0.5)
    {result=(int32_t)result+1;}
  else
    {result=(int32_t)result;}
  return result;
}
uint8_t getnormal_PR(int index)  //获取归一化脉搏波+二阶
{
  static int buff[200]={0};//用于归一化波形处理的数据
  static int max=0,min=0,delta=0,Nordelta=0;
  static int test=0;
  int result=0;
  uint8_t i=0,n=0;
  for(n=0;n<199;n++)
  {
    buff[n]=buff[n+1];
  }
  buff[199]=index;//-Output2old)*10;
  max=buff[199];
  min=buff[199];
  for(n=198;n>1;n--)
  {
    if(buff[n]==0&&buff[n-1]==0) break;
    if(buff[n]>max&&buff[n]>=buff[n-1]&&buff[n]>buff[n+1])
    {
      max=buff[n];
      
    }
    if(buff[n]<min&&buff[n]<=buff[n-1]&&buff[n]<buff[n+1])
    {
      min=buff[n];
    }
  }
  //MaxOutput2=max;
  delta=max-min;
  isFake=false;
  if(delta<500)
  {
   isFake=true;
  }
  Nordelta=(max-index);
  if(Nordelta<=0)
    Nordelta=0;
  //if(Nordelta>=delta)Nordelta=delta;
  if(delta>100000)
    result=127*(float)(Nordelta)/(float)(((delta)));

  if(delta<5000&&delta>500)
    result=0.4*127*(float)(Nordelta)/(float)(((delta)));
  if(delta>=5000&&delta<=100000)
  {
    result=(0.4+(float)(delta/1000-5)/160)*127*(float)(Nordelta)/(float)(((delta)));
  }
  
  if(result>127)
  {
    if(Nordelta>delta)
    {
      Nordelta=Nordelta;
    }
  }
  
  result=(result>127)?127:result;
  if(abs(result-NormalRatioOld)>=80)
  {
    return NormalRatioOld;
  }
  else 
  {
    NormalRatioOld=result;
  }
  if(result<0)
  {
    result=0;
  }
//  test++;
//  if(test>127)test=0;
//  result=test;
  return result;
  
}
void get2sec_Error(int index)
{
  
}
void select_window(int32_t index)
{
  uint16_t n=0;
  static uint16_t time=0;
  time++;
  
  for(n=0;n<FFTLEN-1;n++)
  {
    fftbuff[n].real=fftbuff[n+1].real;
    fftbuff[n].imag=0;
  }
  fftbuff[FFTLEN-1].real=((float)index)/10000;
  fftbuff[FFTLEN-1].imag=0;
  if(time==FFTLEN)
  {
    
    fft(FFTLEN,fftbuff);
    time=0;
    for(n=0;n<FFTLEN;n++)
    {
      Amfft[n]=fftbuff[n].real*fftbuff[n].real+fftbuff[n].imag*fftbuff[n].imag;
    }
    MaxwindowAmp=1;
    for(n=1;n<FFTLEN;n++)//排除低灌注基线漂移影响
    {
      
      if(Amfft[n]>Amfft[1])
      {
        Amfft[1]=Amfft[n];
        MaxwindowAmp=n;
      }
    }
    
  }
  
}
/***************************
*归一化协方差
*
************************/
double RoXY(int x[],int y[],uint8_t n)
{
   int i;
   double averx,avery,sumx=0,sumy=0;
   double sum=0,sumDx=0,sumDy=0;
   double p,Dx,Dy;
   for(i=0;i<n;i++)
   {
      sumx+=x[i];
      sumy+=y[i];
   }
   averx=sumx/n;
   avery=sumy/n;
   for(i=0;i<n;i++)
   {
     sum+=(x[i]-averx)*(y[i]-avery);
   }
   p=sum/n;
   
   for(i=0;i<n;i++)
   {
    sumDx+=(x[i]-averx)*(x[i]-averx);
    sumDy+=(y[i]-avery)*(y[i]-avery);
   }
   Dx=sqrt(sumDx/n);
   Dy=sqrt(sumDy/n);
   return p/(Dx*Dy);
}
/*********************************************************
*kalman filter for SpO2
*
*////////////////////////////////////////////////////////
void kalman1_init(kalman1_state state, float init_x, float init_p)
{    
    SpO2state.x = init_x;
    SpO2state.p = init_p;
    SpO2state.A = 1;
    SpO2state.H = 1;
    SpO2state.q = 0.01;//10e-6;  /* predict noise convariance */
    SpO2state.r = 0.3;//10e-5;  /* measure error convariance */
}

/*
 * @brief   
 *   1 Dimension Kalman filter
 * @inputs  
 *   state - Klaman filter structure
 *   z_measure - Measure value
 * @outputs 
 * @retval  
 *   Estimated result
 */
float kalman1_filter(kalman1_state state, float z_measure)
{
    /* Predict */
    SpO2state.x = SpO2state.A * SpO2state.x;
    SpO2state.p = SpO2state.A * SpO2state.A * SpO2state.p + SpO2state.q;  /* p(n|n-1)=A^2*p(n-1|n-1)+q */

    /* Measurement */
    SpO2state.gain = SpO2state.p * SpO2state.H / (SpO2state.p * SpO2state.H * SpO2state.H + SpO2state.r);
    SpO2state.x = SpO2state.x + SpO2state.gain * (z_measure - SpO2state.H * SpO2state.x);
    SpO2state.p = (1 - state.gain * SpO2state.H) * SpO2state.p;

    return SpO2state.x;
}
