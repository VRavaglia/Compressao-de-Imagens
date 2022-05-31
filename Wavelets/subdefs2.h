/* this file contains all the includes and defines necessary for running the */
/* SUBDCT PROGRAMS */

#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

//#define XIMG      720    /* effective width of the image */
//#define YIMG      864    /* effective height of the image */
//#define YLUM      576    /* effective height of the luminance of the image*/
#define XIMG      1000    /* effective width of the image */
#define YIMG      1000    /* effective height of the image */
#define YLUM      1000    /* effective height of the luminance of the image*/
//#define XIMG      720    /* effective width of the image */
//#define YIMG      576    /* effective height of the image */
//#define YLUM      576    /* effective height of the luminance of the image*/
#define DCTSIZE   8      /* the DCT is DCTSIZE in each direction */
#define DCT2SIZE  64     /* DCTSIZE*DCTSIZE */
//#define YDCTNO    54     /* YIMG/DCTSIZE -number of blocks in the y direction */
//#define XDCTNO    44     /* XIMG/DCTSIZE -number of blocks in the x direction */
#define YDCTNO    112     /* YIMG/DCTSIZE -number of blocks in the y direction */
#define XDCTNO    100     /* XIMG/DCTSIZE -number of blocks in the x direction */

//#define NSTAGES   3
#define NSTAGES   3
#define NBANDS    NSTAGES*3+1     /* number of bands */
#define NCOMPS    3      /* 3 image components - YUV */
#define NLEVELS   48     /* number of levels of the non-negative part of the */
/* non-linear quantizer used */

#define SKIPMAX   20     /* maximum number of pixels which will be skipped on */
/* each side when reading or writing the image file */


#define MAGIC     0xc0de /* magic number */
#define PERMS     0644   /* File acess permits:RW for me and R for the others */
#define FORMAT    68     /* specifies YUV-CIF format */

#define mpel(x)     (unsigned char)( (x)>255 ? 255 : (x)<0 ? 0 : (x) )
/* restricts integer variable to valid pel values */

#define mshort(x)   (short int)( (x)>32767 ? 32767 : (x)<-32768 ? -32768 : (x) )
/* restricts double variable to valid short int values */

#define round(x)    ( (x)>=0 ? floor((x) + 0.5) : (-floor(-(x) + 0.5)) )
/* performs rounding of a double variable */

