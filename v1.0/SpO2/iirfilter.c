#include "iirfilter.h"
#include <string.h>

#if(IIR_FILTER_ENABLE==1)	

#define IIR_NSEC 21

const float IIR_B[IIR_NSEC][3] = {
  {
     0.9560650587,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.9154215455,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.8794227242,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.8481743932,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.8216649294,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.7998158932,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.7825190425,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.7696630955,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.7611513734,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
     0.7569138408,              0,              0 
  },
  {
                1,             -2,              1 
  },
  {
                1,              0,              0 
  }
};

const float IIR_A[IIR_NSEC][3] = {
  {
                1,              0,              0 
  },
  {
                1,   -1.869212389,   0.9550478458 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.789749742,   0.8719363213 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.719368219,   0.7983226776 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.658274293,   0.7344233394 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.606445432,   0.6802144051 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.563728094,   0.6355354786 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.529910803,   0.6001653671 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.504776001,   0.5738763213 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.488134623,   0.5564708114 
  },
  {
                1,              0,              0 
  },
  {
                1,   -1.479849815,   0.5478054881 
  },
  {
                1,              0,              0 
  }
};

static double y[2][IIR_NSEC][3];
static double x[2][IIR_NSEC+1][3];

//type:0-ir 1-r
u32 IIRFilter(u32 in,u8 type) 
{ 
	u16 i; 
  x[type][0][0] = in; 
  for(i=0;i<IIR_NSEC;i++) 
  { 
    //  y[i][0] = x[i][0]*IIR_B[i][0] +x[i][1]*IIR_B[i][1] +x[i][2]*IIR_B[i][2]-y[i][1]*IIR_A[i][1]-y[i][2]*IIR_A[i][2]; 
    y[type][i][0] = 0; 
		
		y[type][i][0] += x[type][i][0]*IIR_B[i][0]; 
		y[type][i][0] += x[type][i][1]*IIR_B[i][1]; 
		y[type][i][0] += x[type][i][2]*IIR_B[i][2]; 
		y[type][i][0] -= y[type][i][1]*IIR_A[i][1]; 
		y[type][i][0] -= y[type][i][2]*IIR_A[i][2]; 
		y[type][i][0] /= IIR_A[i][0]; 
           
    y[type][i][2]=y[type][i][1];
		y[type][i][1]=y[type][i][0]; 
    x[type][i][2]=x[type][i][1];
		x[type][i][1]=x[type][i][0]; 
           
    x[type][i+1][0] = y[type][i][0]; 
	} 
       
  /*if(x[type][IIR_NSEC][0]>32767)  
	{
		x[type][IIR_NSEC][0]=32767; 
	}
  if(x[type][IIR_NSEC][0]<-32768) 
	{
		x[type][IIR_NSEC][0]=-32768; 
	}*/
  return (x[type][IIR_NSEC][0]);      
} 

//type 0:ir 1:r
void IIRReset(u8 type)
{
	u16 i,j;
  for(i=0;i<IIR_NSEC+1;i++)
  {
		for(j=0;j<3;j++)
    {
			x[type][i][j]=0;
    }
  }    
  for(i=0;i<IIR_NSEC;i++)
  {
		for(j=0;j<3;j++)
    {
			y[type][i][j]=0;
    }
  }
}

#endif
