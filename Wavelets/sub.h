//
// Created by raposo on 5/31/22.
//
#pragma once
#ifndef WAVELETS_SUB_H
#define WAVELETS_SUB_H

#include "subdefs2.h"

void sub(int **Image_orig, int **Image_out, int **Image, int xsize, int ysize);
void only_anal(int **Image_orig, double *pSIMG[YLUM], int xsize, int ysize);
void only_synt(int **Image_out, double *pSIMG[YLUM], int xsize, int ysize);
int sub4anal(double *pSIMG[], int nstg, int l_f);
int sub4synt(double *pSIMG[], int nstg, int l_f);

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
