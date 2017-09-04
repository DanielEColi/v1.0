#include "leastsquaremethod.h"
#include <math.h>
#include <stdlib.h>

/**************************************/
//��С���˷�

//�Ѷ�ά��������һά�����ת����Ҳ����ֱ���ö�ά���飬ֻ���ҵ�ϰ���ǲ��ö�ά����
 #define ParaBuffer(Buffer,Row,Col) (*(Buffer + (Row) * (SizeSrc + 1) + (Col)))

/***********************************************************************************
 ��ӡϵ������ֻ���ڵ��ԣ����߱����㹦��
 ����һ��N����ϣ�����ϵ�������С�ǣ�N + 1���У�N + 2����
 float* Para��ϵ������洢��ַ
 int SizeSrc��ϵ�������С��SizeSrc���У�SizeSrc + 1����
 ***********************************************************************************/
 static int PrintPara(float* Para, int SizeSrc)
 {
 /*        int i, j;
         for (i = 0; i < SizeSrc; i++)
         {
                 for (j = 0; j <= SizeSrc; j++)
                         TRACE("%10.6lf ", ParaBuffer(Para, i, j));
                 TRACE("\r\n");
         }
         TRACE("\r\n");*/
         return 0;
 }
 

/***********************************************************************************
 ϵ��������޷�������ֹ�������Ŀǰ��������ܲ����ƣ������ܺܺõؽ���������
 ԭ�����������ʽ��ͬһ�г���һ��ϵ��������ʽ�Ľⲻ��
 ��Ȼ�����������⣬����һ���������⣬Ҳ��ͬ����˼·�����ڶ���������Ĵ���ܲ����ƣ��д��Ż�
 ����Ϊ��λ����
 ***********************************************************************************/
 static int ParalimitRow(float* Para, int SizeSrc, int Row)
 {
         int i;
         float Max, Min, Temp;
         for (Max = abs(ParaBuffer(Para, Row, 0)), Min = Max, i = SizeSrc; i; i--)
         {
                 Temp = abs(ParaBuffer(Para, Row, i));
                 if (Max < Temp)
                         Max = Temp;
                 if (Min > Temp)
                         Min = Temp;
         }
         Max = (Max + Min) * 0.000005;
         for (i = SizeSrc; i >= 0; i--)
                 ParaBuffer(Para, Row, i) /= Max;
         return 0;
 }
 
/***********************************************************************************
 ͬ�ϣ��Ծ���Ϊ��λ����
 ***********************************************************************************/
 static int Paralimit(float* Para, int SizeSrc)
 {
         int i;
         for (i = 0; i < SizeSrc; i++)
                 if (ParalimitRow(Para, SizeSrc, i))
                         return -1;
         return 0;
 }
 
/***********************************************************************************
 ϵ����������ʽ�任
 ***********************************************************************************/
 static int ParaPreDealA(float* Para, int SizeSrc, int Size)
 {
         int i, j;
         for (Size -= 1, i = 0; i < Size; i++)
         {
                 for (j = 0; j < Size; j++)
                         ParaBuffer(Para, i, j) = ParaBuffer(Para, i, j) * ParaBuffer(Para, Size, Size) - ParaBuffer(Para, Size, j) * ParaBuffer(Para, i, Size);
                 ParaBuffer(Para, i, SizeSrc) = ParaBuffer(Para, i, SizeSrc) * ParaBuffer(Para, Size, Size) - ParaBuffer(Para, Size, SizeSrc) * ParaBuffer(Para, i, Size);
                 ParaBuffer(Para, i, Size) = 0;
                 ParalimitRow(Para, SizeSrc, i);
         }
         return 0;
 }
 
/***********************************************************************************
 ϵ����������ʽ�任����ParaPreDealA���
 ��ɵ�һ�α任���任�����Ǿ���
 ***********************************************************************************/
 static int ParaDealA(float* Para, int SizeSrc)
 {
         int i;
         for (i = SizeSrc; i; i--)
                 if (ParaPreDealA(Para, SizeSrc, i))
                         return -1;
         return 0;
 }
 
/***********************************************************************************
 ϵ����������ʽ�任
 ***********************************************************************************/
 static int ParaPreDealB(float* Para, int SizeSrc, int OffSet)
 {
         int i, j;
         for (i = OffSet + 1; i < SizeSrc; i++)
         {
                 for (j = OffSet + 1; j <= i; j++)
                         ParaBuffer(Para, i, j) *= ParaBuffer(Para, OffSet, OffSet);
                 ParaBuffer(Para, i, SizeSrc) = ParaBuffer(Para, i, SizeSrc) * ParaBuffer(Para, OffSet, OffSet) - ParaBuffer(Para, i, OffSet) * ParaBuffer(Para, OffSet, SizeSrc);
                 ParaBuffer(Para, i, OffSet) = 0;
                 ParalimitRow(Para, SizeSrc, i);
         }
         return 0;
 }
 
