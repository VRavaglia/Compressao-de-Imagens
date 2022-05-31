

/* This definition will be used throughout the program: */
/* #define mod(x,y) = ( x>=0 ? x%y : (y+x%y)%y )          */
/* it is equivalent to the mathematical modulus operation */

/* There are 4 types of symmetric extensions used by the subband programs */
/* We are supposing that N is the length of the signal to be filtered */

/* type A : odd about both 0 and N-1 */
/*          in this case, the extended signal has period PA = 2N-2 and the */
/* indices are: extA(i,N) = (mod(i,PA)<N ? mod(i,PA) : PA - mod(i,PA)) */

/* type B : even about both 0 and N-1 */
/*          in this case, the extended signal has period PB = 2N and the */
/* indices are: extB(i,N) = (mod(i,PB)<N ? mod(i,PB) : PB - 1 - mod(i,PB))*/

/* type C : odd about 0 and even about N-1 */
/*          in this case, the extended signal has period PCD = 2N-1 and the */
/* indices are: extC(i,N) = (mod(i,PCD)<N ? mod(i,PCD) : PCD - mod(i,PCD)) */

/* type D : even about 0 and odd about N-1 */
/*          in this case, the extended signal has period PCD = 2N-1 and the */
/* indices are: extD(i,N) = (mod(i,PCD)<N ? mod(i,PCD) : PCD - 1 - mod(i,PCD))*/

/* The overall delay of a perfect reconstruction subband analysis/synthesis */
/* will be always odd. As it is the sum of the individual delays of the */
/* analysis and synthesis filters, there will be four cases to consider */
/* (x will denote the input signal and w the corresponding subband) */

/* (i)   analysis filter with even delay synthesis filter with odd delay */
/*       x : type A;  w : type C */

/* (ii)  analysis filter with odd delay synthesis filter with even delay */
/*       x : type A;  w : type D */

/* (iii) analysis and synthesis filters with even-1/2 delay */
/*       x : type B;  w : type B */

/* (iv)  analysis and synthesis filters with odd-1/2 delay */
/*       x : type B;  w : type A */

/* OBS: if the low band is case (i)    the high band will be case (ii)  */
/* OBS: if the low band is case (ii)   the high band will be case (i)  */
/*      if the low band is case (iii)  the high band will be case (iii) */
/*      if the low band is case (iv)   the high band will be case (iv)  */


#include "fqmf16AS.h"
#include <math.h>

/* THIS FUNCTION PERFORMS THE SUBBAND SYNTHESIS USING THE CASE (iii) FOR BOTH */
/* THE LOW AND HIGHPASS FILTERS */
/* both bands must be extended with type B */

#define mod(x, y)   ( x>=0 ? x%y : (y+x%y)%y )

/* P is the extended signal period, and must be set equal to 2N */

#define ext(i, N, P)   (mod(i,P)<N ? mod(i,P) : P - 1 - mod(i,P))

#define sgn(i, N, P)   (mod(i,P)<N ? 1 : -1)


#include <math.h>
/* This function performs one step of the subband synthesis of an image */

/* IMPORTANT! the matrixes passed as parameters must be of type int */

