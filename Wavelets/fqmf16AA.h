//
// Created by raposo on 5/31/22.
//

#ifndef WAVELETS_FQMF16AA_H
#define WAVELETS_FQMF16AA_H

int subanal(double *pIMG[],
        int xsize,
        int ysize,
        int xsrc,
        int ysrc,
        int xLdst,
        int yLdst,
        int xHdst,
        int yHdst,
        int h_v);

#define MAXDIM   896 /* maximum dimension in either horizontal or vertical */
/* direction that the function will handle */
#define LLENGTH  16  /* length of low band filter */
#define HLENGTH  16   /* length of high band filter */
#define LNORM    1.0 /* normalization constant of the low pass filter */
#define HNORM    1.0     /* normalization constant of the high pass filter */
#define ADVL     8   /* phase advance of the low pass filter in order to */
/* compensate for the delay of the subband */
/* analysis/synthesis process */
#define ADVH     8   /* phase advance of the high pass filter in order to */
/* compensate for the delay of the subband */
/* analysis/synthesis process */

#endif //WAVELETS_FQMF16AA_H