/***********************************************************************************
 ϵ����������ʽ�任����ParaPreDealB���
 ��ɵ�һ�α任���任�ɶԽǾ��󣬱任���
 ***********************************************************************************/
 static int ParaDealB(float* Para, int SizeSrc)
 {
         int i;
         for (i = 0; i < SizeSrc; i++)
                 if (ParaPreDealB(Para, SizeSrc, i))
                         return -1;
         for (i = 0; i < SizeSrc; i++)
         {
                 if (ParaBuffer(Para, i, i))
                 {
                         ParaBuffer(Para, i, SizeSrc) /= ParaBuffer(Para, i, i);
                         ParaBuffer(Para, i, i) = 1.0;
                 }
         }
         return 0;
 }
 
/***********************************************************************************
 ϵ������任
 ***********************************************************************************/
 static int ParaDeal(float* Para, int SizeSrc)
 {
         PrintPara(Para, SizeSrc);
         Paralimit(Para, SizeSrc);
         PrintPara(Para, SizeSrc);
         if (ParaDealA(Para, SizeSrc))
                 return -1;
         PrintPara(Para, SizeSrc);
         if (ParaDealB(Para, SizeSrc))
                 return -1;
         return 0;
 }
 
/***********************************************************************************
 ��С���˷��ĵ�һ�����Ǵ�XY���������ȡϵ������
 float* Para��ϵ�������ַ
 const float* X��X���ݵ�ַ
 const float* Y��Y���ݵ�ַ
 int Amount��XY��������
 int SizeSrc��ϵ�������С��SizeSrc���У�SizeSrc + 1����
 ***********************************************************************************/
 static int GetParaBuffer(float* Para, const float* X, const float* Y, int Amount, int SizeSrc)
 {
         int i, j;
         for (i = 0; i < SizeSrc; i++)
         {
					 for(ParaBuffer(Para, 0, i) = 0, j = 0; j < Amount; j++)
                         ParaBuffer(Para, 0, i) += pow(*(X + j), 2 * (SizeSrc - 1) - i);
				 }
         for (i = 1; i < SizeSrc; i++)
         {
					 for (ParaBuffer(Para, i, SizeSrc - 1) = 0, j = 0; j < Amount; j++)
                         ParaBuffer(Para, i, SizeSrc - 1) += pow(*(X + j), SizeSrc - 1 - i);
				 }
         for (i = 0; i < SizeSrc; i++)
         {
					 for (ParaBuffer(Para, i, SizeSrc) = 0, j = 0; j < Amount; j++)
                         ParaBuffer(Para, i, SizeSrc) += (*(Y + j)) * pow(*(X + j), SizeSrc - 1 - i);
				 }
         for (i = 1; i < SizeSrc; i++)
         {
					 for (j = 0; j < SizeSrc - 1; j++)
                         ParaBuffer(Para, i, j) = ParaBuffer(Para, i - 1, j + 1);
				 }
         return 0;
 }
 
/***********************************************************************************
 �����������
 ***********************************************************************************/
int Cal(const float* BufferX, const float* BufferY, int Amount, int SizeSrc, float* ParaResK)
{
	float* ParaK = (float*)malloc(SizeSrc * (SizeSrc + 1) * sizeof(float));
  GetParaBuffer(ParaK, BufferX, BufferY, Amount, SizeSrc);
  ParaDeal(ParaK, SizeSrc);
  for(Amount=0;Amount<SizeSrc; Amount++, ParaResK++)
	{
		*ParaResK = ParaBuffer(ParaK, Amount, SizeSrc);
	}
  free(ParaK);
  return 0;
}

/***********************************************************************************
 ����main����
 ����������20
 ������5
 ***********************************************************************************/
 int TestLeastSquareMethod(float* BufferX,float* BufferY,int Amount)
 {
//��������
//XY���棬ϵ�����󻺴�
	float ParaK[3];
	int count=sizeof(ParaK) / sizeof(float);
	float v=0.0;
	int i;
//����
  Cal((const float*)BufferX,(const float*)BufferY,Amount,sizeof(ParaK)/sizeof(float),(float*)ParaK);
//���ϵ��
/*        for (Amount = 0; Amount < sizeof(ParaK) / sizeof(float); Amount++)
		 {
			 TRACE("ParaK[%d] = %lf!\r\n", Amount, ParaK[Amount]);
		 }*/

	for( i=0;i<sizeof(ParaK)/sizeof(float);i++)
	{
		v+=ParaK[i]*pow((float)Amount,count-i);
	}
			 //v=ParaK[0]*pow((float)Amount,(int)5)+ParaK[1]*pow((float)Amount,(int)4)+ParaK[2]*pow((float)Amount,(int)3)+ParaK[3]*pow((float)Amount,(int)2)+ParaK[4]*pow((float)Amount,(int)1)+ParaK[5];
			 //v=ParaK[5]*pow((float)Amount,(int)5)+ParaK[4]*pow((float)Amount,(int)4)+ParaK[3]*pow((float)Amount,(int)3)+ParaK[2]*pow((float)Amount,(int)2)+ParaK[1]*pow((float)Amount,(int)1)+ParaK[0];			 
  return v;
}
