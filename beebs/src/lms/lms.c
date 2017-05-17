/* MDH WCET BENCHMARK SUITE. */

/* 2012/09/28, Jan Gustafsson <jan.gustafsson@mdh.se>
 * Changes:
 *  - This program redefines the C standard functions log, fabs, sqrt, and sin. Therefore, these function has been renamed with prefix lms_.
 *  - Warning: explicitly assigning a variable of type 'float' to itself
 *  x =x;
 *  removed.
 *  - Unused variables removed.
 */


/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: lms.c                                                          */
/*  SOURCE : C Algorithms for Real-Time DSP by P. M. Embree              */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     An LMS adaptive signal enhancement. The input signal is a sine    */
/*     wave with added white noise.                                      */
/*     The detailed description is in the program source code.           */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/


#include "support.h"

// This scale factor will be changed to equalise the runtime of the
// benchmarks
#define SCALE_FACTOR    (REPEAT_FACTOR >> 4)


#define RAND_MAX 32768
#define PI 3.14159265358979323846


/* function prototypes for fft and filter functions */

static float gaussian(void);

#define N 101
#define L 20            /* filter order, (length L+1) */

/* set convergence parameter */
float mu = 0.01;


int lms_rand()
{
  static unsigned long next = 1;

  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

static float lms_log(float r)
{
  return 4.5;
}

static float lms_fabs(float n)
{
  float f;

  if (n >= 0) f = n;
  else f = -n;
  return f;
}

static float lms_sqrt(float val)
{
  float x = val/10;

  float dx;

  double diff;
  double min_tol = 0.00001;

  int i, flag;

  flag = 0;
  if (val == 0 ) x = 0;
  else {
    for (i=1;i<20;i++)
      {
	if (!flag) {
	  dx = (val - (x*x)) / (2.0 * x);
	  x = x + dx;
	  diff = val - (x*x);
	  if (lms_fabs(diff) <= min_tol) flag = 1;
	}
	else { }         /* JG */
/*	  x =x; */
      }
  }
  return (x);
}


static float lms_sin(float rad)
{
  float app;

  float diff;
  int inc = 1;

  while (rad > 2*PI)
	rad -= 2*PI;
  while (rad < -2*PI)
    rad += 2*PI;
  app = diff = rad;
   diff = (diff * (-(rad*rad))) /
      ((2.0 * inc) * (2.0 * inc + 1.0));
    app = app + diff;
    inc++;
  while(lms_fabs(diff) >= 0.00001) {
    diff = (diff * (-(rad*rad))) /
      ((2.0 * inc) * (2.0 * inc + 1.0));
    app = app + diff;
    inc++;
  }

  return(app);
}

static float gaussian()
{
    static int ready = 0;       /* flag to indicated stored value */
    static float gstore;        /* place to store other value */
    static float rconst1 = (float)(2.0/RAND_MAX);
    static float rconst2 = (float)(RAND_MAX/2.0);
    float v1,v2,r,fac;
    float gaus;

/* make two numbers if none stored */
    if(ready == 0) {
            v1 = (float)lms_rand() - rconst2;
            v2 = (float)lms_rand() - rconst2;
            v1 *= rconst1;
            v2 *= rconst1;
            r = v1*v1 + v2*v2;
        while (r > 1.0f) {
            v1 = (float)lms_rand() - rconst2;
            v2 = (float)lms_rand() - rconst2;
            v1 *= rconst1;
            v2 *= rconst1;
            r = v1*v1 + v2*v2;
        }       /* make radius less than 1 */

/* remap v1 and v2 to two Gaussian numbers */
        fac = lms_sqrt(-2.0f*lms_log(r)/r);
        gstore = v1*fac;        /* store one */
        gaus = v2*fac;          /* return one */
        ready = 1;              /* set ready flag */
    }

    else {
        ready = 0;      /* reset ready flag for next pair */
        gaus = gstore;  /* return the stored one */
    }

    return(gaus);
}

int benchmark()
{
    float lms(float,float,float *,int,float,float);
    static float d[N],b[21];
    float signal_amp,noise_amp,arg,x/*,y*/; /* JG */
    int k;

    /* create signal plus noise */
    signal_amp = lms_sqrt(2.0);
    noise_amp = 0.2*lms_sqrt(12.0);
    arg = 2.0*PI/20.0;
    for(k = 0 ; k < N ; k++) {
        d[k] = signal_amp*lms_sin(arg*k) + noise_amp*gaussian();
    }

    /* scale based on L */
    mu = 2.0*mu/(L+1);

    x = 0.0;
    for(k = 0 ; k < N ; k++) {
        lms(x,d[k],b,L,mu,0.01);
    /* delay x one sample */
        x = d[k];
    }
    return 0;
}


/*
      function lms(x,d,b,l,mu,alpha)

Implements NLMS Algorithm b(k+1)=b(k)+2*mu*e*x(k)/((l+1)*sig)

x      = input data
d      = desired signal
b[0:l] = Adaptive coefficients of lth order fir filter
l      = order of filter (> 1)
mu     = Convergence parameter (0.0 to 1.0)
alpha  = Forgetting factor   sig(k)=alpha*(x(k)**2)+(1-alpha)*sig(k-1)
         (>= 0.0 and < 1.0)

returns the filter output
*/

float lms(float x,float d,float *b,int l,
                  float mu,float alpha)
{
    int ll;
    float e,mu_e,/*lms_const,*/y; /* JG */
    static float px[51];      /* max L = 50 */
    static float sigma = 2.0; /* start at 2 and update internally */

    px[0]=x;

/* calculate filter output */
    y=b[0]*px[0];
#ifdef DEBUG
    printf("l=%d\n",l);
#endif
    for(ll = 1 ; ll <= l ; ll++)
        y=y+b[ll]*px[ll];

/* error signal */
    e=d-y;

/* update sigma */
    sigma=alpha*(px[0]*px[0])+(1-alpha)*sigma;
    mu_e=mu*e/sigma;

/* update coefficients */
    for(ll = 0 ; ll <= l ; ll++)
        b[ll]=b[ll]+mu_e*px[ll];
/* update history */
    for(ll = l ; ll >= 1 ; ll--)
        px[ll]=px[ll-1];

    return(y);
}
