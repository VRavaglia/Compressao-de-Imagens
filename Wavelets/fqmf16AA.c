
/* This definition will be used throughout the program: */
/* #define mod(x,y)   ( x>=0 ? x%y : (y+x%y)%y )          */
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


#include "fqmf16AA.h"
#include <math.h>

/* THIS FUNCTION PERFORMS THE SUBBAND ANALYSIS USING BOTH THE CASE (iii) AND */
/* CASE (iv) FILTERS */
/* the input signal must be extended with type B */

#define mod(x, y)   ( x>=0 ? x%y : (y+x%y)%y )

/* P is the extended signal period, and must be set equal to 2N */

#define ext(i, N, P)   (mod(i,P)<N ? mod(i,P) : P - 1 - mod(i,P))


/* IMPORTANT! the matrixes passed as parameters must be of type double */


int subanal(double *pIMG[],
        int xsize,
        int ysize,
        int xsrc,
        int ysrc,
        int xLdst,
        int yLdst,
        int xHdst,
        int yHdst,
        int h_v){

/* pIMG   : pointer to the lines of the image */
/* xsize  : length of the band to be decomposed in the horizontal */
/* ysize  : length of the band to be decomposed in the vertical */
/* xsrc   : x position of upper left corner of the band to be decomposed */
/* ysrc   : y position of upper left corner of the band to be decomposed */
/* xLdst  : x position of upper left corner of the low band */
/* yLdst  : y position of upper left corner of the low band */
/* xHdst  : x position of upper left corner of the high band */
/* yHdst  : y position of upper left corner of the high band */
/* h_v     : h_v = 0 : horizontal direction; h_v = 1 : vertical direction */

static double h0[LLENGTH] = {0.001050167, -0.005054526, -0.002589756, 0.0276414, -0.009666376, -0.09039223, 0.09779817,
                             0.4810284, 0.4810284, 0.09779817, -0.09039223, -0.009666376, 0.0276414, -0.002589756,
                             -0.005054526, 0.001050167};
/* these coefficients */
/* must be normalized */
double hh0[LLENGTH];
static double h1[HLENGTH] = {0.001050167, 0.005054526, -0.002589756, -0.0276414, -0.009666376, 0.09039223, 0.09779817,
                             -0.4810284, 0.4810284, -0.09779817, -0.09039223, 0.009666376, 0.0276414, 0.002589756,
                             -0.005054526, -0.001050167};
/* these coefficients */
/* must be normalized */
double hh1[HLENGTH];

double z[MAXDIM], w0[MAXDIM], w1[MAXDIM];
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
hh0[k] = (h0[k]/LNORM)*M_SQRT2;
}

for (
k = 0;
k<HLENGTH; k++)
{
hh1[k] = (h1[k]/HNORM)*M_SQRT2;
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

for (
s = 0;
s<sdim;
s++)
{
if (h_v == 0) /* lines */
{
/* loads current line in vector z[] */
for (
r = 0;
r<rdim;
r++)
{
z[r] = *(pIMG[s+ysrc]+r+xsrc);
}
}

if (h_v == 1) /* columns */
{
/* loads current column in vector z[] */
for (
r = 0;
r<rdim;
r++)
{
z[r] = *(pIMG[r+ysrc]+s+xsrc);
}
}

per = 2 * rdim; /* per set to type B extension */

for (
k = 0;
k<rdim/2; k++)
{
/* computes subsampled output of L filter (w0) */

w0[k] = 0;
for (
m = 0;
m<LLENGTH; m++)
{
/* sets index of summation */
i = 2 * k - m + ADVL;
w0[k] += hh0[m]*z[ext(i, rdim, per)];
}

/* computes subsampled output of H filter (w1) */

w1[k] = 0;
for (
m = 0;
m<HLENGTH; m++)
{
/* sets index of summation */
i = 2 * k - m + ADVH;
w1[k] += hh1[m]*z[ext(i, rdim, per)];
}

}

/* loads filtered line in the output matrixes */
if (h_v == 0) /* horizontal filtering */
{
for (
r = 0;
r<rdim/2;r++)
{
*(pIMG[s+yLdst]+r+xLdst) = (w0[r]);
*(pIMG[s+yHdst]+r+xHdst) = (w1[r]);
}
}

if (h_v == 1) /* vertical filtering */
{
for (
r = 0;
r<rdim/2;r++)
{
*(pIMG[r+yLdst]+s+xLdst) = (w0[r]);
*(pIMG[r+yHdst]+s+xHdst) = (w1[r]);
}
}
}


return(1);

}


