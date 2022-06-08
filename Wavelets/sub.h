//
// Created by raposo on 5/31/22.
//
#pragma once
#ifndef WAVELETS_SUB_H
#define WAVELETS_SUB_H

#include "subdefs2.h"

double** sub(int **Image_orig, int **Image_out, int **Image, double *pANAL[], int xsize, int ysize);
int sub4anal(double *pSIMG[], int nstg, int l_f);
int sub4synt(double *pSIMG[], int nstg, int l_f);
void sub_sintese_only(int **Image_out, int xsize, int ysize);

static int ximg;
static int yimg;
static int ylum;
static int xlowsize;
static int ylowsize;
static int skip;   /* number of pixels which will be skipped on each */
/* side when reading or writing the image file */

static int ncomps = 0; /* 0: B&W; 1: color */

typedef struct {
    int xmin; /* minimum value of the x boundary */
    int xmax; /* maximum value of the x boundary */
    int ymin; /* minimum value of the y boundary */
    int ymax; /* maximum value of the y boundary */
} boundary; /* holds values of image boundaries */

static boundary bband[NBANDS][NCOMPS]; /* holds the boundary values of each band */

#endif //WAVELETS_SUB_H
