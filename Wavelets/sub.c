
/* This program does multistage subband analysis */

/* it does the subband decomposition for a predetermined number of stages */

/* this file contains all the includes and defines necessary for running the */
/* SUBDCT programs */



//extern double ufpsnr();  /* psnr between two unsigned char images */
//extern double entropy(); /* entropy of a band */

/* global variables which will contain the image dimensions - they MUST be */
/* external to all the routines */

#include "fqmf16AS.h"
#include "fqmf16AA.h"
#include "sub.h"
#include "subdefs2.h"

void sub(int **Image_orig, int **Image_out, int **Image, int xsize, int ysize) {

//    int sub4anal(); /* function that performs two-band subband analysis */
//    int sub4synt(); /* function that performs two-band subband analysis */

    int cont;


    unsigned char *pY[YLUM];
    /* array of pointers to the beginning of each */
    /* line of the image luminance and chrominances */
    /* it is used for reading the images */

    unsigned char *pCY[YLUM];
    /* array of pointers to the beginning of each */
    /* line of the coded image luminance and chrominances */

    unsigned char *pSY[YLUM];
    /* array of pointers to the beginning of each */
    /* line of the subband decomposition luminance and */
    /* chrominances. It is used for writing the coded */
    /* subband decomposition */

    double *pSIMG[YIMG];
    /* array of pointers to the beginning of each line */
    /* of the image subband decomposition */

    short int *pISIMG[YIMG];
    /* array of pointers to the beginning of each line */
    /* of the image subband decomposition - short int */

    unsigned short int head[4]; /* stores header of image files */
    /* head[0] - must store 0xc0de */
    /* head[1] - store width of image */
    /* head[2] - store height of image */
    /* head[3] - store format of image */


    int x, y; /* hold position into images */

    int u, v; /* hold block indices */

    int umin, umax, vmin, vmax; /* boudaries of block indexes of subbands */

    int c, i, j, k, n, m, s; /* indices */

    int band; /* indicates the current band */

    int stage; /* indicates the resolution stage of a band */

    short int qstp; /* quantizer step size */

    double bpp[NBANDS];  /* bitrate of each band */
    double bpptot;       /* sums total bitrate */

    int zero[SKIPMAX]; /* array with zero values */


    /* initializes the array with zero values */
    for (x = 0; x < SKIPMAX; x++) {
        zero[x] = 0;
    }


    head[1] = (unsigned short int) xsize;
    head[2] = (unsigned short int) ysize;



    /* updates values of dimensions */
    if (head[1] <= XIMG + SKIPMAX) {
        ximg = ((int) head[1] / (2 * DCTSIZE)) * 2 * DCTSIZE;
        //printf("\nximg= %d", ximg);
        skip = ((int) head[1] - ximg) / 2; /* number of pixels to skip in each side */
        //printf("\nskip= %d", skip);
    } else {
        printf("\nInvalid X dimension\n");
        exit(1);
    }
    if (head[2] <= YLUM) {
        ylum = (int) head[2];
    } else {
        printf("\nInvalid Y dimension\n");
        exit(1);
    }
    yimg = ylum + ylum / 2;
//printf("\nying= %d", yimg);
    ylowsize = yimg / ((int) pow(2.0, NSTAGES));
    xlowsize = ximg / ((int) pow(2.0, NSTAGES));

    /* allocates memory to store the luminace of the coded image */
    for (y = 0; y < ylum; y++) {
        if ((pCY[y] = (unsigned char *) malloc(ximg)) == NULL) {
            printf("Memory allocation for luminance failed at line %d", y);
            exit(1);
        }
    }


    /* allocates memory to store the luminace subband decomposition - short int */
    for (y = 0; y < ylum; y++) {
        if ((pSY[y] = (unsigned char *) malloc(ximg)) == NULL) {
            printf("Memory allocation for luminance transform failed at line %d", y);
            exit(1);
        }
    }


    /* allocates memory to store the subband decomposition - double */
//printf("\npSIMG[y]= %dx%d", yimg, ximg);
    for (y = 0; y < yimg; y++) {
        if ((pSIMG[y] = (double *) malloc(ximg * (sizeof(double)))) == NULL) {
            printf("Memory allocation for luminance transform failed at line %d", y);
            exit(1);
        }
    }

    /* allocates memory to store the subband decomposition - short int */
    for (y = 0; y < yimg; y++) {
        if ((pISIMG[y] = (short int *) malloc(ximg * (sizeof(short int)))) == NULL) {
            printf("Memory allocation for luminance transform failed at line %d", y);
            exit(1);
        }
    }

    /* IMPORTANT: in each line, it skips the first skip pixels and the last */
    /* skip. For the chrominance, it skips only skip/2 pixels on each side */




    /* translates unsigned char image into double and loads it in pSIMG*/
    for (y = 0; y < ylum; y++) /* luminance */
    {
        for (x = 0; x < ximg; x++) {
            *(pSIMG[y] + x) = (double) Image_orig[y][x];
        }
    }

    /* does the subband analysis */

    printf("\nSubband analysis ...");
    sub4anal(pSIMG, NSTAGES, 1);

    /* initializes subband boundaries */
    printf("\nInitializing band boundaries...");

    for (n = NSTAGES; n > 0; n--) {
        /* 0: luminance */
        bband[3 * n][0].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n][0].xmax = ximg / ((int) pow(2.0, NSTAGES - n));
        bband[3 * n][0].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n][0].ymax = ylum / ((int) pow(2.0, NSTAGES - n));
        /* 1: chrominance U */
        bband[3 * n][1].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 2));
        bband[3 * n][1].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n][1].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
        bband[3 * n][1].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 1)) + ylum;
        /* 2: chrominance V */
        bband[3 * n][2].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 2)) + ximg / 2;
        bband[3 * n][2].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 1)) + ximg / 2;
        bband[3 * n][2].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
        bband[3 * n][2].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 1)) + ylum;

        /* 0: luminance */
        bband[3 * n - 1][0].xmin = 0;
        bband[3 * n - 1][0].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n - 1][0].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n - 1][0].ymax = ylum / ((int) pow(2.0, NSTAGES - n));
        /* 1: chrominance U */
        bband[3 * n - 1][1].xmin = 0;
        bband[3 * n - 1][1].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 2));
        bband[3 * n - 1][1].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
        bband[3 * n - 1][1].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 1)) + ylum;
        /* 2: chrominance V */
        bband[3 * n - 1][2].xmin = ximg / 2;
        bband[3 * n - 1][2].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 2)) + ximg / 2;
        bband[3 * n - 1][2].ymin = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
        bband[3 * n - 1][2].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 1)) + ylum;

        /* 0: luminance */
        bband[3 * n - 2][0].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n - 2][0].xmax = ximg / ((int) pow(2.0, NSTAGES - n));
        bband[3 * n - 2][0].ymin = 0;
        bband[3 * n - 2][0].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 1));
        /* 1: chrominance U */
        bband[3 * n - 2][1].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 2));
        bband[3 * n - 2][1].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 1));
        bband[3 * n - 2][1].ymin = ylum;
        bband[3 * n - 2][1].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
        /* 2: chrominance V */
        bband[3 * n - 2][2].xmin = ximg / ((int) pow(2.0, NSTAGES - n + 2)) + ximg / 2;
        bband[3 * n - 2][2].xmax = ximg / ((int) pow(2.0, NSTAGES - n + 1)) + ximg / 2;
        bband[3 * n - 2][2].ymin = ylum;
        bband[3 * n - 2][2].ymax = ylum / ((int) pow(2.0, NSTAGES - n + 2)) + ylum;
    }
    /* 0: luminance */
    bband[0][0].xmin = 0;
    bband[0][0].xmax = ximg / ((int) pow(2.0, NSTAGES));
    bband[0][0].ymin = 0;
    bband[0][0].ymax = ylum / ((int) pow(2.0, NSTAGES));
    /* 1: chrominance U */
    bband[0][1].xmin = 0;
    bband[0][1].xmax = ximg / ((int) pow(2.0, NSTAGES + 1));
    bband[0][1].ymin = ylum;
    bband[0][1].ymax = ylum / ((int) pow(2.0, NSTAGES + 1)) + ylum;
    /* 2: chrominance V */
    bband[0][2].xmin = ximg / 2;
    bband[0][2].xmax = ximg / ((int) pow(2.0, NSTAGES + 1)) + ximg / 2;
    bband[0][2].ymin = ylum;
    bband[0][2].ymax = ylum / ((int) pow(2.0, NSTAGES + 1)) + ylum;

    /* translates subband decomposition into short int values for the entropy */
    /* calculation */

    /* processes each band */
    for (s = 0; s < NSTAGES; s++) {
        for (k = 1; k <= 3; k++) {
            band = 3 * s + k;
        }
    }


    /* translates subband decomposition into unsigned char values for storage */
    printf("\nTranslating subband decomposition...");
    for (s = 0; s < NBANDS; s++) {
        for (y = bband[s][0].ymin; y < bband[s][0].ymax; y++) /* luminance */
        {
            for (x = bband[s][0].xmin; x < bband[s][0].xmax; x++) {
                if (s == 0) {
                    *(pSY[y] + x) = mpel(round(*(pSIMG[y] + x) /
                                               pow(2.0, (double) NSTAGES)));
                } else {
                    *(pSY[y] + x) = mpel(round(*(pSIMG[y] + x) /
                                               pow(2.0, (double) (NSTAGES - 1 - s / 3)))) + 128;
                }
            }
        }
    }


    /* stores subband decomposition */

    printf("\nStoring coded subband decomposition...");


    for (y = 0; y < ylum; y++) {
        for (x = 0; x < ximg; x++) {
            Image[x][y] = pSY[x][y];
        }
    }

    /* does the subband synthesis */

    printf("\nSubband synthesis ...");
    sub4synt(pSIMG, NSTAGES, 1);

    /* translates restored image into unsigned char values */
    printf("\nTranslating restored image...");
    for (y = 0; y < ylum; y++) /* luminance */
    {
        for (x = 0; x < (ximg); x++) {
            Image_out[x][y] = mpel(round(*(pSIMG[y] + x)));
        }
    }

    /* stores restored image */
    printf("\nWriting restored image...");