int subsynt(double *pIMG[],
             int xsize,
             int ysize,
             int xLsrc,
             int yLsrc,
             int xHsrc,
             int yHsrc,
             int xdst,
             int ydst,
             int h_v){

/* pIMG   : pointer to the lines of the image */
/* xsize  : length of the source bands in the horizontal */
/* ysize  : length of the source bands in the vertical */
/* xLsrc  : x position of upper left corner of the low band */
/* yLsrc  : y position of upper left corner of the low band */
/* xHsrc  : x position of upper left corner of the high band */
/* yHsrc  : y position of upper left corner of the high band */
/* xdst   : x position of upper left corner of the band to be decomposed */
/* ydst   : y position of upper left corner of the band to be decomposed */
/* h_v     : h_v = 0 : horizontal direction; h_v = 1 : vertical direction */


//double *pIMG[];
//int xLsrc, yLsrc, xHsrc, yHsrc, xsize, ysize, xdst, ydst;
//int h_v;


static double g0[LLENGTH] = {0.001050167, -0.005054526, -0.002589756, 0.0276414, -0.009666376, -0.09039223, 0.09779817,
                             0.4810284, 0.4810284, 0.09779817, -0.09039223, -0.009666376, 0.0276414, -0.002589756,
                             -0.005054526, 0.001050167};
/* these coefficients */
/* must be divided by 2 */
double gg0[LLENGTH];
static double g1[HLENGTH] = {-0.001050167, -0.005054526, 0.002589756, 0.0276414, 0.009666376, -0.09039223, -0.09779817,
                             0.4810284, -0.4810284, 0.09779817, 0.09039223, -0.009666376, -0.0276414, -0.002589756,
                             0.005054526, 0.001050167};
/* these coefficients */
/* must be divided by 8 */
double gg1[HLENGTH];

double z0[MAXDIM], z1[MAXDIM], w0[MAXDIM], w1[MAXDIM];
/* hold the temporary input and output of */
/* the one-dimensional filters */

int per; /* holds the period of the symmetric extension */

int k, m, i, j; /* indices */

int r, s; /* indices that go through rows and columns, alternately */
int rdim, sdim; /* bounds for variation of r and s , respectively */
/* the filtering will always be done on sdim */
/* unidimensional vectors of length rdim */
int a, b; /* limits of summations */



/* normalizes filters */
for (
k = 0;
k<LLENGTH; k++)
{
gg0[k] = g0[k]/(LNORM*M_SQRT2);
}

for (
k = 0;
k<HLENGTH; k++)
{
gg1[k] = g1[k]/(HNORM*M_SQRT2);
}


/* go through lines or columns */
if (h_v == 0)
{
rdim = xsize;
sdim = ysize;
}
else
{
rdim = ysize;
sdim = xsize;
}

per = 2 * rdim; /* per set to type B extension */

for (
s = 0;
s<sdim;
s++)
{
if (h_v == 0) /* lines */
{
/* loads the two band lines in the vectors w0[] and w1[] */
for (
r = 0;
r<rdim;
r++)
{
w0[r] = *(pIMG[s+yLsrc]+r+xLsrc);
w1[r] = *(pIMG[s+yHsrc]+r+xHsrc);
}
}

if (h_v == 1) /* columns */
{
/* loads the two band columns in the vectors w0[] and w1[] */
for (
r = 0;
r<rdim;
r++)
{
w0[r] = *(pIMG[r+yLsrc]+s+xLsrc);
w1[r] = *(pIMG[r+yHsrc]+s+xHsrc);
}
}


for (
k = 0;
k<rdim;
k++)
{
/* computes even outputs of L filter (z0) */

z0[2*k] = 0;
for (
m = 0;
m<=(LLENGTH-2)/2; m++)
{
i = k - m + (ADVL / 2);
z0[2*k] += gg0[2*m+1]*w0[ext(i, rdim, per)];
}


/* computes even outputs of H filter (z1) */

z1[2*k] = 0;
for (
m = 0;
m<=(HLENGTH-2)/2; m++)
{
i = k - m + (ADVH / 2);
z1[2*k] += gg1[2*m+1]*sgn(i, rdim, per)*w1[ext(i, rdim, per)];
}

/* computes odd outputs of L filter (z0) */

z0[2*k+1] = 0;
for (
m = 0;
m<=(LLENGTH-1)/2; m++)
{
i = k - m + (ADVL / 2) + 1;
z0[2*k+1] += gg0[2*m]*w0[ext(i, rdim, per)];
}

/* computes odd outputs of H filter (z1) */

z1[2*k+1] = 0;
for (
m = 0;
m<=(HLENGTH-1)/2; m++)
{
i = k - m + (ADVH / 2) + 1;
z1[2*k+1] += gg1[2*m]*sgn(i, rdim, per)*w1[ext(i, rdim, per)];
}

}


/* loads sum of filtered lines in the output matrix */
if (h_v == 0) /* horizontal filtering */
{
for (
r = 0;
r<2*
rdim;
r++)
{
*(pIMG[s+ydst]+r+xdst) = (z0[r]) + (z1[r]);
}
}

if (h_v == 1) /* vertical filtering */
{
for (
r = 0;
r<2*
rdim;
r++)
{
*(pIMG[r+ydst]+s+xdst) = (z0[r]) + (z1[r]);
}
}
}


return(1);

}


