//
// Created by raposo on 5/31/22.
//

#ifndef WAVELETS_FQMF16AS_H
#define WAVELETS_FQMF16AS_H

int subsynt(double *pIMG[],
             int xsize,
             int ysize,
             int xLsrc,
             int yLsrc,
             int xHsrc,
             int yHsrc,
             int xdst,
             int ydst,
             int h_v);

#define MAXDIM   896 /* maximum dimension in either horizontal or vertical */
/* direction that the function will handle */
#define LLENGTH  16   /* length of low band filter */
#define HLENGTH  16  /* length of high band filter */
#define LNORM    0.5     /* normalization constant of the low pass filter */
#define HNORM    0.5 /* normalization constant of the high pass filter */
#define ADVL     7   /* phase advance of the low pass filter in order to */
/* compensate for the delay of the subband */
/* analysis/synthesis process */
#define ADVH     7   /* phase advance of the high pass filter in order to */
/* compensate for the delay of the subband */
/* analysis/synthesis process */

#endif //WAVELETS_FQMF16AS_H