/* the memory allocated is assumed to be freed upon program completion */

    printf("\n");
}


/* function that performs the 4 band subband analysis */

//extern int subanal(); /* function that performs two-band subband analysis */

/* external variables which contain the image dimensions */




int sub4anal(double *pSIMG[], int nstg, int l_f){

    int n, m, i, j, k; /* indices */
    int M;
    /* does the subband analysis */

/*
subanal(pIMG, xsize, ysize, xsrc, ysrc, xLdst, yLdst, xHdst, yHdst, h_v)
*/

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

    /* does the subband analysis */




    if (l_f == 0)  /* full band decomposition */
    {
        /* does every stage of the  horizontal subband decomposition */
        for (n = 0; n < nstg; n++) {
            printf("\nHorizontal decomposition - stage %d...", n + 1);
            m = (int) pow(2.0, n);
            for (k = 0; k < m; k++) {
                /* luminance */
                subanal(pSIMG, ximg / m, ylum, k * ximg / m, 0,
                        k * ximg / m, 0, (2 * k + 1) * ximg / (2 * m), 0, 0);
                if (ncomps == 3) {
                    /* chrominance U */
                    subanal(pSIMG, ximg / 2 * m, ylum / 2, k * ximg / 2 * m, ylum,
                            k * ximg / 2 * m, ylum, (2 * k + 1) * ximg / (4 * m), ylum, 0);
                    /* chrominance V */
                    subanal(pSIMG, ximg / 2 * m, ylum / 2, ximg / 2 + k * ximg / 2 * m, ylum,
                            ximg / 2 + k * ximg / 2 * m, ylum,
                            ximg / 2 + (2 * k + 1) * ximg / (4 * m), ylum, 0);
                }
            }
        }

        /* does every stage of the vertical subband decomposition */
        M = (int) pow(2.0, nstg);

        for (n = 0; n < nstg; n++) {
            printf("\nVertical decomposition - stage %d...", n + 1);
            m = (int) pow(2.0, n);
            /* passes through every horizontal band */
            for (j = 0; j < M; j++) {
                for (k = 0; k < m; k++) {
                    /* luminance */
                    subanal(pSIMG, ximg / M, ylum / m, j * ximg / M, k * ylum / m,
                            j * ximg / M, k * ylum / m,
                            j * ximg / M, (2 * k + 1) * ylum / (2 * m), 1);
                    if (ncomps == 3) {
                        /* chrominance U */
                        subanal(pSIMG, ximg / 2 * M, ylum / 2 * m,
                                j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                j * ximg / 2 * M, ylum + (2 * k + 1) * ylum / (4 * m), 1);
                        /* chrominance V */
                        subanal(pSIMG, ximg / 2 * M, ylum / 2 * m,
                                ximg / 2 + j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                ximg / 2 + j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                ximg / 2 + j * ximg / 2 * M, ylum + (2 * k + 1) * ylum / (4 * m), 1);
                    }
                }
            }
        }
    } else  /* low band partitioning only */
    {
        /* does every stage of the subband decomposition */
        for (n = 0; n < nstg; n++) {
            printf("\nAnalysis: stage %d...", n + 1);
            m = (int) pow(2.0, n);
            /* horizontal */
            /* luminance */
            subanal(pSIMG, ximg / m, ylum / m, 0, 0, 0, 0, ximg / (2 * m), 0, 0);
            if (ncomps == 3) {
                /* chrominance U */
                subanal(pSIMG, ximg / (2 * m), ylum / (2 * m), 0, ylum,
                        0, ylum, ximg / (4 * m), ylum, 0);
                /* chrominance V */
                subanal(pSIMG, ximg / (2 * m), ylum / (2 * m), ximg / 2, ylum,
                        ximg / 2, ylum, ximg / 2 + ximg / (4 * m), ylum, 0);
            }
            /* vertical */
            /* luminance */
            subanal(pSIMG, ximg / (2 * m), ylum / m, 0, 0, 0, 0, 0, ylum / (2 * m), 1);
            subanal(pSIMG, ximg / (2 * m), ylum / m,
                    ximg / (2 * m), 0, ximg / (2 * m), 0, ximg / (2 * m), ylum / (2 * m), 1);
            if (ncomps == 3) {
                /* chrominance U */
                subanal(pSIMG, ximg / (4 * m), ylum / (2 * m),
                        0, ylum, 0, ylum, 0, ylum + ylum / (4 * m), 1);
                subanal(pSIMG, ximg / (4 * m), ylum / (2 * m),
                        ximg / (4 * m), ylum, ximg / (4 * m),
                        ylum, ximg / (4 * m), ylum + ylum / (4 * m), 1);
                /* chrominance V */
                subanal(pSIMG, ximg / (4 * m), ylum / (2 * m),
                        ximg / 2, ylum, ximg / 2, ylum, ximg / 2, ylum + ylum / (4 * m), 1);
                subanal(pSIMG, ximg / (4 * m), ylum / (2 * m),
                        ximg / 2 + ximg / (4 * m), ylum, ximg / 2 + ximg / (4 * m),
                        ylum, ximg / 2 + ximg / (4 * m), ylum + ylum / (4 * m), 1);
            }
        }

    }

    return (1);
}


