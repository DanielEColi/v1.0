#ifndef _DATATYPE_H__
#define _DATATYPE_H__

/*特别重要：
拟合方法测试:http://www.douban.com/note/76682423/
采用 
type of fit:Fourier 
a0+a1*cos(x*w)+b1*sin(x*w)+a2*cos(2*x*w)+b2*sin(2*x*w)
利用基线调整后的数据，目测效果较好，但是使用fir后的数据，反使波形丢失了形状，可以尝试用多组数据测试，
>> x=load('D:\yihe.txt'); 
>> y=load('D:\y.txt');
>> cftool
*/

#include "stm32f2xx_conf.h"

#define CODE_VERSION	(100)		//版本号:除以100 20/100=V0.2

#define ANIMAL		1

#define WATCH_DOG			0

#define LED_CURRENT_ADJUST	1	//电流调节功能		0:没有电流调节功能，固定电流  1:自动调节电流功能  2:根据病人类型设置电流
#define AMBIENT_CANCELLATION_DAC	0 //环境光取消电流

//2014-04-17 问题点：
//1、最初显示的波形有些偏低，分析是手指刚接上时，有些数据偏小，需要过滤一段数据
//2、脉搏音位置有些不准，（a）从PR计算准确度下手 (b)延时一下波形数据的显示，并判断峰值与后几个数据，由于确定是不是正确的峰值
//3、

/*V0.04
1、快速显示波形与参数计算的问题，尽量控制在5S以内
2、改进INT滤波与FIR滤波后的数据接合问题，要有续结合
3、
*/

/*
考虑用int整系数方法后的数据求波峰波谷值，但是用fir后的数据计算R值
*/

//注意：用整系数数据查找时，注意与fir数据的对应关系
//用1这种方法，只能用于检测心率，统一减去int数量后，而实际数据与fir数据中的索引位置对不上，峰峰值位置有一定偏差，建议通过在int后找出的位置，在fir数据附近重新查找。
//实际上相位发生了变化
#define P2P_FIND_METHOD		0		//1:用整系数处理后的数据查找峰峰值索引，实际还是用fir后的数据计算 0:直接用fir数据进行查找与计算

#define USE_AFE4490_EVM	0		//0:ad_spo2 1:AFE4490 EVM

#define SIMULATION_SPO2	1

#define SAMPLE_100HZ		1	//1:100hz 0:300hz

#define SAVE_ORIGINAL_DATA		0	//是否保存原有数据，用于血氧值计算	不能与FIR_FILTER_ENABLE同时使能

#define FIR_FILTER_ENABLE	1
#define IIR_FILTER_ENABLE	0
#define INT_COEFF_ENABLE	1	//注意，使用整系数法计算后，会影响R值的计算，因为整系数会产生负数的情况

#define LEASTSQUAREMETHOD_ENABLE	0	//最小二乘法

#define FILTER_ERROR_P2P	0	//过滤错误的峰值，采用间隔检测法
#define THRESHOLD_ERROR_P2P	0	//滤值判断法检测峰值是否正确  ？？基线漂移时方法是否不对？建议使用基线漂移调整后的波形做阀值判断

#define SPO2_METHOD_AREA	0 //0:峰峰值法 1:面积法
#define SPO2_AVER_AREA		1	//1：平均求面积法 0:最优面积求法，主要是判断基线的偏移程序，偏移越小即为最优


//0:从最小范围6找到最大范围150 
//1:印总提供的方法，先用6个点找出所有大小值，如果大于9个值，取之前1/3，求出最大值与最小值，用平均差的方法去掉可能错的值,
//小于最大值到最小值的80%比例，直接扔掉，再判断间距。


#if(SIMULATION_SPO2==1)
#define SIMULATION_SPO2_TYPE	1		//0:ads8328 1:afe4490
#endif

//AFE4490 AD采样，间隔一数数据就有漂动,是由于50hz工频干扰

#define TRANSMISSION_ORIGINAL_DATA	1	//传输原始采样数据


#define DIGITAL_SPO2		0
#define BCI_PROTOCOL_DATA		1	//0:原始数据 1:滤波后数据 

#define DIGITAL_OXI_IDX			0		//数字血氧全用下标0
#define SIMULATION_OXI_IDX	1		//模拟血氧全用下标1

#define IR_IDX	0				//红外数据全用下标0
#define R_IDX		1				//红光数据全用下标1

#if(ANIMAL==1)
#define PR_MAX				400		//脉率最大值
#define PR_MIN				25		//脉率最小值
#else
#define PR_MAX				250		//脉率最大值
#define PR_MIN				25		//脉率最小值
#endif

#define SPO2_MAX	100	//血氧最大值
#define SPO2_MIN	40		//血氧最小值


/*
Ti John:
需要注意，当DC值太小时，需提高增益，反之，需降低增益。将DC值调到一定范围内再进行血氧值计算。

50Hz工频干扰，建议模拟仪和评估板以及电脑都采用电池供电，与电网完全隔离，可降低工频干扰；
为进一步降低噪声，建议您采用多次采样平均的方式（4490就有这项功能），建议每次采样至少取8次平均。
另外，需仔细配置输入放大器的时间常数，即RF*CF值。假定你的LED点亮时间是TON，那么，RF*CF值应小于且接近于0.1*Ton。
此外，每个采样电路接通的时间应比相应LED ON的时间至少延迟20us左右。LED点亮的时间不必太长。
例如，检定选100Hz采样频率，可尝试LED点亮时间420us（即4.2%占空比），每次采8个样取平均（需400us）。关闭第二级放大，只开第一级放大，调节RF值，使DC值在1,000,000左右。

如果追求高性能，需要能够测量很弱的信号，建议采用比较大的LED电流，比较小的放大器增益。
通常只用第一级TIA放大器即可，旁路第二级，不用Ambient-Cancellation DAC，只用数字方式的环境光消减；
若追求低功耗，可采用较低的LED电流，较高的放大器增益，此时环境光的数值会比较大，为节省ADC动态范围可使用模拟方式的环境光消减，即使用Ambient-Cancellation DAC来消减环境光。

Mike 总结:
特别是在弱灌注的情况下，大电流，小增益，测量效果更佳,任何时候都建议按这种方法进行

疑问点：目前这里我们R1使用158K，电压3.3V左右，而我们的VHR是V1.6，VLED,VCable是多少？
LED_DRV_SUP [3.0 or (VHR + VLED + VCABLE) whichever is greater] to 5.25
调整电流或者是增益之后，如何辨别是否是最佳电流或者是增益

诊断使能时，ADC值是无效的。需要考虑何时做诊断判断

Mike:想法
建议在检测到手指脱落后，定时检测探头是否没有连接
Enable_Diagnostic(1);

//E2E Praveen.
PI in percent = ( AC / DC ) * 100

*/

#endif