/* function that performs the 4 band subband synthesis */

//extern int subsynt(); /* functions that perform two-band subband */
/* analysis and synthesis, respectively */

/* external variables which contain the image dimensions */

int sub4synt(double *pSIMG[], int nstg, int l_f){
    int n, m, i, j, k; /* indices */
    int M;
    /* does the subband synthesis */

/*
subsynt(pIMG, xsize, ysize, xLsrc, yLsrc, xHsrc, yHsrc, xdst, ydst, h_v)
*/

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

    /* does the synthesis */


    if (l_f == 0)  /* full band decomposition */
    {
        /* does every stage of the vertical subband synthesis */
        M = (int) pow(2.0, nstg);
        for (n = nstg - 1; n >= 0; n--) {
            printf("\nVertical synthesis - stage %d...", n + 1);
            m = (int) pow(2.0, n);
            /* passes through every horizontal band */
            for (j = 0; j < M; j++) {
                for (k = 0; k < m; k++) {
                    /* luminance */
                    subsynt(pSIMG, ximg / M, ylum / (2 * m),
                            j * ximg / M, k * ylum / m,
                            j * ximg / M, (2 * k + 1) * ylum / (2 * m),
                            j * ximg / M, k * ylum / m, 1);
                    if (ncomps == 3) {
                        /* chrominance U */
                        subsynt(pSIMG, ximg / 2 * M, ylum / (4 * m),
                                j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                j * ximg / 2 * M, ylum + (2 * k + 1) * ylum / (4 * m),
                                j * ximg / 2 * M, ylum + k * ylum / 2 * m, 1);
                        /* chrominance V */
                        subsynt(pSIMG, ximg / 2 * M, ylum / (4 * m),
                                ximg / 2 + j * ximg / 2 * M, ylum + k * ylum / 2 * m,
                                ximg / 2 + j * ximg / 2 * M, ylum + (2 * k + 1) * ylum / (4 * m),
                                ximg / 2 + j * ximg / 2 * M, ylum + k * ylum / 2 * m, 1);
                    }

                }
            }
        }

        /* does every stage of the  horizontal subband synthesis */

        for (n = nstg - 1; n >= 0; n--) {
            printf("\nHorizontal decomposition - stage %d...", n + 1);
            m = (int) pow(2.0, n);
            for (k = 0; k < m; k++) {
                /* luminance */
                subsynt(pSIMG, ximg / (2 * m), ylum,
                        k * ximg / m, 0, (2 * k + 1) * ximg / (2 * m), 0,
                        k * ximg / m, 0, 0);
                if (ncomps == 3) {
                    /* chrominance U */
                    subsynt(pSIMG, ximg / (4 * m), ylum / 2,
                            k * ximg / (2 * m), ylum, (2 * k + 1) * ximg / (4 * m), ylum,
                            k * ximg / (2 * m), ylum, 0);
                    /* chrominance V */
                    subsynt(pSIMG, ximg / (4 * m), ylum / 2,
                            ximg / 2 + k * ximg / (2 * m), ylum,
                            ximg / 2 + (2 * k + 1) * ximg / (4 * m), ylum,
                            ximg / 2 + k * ximg / (2 * m), ylum, 0);
                }

            }
        }
    } else {
        /* does every stage of the subband synthesis */
        for (n = nstg - 1; n >= 0; n--) {
            printf("\nSynthesis: stage %d...", n + 1);
            m = (int) pow(2.0, n);
            /* vertical */
            /* luminance */
            subsynt(pSIMG, ximg / (2 * m), ylum / (2 * m), 0, 0, 0, ylum / (2 * m), 0, 0, 1);
            subsynt(pSIMG, ximg / (2 * m), ylum / (2 * m), ximg / (2 * m), 0,
                    ximg / (2 * m), ylum / (2 * m), ximg / (2 * m), 0, 1);
            if (ncomps == 3) {
                /* chrominance U */
                subsynt(pSIMG, ximg / (4 * m), ylum / (4 * m),
                        0, ylum, 0, ylum + ylum / (4 * m), 0, ylum, 1);
                subsynt(pSIMG, ximg / (4 * m), ylum / (4 * m), ximg / (4 * m), ylum,
                        ximg / (4 * m), ylum + ylum / (4 * m), ximg / (4 * m), ylum, 1);
                /* chrominance V */
                subsynt(pSIMG, ximg / (4 * m), ylum / (4 * m), ximg / 2, ylum,
                        ximg / 2, ylum + ylum / (4 * m), ximg / 2, ylum, 1);
                subsynt(pSIMG, ximg / (4 * m), ylum / (4 * m), ximg / 2 + ximg / (4 * m), ylum,
                        ximg / 2 + ximg / (4 * m), ylum + ylum / (4 * m),
                        ximg / 2 + ximg / (4 * m), ylum, 1);
            }
            /* horizontal */
            /* luminance */
            subsynt(pSIMG, ximg / (2 * m), ylum / m, 0, 0, ximg / (2 * m), 0, 0, 0, 0);
            if (ncomps == 3) {
                /* chrominance U */
                subsynt(pSIMG, ximg / (4 * m), ylum / (2 * m),
                        0, ylum, ximg / (4 * m), ylum, 0, ylum, 0);
                /* chrominance V */
                subsynt(pSIMG, ximg / (4 * m), ylum / (2 * m), ximg / 2, ylum,
                        ximg / 2 + ximg / (4 * m), ylum, ximg / 2, ylum, 0);
            }
        }

    }

    return (1);
}


