/***************************************************************************
 *   Copyright (C) 2008 by Nelson Carreira Francisco                       *
 *   eng.nelsito@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
*   Implementacaoo de um codificadror de imagens baseado em                *
*   quantificacao vectorial                                                *
*   Trabalho para a disciplina de Compressao de imagens                    *
*   Doutoramento em Engenharia Eletrica                                    *
*   Universidade Federal do Rio de Janeiro                                 *
*   Nelson Carreira Francisco                                              *
****************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "subdefs2.h"

#define RANGE_LUMINANCE 255 /* Range of image luminance values */
#define PERMS        0644     /* File acess permits:RW for the users and R for the others */
#define RANGEY         255     /* Range level of luminance */

#define NBANDAS 7
#define NDICS_MAX 30        //5 bits para a sua representacao. O indice 32 indica que a banda nao foi comprimida.

#define Clip1(a)            ((a)>255?255:((a)<0?0:(a)))


void ajuda(char *prgname);

void read_header_pgm(int *ysize, int *xsize, char *Fname);

void read_f_pgm(int **pelimg, int *ysize, int *xsize, char *Fname);

int **intmatrix(int nr, int nc);

float **floatmatrix(int nr, int nc);

float quad_err(int indice_dic);

void carrega_dicionario(int banda, int ndic);

double PSNR(int **origblk, int **cmpblk, int nline, int npixel);

double MSE(int **origblk, int **cmpblk, int nline, int npixel);

void escreve_indice(int indice);

void escreve_ind_dicionario(int dicionario);

void write_f_pgm(int **im_matrix, int nline, int npixel, char *filename);

unsigned char **ucmatrix(int nrl, int nrh, int ncl, int nch);

void escreve_relatorio(int banda);

void aloca_bandas(float taxa);

//Funcoes de escrita de bits para o ficheiro
void start_outputing_bits();

void output_bit(int bit, FILE *output_file);

void done_outputing_bits(FILE *output_file);


//Funcoes de decomposicao wavelets
void sub(int **Image_orig, int **Image, int xsize, int ysize);

extern int subanal();

int sub4anal(double *pSIMG[], int nstg, int l_f);

extern int subsynt();

int sub4synt(double *pSIMG[], int nstg, int l_f);

void sub_sintese_only(int **Image_out, int xsize, int ysize);

/* THE BIT BUFFER */
static int buffer;            /* Bits buffered for output 		*/
static int bits_to_go;            /* Number of bits still in buffer	        */
long Bits_Count;

//FILE *pointf_in;		//Ponteiro para o ficheiro (imagem) original
//FILE *pointf_out;		//Ponteiro para o ficheiro de saida

int **Image;
int **Image_orig;
int **Image_out;
int ysize[1], xsize[1];           /* The dimentions of the original image */
int sizex_b, sizey_b;


int BsizeX;
int BsizeY;
int num_codewords;
int bits_indice, num_el_dic;
int num_blocos;
int **dic;

int *bloco_original;

float SSE_total_banda, bitspp;
float taxa_total_final;

char nomes[NBANDS][NDICS_MAX][50];
float taxas[NBANDS][NDICS_MAX];
float distorcoes[NBANDS][NDICS_MAX];
int best_dic[NBANDS];

int num_dics[NBANDS];        //Numero de dicionarios efectivamente existentes em cada banda

FILE *pointf_list_dic;
FILE *pointf_in;
FILE *pointf_dic;
FILE *pointf_out;
FILE *pointf_file;
char *inname, *outname, *subname, *filename;            //Nome dos ficheiros de input e output

//---------------------------------------------------------------------------------------------------
int ximg;
int yimg;
int ylum;
int xlowsize;
int ylowsize;
int skip;   /* number of pixels which will be skipped on each */
/* side when reading or writing the image file */
int ncomps = 1; /* number of components of the image: 1 for B&W and 3 for color */

typedef struct {
    int xmin; /* minimum value of the x boundary */
    int xmax; /* maximum value of the x boundary */
    int ymin; /* minimum value of the y boundary */
    int ymax; /* maximum value of the y boundary */
} boundary; /* holds values of image boundaries */

boundary bband[NBANDS][NCOMPS]; /* holds the boundary values of each band */
/* for each image component (Y, U and V) */

//---------------------------------------------------------------------------------------------------

//******************************************************************************
//*                                                                            *
//*     Funcao main                                                            *
//*                                                                            *
//*                                                                            *
//******************************************************************************
int main(int argc, char *argv[]) {

    int i, j, i1, j1, n, k, ii;
    float aux, aux1;
    int indice;
    int contador;
    int media;
    float distorcao;
    float acumulador = 0;
    double psnr, mse;
    float taxa;
    int tamanho_dicionario;
    char list_dic[100];


    clock_t start, end;
    double elapsed = 0;
    start = clock();

    //Verifica argumentos
    if (argc < 6) {
        ajuda(argv[0]);
        return 0;
    }

    taxa = (float) atof(argv[1]);
    //printf("Taxa: %f", taxa);
    if ((taxa > 8) || (taxa < 0.01)) {
        printf(" A taxa devera estar entre 0.01 e 8 bpp\n\n");
        exit(1);
    }

    inname = argv[2];    //Ficheiro a comprimir
    subname = argv[3];    //Guarda decomposicao em wavelets
    outname = argv[4];    //Guarda reconstrucao
    filename = argv[5];    //Guarda ficheiro codificado

    //BsizeX=atoi(argv[6]);
    //tamanho_dicionario= atoi(argv[5]);	//Tamanho do bloco
    //BsizeX=atoi(argv[6]);
    //BsizeY=atoi(argv[7]);

    //num_codewords=tamanho_dicionario;

//.................................................................
    sprintf(list_dic, "../Dicionarios/list_dic.dat");
    pointf_list_dic = fopen(list_dic, "r");
    if (pointf_list_dic == NULL) {
        fprintf(stderr, "Impossivel abrir ficheiro de lista de dicionarios: %s\n\n", list_dic);
        exit(1);
    }

    for (k = 0; k < NBANDAS; k++) {
        fscanf(pointf_list_dic, "%d\n", &num_dics[k]);
        //printf("\nNumero de dicionarios banda [%d]: %d", k,num_dics[k]);
        for (ii = 0; ii < num_dics[k]; ii++) {
            fscanf(pointf_list_dic, "%s\n", &nomes[k][ii]);
            //printf("\nDicionario (%d): %s",k,  nomes[k][ii]);
        }
    }



    //for(k=0; k<NBANDAS; k++){
    //ii=0;
    //Varre os vários dicionarios
    //for(BsizeX=2; BsizeX<=4; BsizeX+=2){
    //	for(BsizeY=BsizeX; BsizeY<=4; BsizeY+=2){
    //		for(num_el_dic=3; num_el_dic<13; num_el_dic++){
    //			sprintf(nomes[k][ii],"../Dicionarios/dic_%d_%d_%dx%d.dat", k, ((int)pow(2, num_el_dic)), BsizeX, BsizeY);
    //			printf("\nDicionario (%d): %s",ii,  nomes[k][ii]);
    //			ii++;
    //		}
    //	}
    //}
    //printf("\nii: %d", ii);
    //}


//.................................................................


    //Le imagem a comprimir
    printf("\n Imagem a comprimir            : %s", inname);
    read_header_pgm(ysize, xsize, inname);     /* Reads the PGM file and returns the picture size */

    Image_orig = intmatrix(*ysize, *xsize);        //Garda a imagem original
    Image = intmatrix(*ysize,
                      *xsize);        //Guarda a decomposição em sub-banda das imagem original e posterior reconstrução
    Image_out = intmatrix(*ysize,
                          *xsize);        //Guarda a decomposição quantizada e a reconstrução da recosntrução quantizada

    read_f_pgm(Image_orig, ysize, xsize, inname);    /* Reads the PGM file and stores the image in pely */
    printf("\n Tamanho (%dx%d)             : %d pixels", *xsize, *ysize, ((*xsize) * (*ysize)));


    //Faz análise wavelets
    sub(Image_orig, Image, *xsize, *ysize);        //Image fica com a decomposicao da imagem original


    //Codigo para apagar
    //sub_sintese_only(Image, *xsize, *ysize);

    //write_f_pgm(Image, *ysize, *xsize, "Teste_desenperado.pgm");
    //return EXIT_SUCCESS;

    //--------------------------------------


    //***************************************************************************
    //Calcula a media das luminancias da gama LLL da imagem
    //***************************************************************************
    media = 0;

    for (i = bband[0][0].ymin; i < bband[0][0].ymax; i++) {
        //printf("\n");
        for (j = bband[0][0].xmin; j < bband[0][0].xmax; j++) {
            acumulador += Image[i][j];
            //printf(" %3d", Image[i][j]);
        }
    }
    media = acumulador / ((bband[0][0].xmax - bband[0][0].xmin) * (bband[0][0].ymax - bband[0][0].ymin));
    printf("\n Media: %d", media);
    //***************************************************************************


    //***************************************************************************
    //Subtrai a media a todos os pixels da gama LLL
    //***************************************************************************
    for (i = bband[0][0].ymin; i < bband[0][0].ymax; i++) {
        //printf("\n");
        for (j = bband[0][0].xmin; j < bband[0][0].xmax; j++) {
            Image[i][j] -= media;
            //printf(" %3d", Image[i][j]);
        }
    }
    //***************************************************************************


    //***************************************************************************
    //Codifica banda por banda
    //***************************************************************************
    for (k = 0; k < NBANDAS; k++) {        //Para todas as bandas
        sizex_b = bband[k][0].xmax - bband[k][0].xmin;
        sizey_b = bband[k][0].ymax - bband[k][0].ymin;

        printf("\n\nComprimindo banda %d", k);

        //Varre os vários dicionarios
        for (ii = 0; ii < num_dics[k]; ii++) {
            //for(BsizeX=2; BsizeX<=4; BsizeX+=2){
            //	for(BsizeY=BsizeX; BsizeY<=4; BsizeY+=2){

            carrega_dicionario(k, ii);

            free(bloco_original);
            bloco_original = (int *) calloc(BsizeX * BsizeY, sizeof(int));
            if (!bloco_original) {
                printf("intmatrix() - allocation failure 1 \n");
                exit(1);
            }


//				for(num_el_dic=3; num_el_dic<13; num_el_dic++){
            //Carrega dicionario


            //printf(" Bits por indice: %d", bits_indice);

            //printf("\n\nTerminou de ler dicionários\n\n");
            //fflush(stdout);
            SSE_total_banda = 0;
            for (i = bband[k][0].ymin; i < bband[k][0].ymin + sizey_b; i += BsizeY) {
                for (j = bband[k][0].xmin; j < bband[k][0].xmin + sizex_b; j += BsizeX) {
                    //Para todos os blocos
                    for (i1 = 0; i1 < BsizeY; i1++) {
                        for (j1 = 0; j1 < BsizeX; j1++) {
                            bloco_original[j1 + (i1 * BsizeX)] = Image[i + i1][j + j1];
                        }
                    }

                    //for(n=0; n<BsizeX*BsizeY; n++){
                    //	printf("%d\t", bloco_original[n]);
                    //}
                    //printf("\n");

                    distorcao = FLT_MAX;
                    for (n = 0; n < num_codewords; n++) {                    //Varre todos os elementos do codebook
                        aux = quad_err(n);
                        //printf("Distorcao[%d]= %f\t", i, aux);
                        if (aux < distorcao) {
                            indice = n;
                            distorcao = aux;
                        }
                    }
                    SSE_total_banda += distorcao;
                    //printf("\n%d", indice);
                    //fprintf(pointf_out, "%d\n", indice);
                    //escreve_indice(indice);

                    for (i1 = 0; i1 < BsizeY; i1++) {
                        for (j1 = 0; j1 < BsizeX; j1++) {
                            //printf(" %d", dic[indice][i1*BsizeX+j1]);
                            Image_out[i + i1][j + j1] = dic[indice][i1 * BsizeX + j1];
                        }
                    }

                }
            }

            taxas[k][ii] = bitspp;
            distorcoes[k][ii] = SSE_total_banda / ((float) (sizex_b * sizey_b));
            printf("\n %s", nomes[k][ii]);
            printf("\t\t\tbpp: %.3f MSE: %.0f ", taxas[k][ii], distorcoes[k][ii]);
//				}

        }
        //-------------------------------------------------------------------------
        escreve_relatorio(k);
    }


    //***************************************************************************
    aloca_bandas(taxa);


    fflush(stdout);



    //***************************************************************************
    //Codifica cada banda com o dicionario optimo

    start_outputing_bits();

    free(Image_out);            //Limpa Image_out para ter a certeza que nao fica la lixo
    Image_out = intmatrix(*ysize, *xsize);

    pointf_file = fopen(filename, "w");
    if (pointf_file == NULL) {
        fprintf(stderr, "Impossivel criar ficheiro de saida: %s\n\n", filename);
        exit(1);
    }

    fprintf(pointf_file, "%d\n", *xsize);
    fprintf(pointf_file, "%d\n", *ysize);
    fprintf(pointf_file, "%d\n", media);


    taxa_total_final = 0;
    for (k = 0; k < NBANDAS; k++) {        //Para todas as bandas
        sizex_b = bband[k][0].xmax - bband[k][0].xmin;
        sizey_b = bband[k][0].ymax - bband[k][0].ymin;

        //fprintf(pointf_file, "%d\n", best_dic[k]);
        escreve_ind_dicionario(best_dic[k]);

        if ((best_dic[k] >= 0) && (best_dic[k] < 31)) {
            carrega_dicionario(k, best_dic[k]);
            aux1 = (float) (((float) (sizex_b * sizey_b)) / ((float) ((*xsize) * (*ysize))));
            taxa_total_final += (taxas[k][best_dic[k]] * aux1);


            free(bloco_original);
            bloco_original = (int *) calloc(BsizeX * BsizeY, sizeof(int));
            if (!bloco_original) {
                printf("intmatrix() - allocation failure 1 \n");
                exit(1);
            }

            for (i = bband[k][0].ymin; i < bband[k][0].ymin + sizey_b; i += BsizeY) {
                for (j = bband[k][0].xmin; j < bband[k][0].xmin + sizex_b; j += BsizeX) {
                    //Para todos os blocos
                    for (i1 = 0; i1 < BsizeY; i1++) {
                        for (j1 = 0; j1 < BsizeX; j1++) {
                            bloco_original[j1 + (i1 * BsizeX)] = Image[i + i1][j + j1];
                        }
                    }

                    distorcao = FLT_MAX;
                    for (n = 0; n < num_codewords; n++) {                    //Varre todos os elementos do codebook
                        aux = quad_err(n);
                        if (aux < distorcao) {
                            indice = n;
                            distorcao = aux;
                        }
                    }
                    escreve_indice(indice);

                    for (i1 = 0; i1 < BsizeY; i1++) {
                        for (j1 = 0; j1 < BsizeX; j1++) {
                            Image_out[i + i1][j + j1] = dic[indice][i1 * BsizeX + j1];
                        }
                    }

                }
            }
            //aux1=(float)(((taxas[k][best_dic[k]]*aux1)/taxa)*100);
            //printf("\nBanda[%d] best_dic=%d: %.4f bpp (%f da taxa)", k, best_dic[k], taxas[k][best_dic[k]], aux1);
        } else {
            //printf("\nBanda[%d] nao codificada", k);
        }
    }

    for (k = 0; k < NBANDAS; k++) {
        if ((best_dic[k] >= 0) && (best_dic[k] < 31)) {
            sizex_b = bband[k][0].xmax - bband[k][0].xmin;
            sizey_b = bband[k][0].ymax - bband[k][0].ymin;
            aux1 = (float) (((float) (sizex_b * sizey_b)) / ((float) ((*xsize) * (*ysize))));
            aux1 = (float) (((taxas[k][best_dic[k]] * aux1) / taxa_total_final) * 100);
            printf("\nBanda[%d] best_dic=%d\t: %.4f bpp (%.2f%% da taxa)", k, best_dic[k], taxas[k][best_dic[k]], aux1);
        } else {
            printf("\nBanda[%d] nao codificada", k);
        }
    }

    done_outputing_bits(pointf_file);


    //-----------------------------------------------------
    //write_f_pgm(Image_out, *ysize, *xsize, outname);
    //------------------------------------------------------

    end = clock();
    elapsed += ((double) (end - start)) / CLOCKS_PER_SEC;
    start = clock();

    //***************************************************************************
    //Soma novamente a média a todos os pixels
    //***************************************************************************
    for (i = bband[0][0].ymin; i < bband[0][0].ymax; i++) {
        //printf("\n");
        for (j = bband[0][0].xmin; j < bband[0][0].xmax; j++) {
            Image_out[i][j] += media;                //Imagem quantizada
            Image[i][j] += media;                //Imagem nao quantizada
            //printf(" %3d", Image[i][j]);
        }
    }
    //***************************************************************************


    //***************************************************************************
    //PSNR no dominio da wavelet
    //***************************************************************************
    psnr = PSNR(Image_out, Image, *ysize, *xsize);
    mse = MSE(Image_out, Image, *ysize, *xsize);
    printf("\n PSNR dominio da wavelet          : %f dB", psnr);
    //write_f_pgm(Image_out, *ysize, *xsize, "Outintermedio.pgm");
    //write_f_pgm(Image, *ysize, *xsize, "Imageintermedio.pgm");


    //***************************************************************************
    //PSNR no dominio da wavelet, por banda
    //***************************************************************************
    int **bloco_orig;
    int **bloco_rec;

    for (k = 0; k < NBANDAS; k++) {
        sizex_b = bband[k][0].xmax - bband[k][0].xmin;
        sizey_b = bband[k][0].ymax - bband[k][0].ymin;

        bloco_orig = intmatrix(sizey_b, sizex_b);
        bloco_rec = intmatrix(sizey_b, sizex_b);

        for (i = 0; i < sizey_b; i++) {
            for (j = 0; j < sizex_b; j++) {
                bloco_orig[i][j] = Image[i + bband[k][0].ymin][j + bband[k][0].xmin];
                bloco_rec[i][j] = Image_out[i + bband[k][0].ymin][j + bband[k][0].xmin];
            }
        }
        psnr = PSNR(bloco_orig, bloco_rec, sizey_b, sizex_b);
        printf("\n\t PSNR banda[%d]            : %f dB", k, psnr);
    }
    //***************************************************************************




    //***************************************************************************
    //Faz a sintese da decomposicao original e da reconstruida
    //***************************************************************************
    sub_sintese_only(Image_out, *xsize, *ysize);
    sub_sintese_only(Image, *xsize, *ysize);
    //***************************************************************************


    psnr = PSNR(Image_out, Image, *ysize,
                *xsize);        //Calcula PSNR entre a imagem sintetizada a partir da decomposicao com e sem quantizacao

    //mse=MSE(Image_out, Image, *ysize, *xsize);

    printf("\n Tempo total de execucao       : %9.3f segundos", elapsed);
    printf("\n Total Bits                    : %d bits (%d Bytes)", Bits_Count, Bits_Count / 8);
    printf("\n bpp                           : %.4f", (float) Bits_Count / ((*ysize) * (*xsize)));

    //taxa=(float)(Bits_Count)/(float)((*xsize)*(*ysize));

    //printf("\n Taxa                          : %.2f bits/pixel", taxa);
    printf("\n PSNR dominio reconstrucao      : %f dB", psnr);
    //printf("\n MSE                           : %f", mse);


    psnr = PSNR(Image_orig, Image, *ysize, *xsize);
    printf("\n PSNR so wavelet sem quantizacao: %f dB", psnr);

    psnr = PSNR(Image_orig, Image_out, *ysize, *xsize);
    printf("\n PSNR final                     : %f dB", psnr);
    printf("\n Taxa total final: %.3f bpp", taxa_total_final);
    printf("\n Taxa alvo       : %.3f bpp", taxa);
    printf("\n-----------------------------------------------------\n\n");



    //***************************************************************************
    //Guarda reconstrucoes, com e sem quantificacao
    //***************************************************************************
    write_f_pgm(Image_out, *ysize, *xsize, outname);
    write_f_pgm(Image, *ysize, *xsize, subname);
    //***************************************************************************

    //fclose(pointf_out);
    return EXIT_SUCCESS;
}
//Fim da funcao main


void aloca_bandas(float taxa) {
    int i, k, i1, j1;
    float y_origem;
    float lambda, old_lambda;
    float distorcao = 0;
    float taxa_actual = 0;
    float lamb_max, lamb_min;
    float aux;

    lamb_max = FLT_MAX;
    //lamb_max=100000;
    lamb_min = 10;

    lambda = lamb_max;

    while (abs(old_lambda - lambda) >= 1) {

        taxa_actual = 0;
        for (k = 0; k < NBANDAS; k++) {
            distorcao = 0;
            sizex_b = bband[k][0].xmax - bband[k][0].xmin;
            sizey_b = bband[k][0].ymax - bband[k][0].ymin;
            //Calcula a distorcao incorrida se nao se codificar a banda
            for (i1 = bband[k][0].ymin; i1 < bband[k][0].ymax; i1++) {
                for (j1 = bband[k][0].xmin; j1 < bband[k][0].xmax; j1++) {
                    distorcao += (Image[i1][j1] * Image[i1][j1]);
                }
            }

            //distorcao=distorcao*(float)(((*xsize)*(*ysize))/(sizex_b*sizey_b));
            distorcao = distorcao / ((float) (sizex_b * sizey_b));

            if (k == 0) {    //Forca a que a banda 0 seja sempre codificada
                distorcao = FLT_MAX;
            }

            best_dic[k] = 31;        //Indicacao de que a banda n sera codificada
            //printf("\tDistorcao: %f", distorcao);


            for (i = 0; i < num_dics[k]; i++) {
                y_origem = distorcoes[k][i] + lambda * (taxas[k][i]);
                //printf("\nyo: %f, distorcao: %f, taxa: %f, lambda: %f", y_origem, distorcoes[k][i], taxas[k][i], lambda);
                if (y_origem <
                    distorcao) {        //O dicionário que apresentar um menor y_origem sera o dicionario optimo para um dado lambda
                    distorcao = y_origem;
                    best_dic[k] = i;
                }
            }

            if ((best_dic[k] >= 0) && (best_dic[k] < 31)) {
                //taxa_actual+=taxas[k][best_dic[k]];	//Calcula taxa resultante
                aux = (float) (((float) (sizex_b * sizey_b)) / ((float) ((*xsize) * (*ysize))));
                taxa_actual += (taxas[k][best_dic[k]] * aux);
            }
        }

        old_lambda = lambda;
        //printf("\n Lambda: %f, Taxa_actual: %.4f, aux: %f", lambda, taxa_actual, aux);
        //getchar();

        //Actualizacao do lambda
        if (taxa_actual > taxa) {        //Taxa demasiado grande->reduz lambda
            lamb_min = lambda;
        } else {                //Taxa demasiado pequena->aumenta lambda
            lamb_max = lambda;
        }

        lambda = (lamb_max + lamb_min) / 2;

        //getchar();

    }
    printf("\nLambda final: %f", lambda);
    //printf("\nBest_dic[%d]=%d", k, best_dic[k]);
}


void escreve_relatorio(int banda) {
    int i;
    FILE *pointf_relatorio;
    char rel_name[100];

    sprintf(rel_name, "./relatorio/relatorio_%d.dat", banda);
    pointf_relatorio = fopen(rel_name, "w");
    if (pointf_relatorio == NULL) {
        fprintf(stderr, "Impossivel abrir relatorio: %s\n\n", rel_name);
        exit(1);
    }

    for (i = 0; i < num_dics[banda]; i++) {
        fprintf(pointf_relatorio, "%f\t%f\n", taxas[banda][i], distorcoes[banda][i]);
    }

    fclose(pointf_relatorio);
}


void escreve_indice(int indice) {
    int k;
    int mask;
    int bit_to_write;

    mask = pow(2, bits_indice - 1);
    //printf("\nIndice: %d\t", indice);
    for (k = 0; k < bits_indice; k++) {
        bit_to_write = (mask & indice) / mask;
        //printf(" %d", bit);
        indice = indice << 1;
        output_bit(bit_to_write, pointf_file);
    }

}


void escreve_ind_dicionario(int dicionario) {
    int k;
    int mask;
    int bit_to_write;

    mask = pow(2, 4);
    //printf("\nIndice: %d\t", indice);
    for (k = 0; k < 5; k++) {
        bit_to_write = (mask & dicionario) / mask;
        //printf(" %d", bit);
        dicionario = dicionario << 1;
        output_bit(bit_to_write, pointf_file);
    }

}


//******************************************************************************
//*                                                                            *
//*     Calcula o erro quadrático entre um vectro do set de treino             *
//*	e um vector do codebook                                                *
//*                                                                            *
//******************************************************************************
void carrega_dicionario(int banda, int ndic) {
    int i, j;
    char dic_name[100];

    fflush(stdout);
    //sprintf(dic_name,"../Dicionarios/dic_%d_%d_%dx%d.dat", banda, ((int)pow(2, num_el_dic)), BsizeX, BsizeY);
    //printf("\nAbrindo dicionario: %s", dic_name);
    //pointf_dic=fopen(dic_name, "r");
    pointf_dic = fopen(nomes[banda][ndic], "r");
    if (pointf_dic == NULL) {
        fprintf(stderr, "\n Impossivel abrir dicionario: %s\n\n", nomes[banda][ndic]);
        exit(1);
    }

    fscanf(pointf_dic, "%d\n", &num_codewords);
    fscanf(pointf_dic, "%d\n", &BsizeX);
    fscanf(pointf_dic, "%d\n", &BsizeY);

    bits_indice = ceil(log(num_codewords) / log(2));

    //printf("\n-----------------------------------------------------");
    //printf("\n Carregou dicionario %s", dic_name);
    //printf("\n Bits: %d X: %d Y: %d", bits_indice, BsizeX, BsizeY);
    //printf("\t%d %d", *xsize/sizex_b, *xsize/sizey_b);
    //printf("\t bpp: %.3f", (float)((float)(bits_indice)/(float)(BsizeX*BsizeY)));

    //Normaliza o numero de bpp em funcao do numero de pixels da imagem original que cada pixel da banda representa
    bitspp = (float) ((float) (bits_indice) / (float) (BsizeX * BsizeY));
    //bitspp=bitspp/(float)(((*xsize)*(*ysize))/(sizex_b*sizey_b));
    //printf("\nbpp: %.4f", bitspp);
    //printf("\t num codewords: %d, Banda: %d, bpp: %.3f", num_codewords, banda, bitspp);
    //printf("\n %d blocos de %dx%d pixels", num_codewords, BsizeY, BsizeX);
    //printf("\n-----------------------------------------------------");
    fflush(stdout);

    free(dic);
    dic = intmatrix(num_codewords, BsizeY * BsizeX);

    for (i = 0; i < num_codewords; i++) {
        for (j = 0; j < BsizeX * BsizeY; j++) {
            fscanf(pointf_dic, "%d\t", &dic[i][j]);
        }
        fscanf(pointf_dic, "\n");
    }

    //printf("\n");
    //for(i=0; i<num_codewords; i++){
    //	for(j=0; j<BsizeX*BsizeY; j++){
    //		printf("%d\t", dic[i][j]);
    //	}
    //	printf("\n");
    //}
    fclose(pointf_dic);
}


//******************************************************************************
//*                                                                            *
//*     Calcula o erro quadrático entre um vectro do set de treino             *
//*	e um vector do codebook                                                *
//*                                                                            *
//******************************************************************************
float quad_err(int indice_dic) {
    int i, j;
    float tmp = 0;

    for (i = 0; i < BsizeX * BsizeY; i++) {
        for (i = 0; i < BsizeX * BsizeY; i++) {
            tmp += ((dic[indice_dic][i] - bloco_original[i]) * (dic[indice_dic][i] - bloco_original[i]));
        }
    }
    return tmp;
} /* Enf of quad_err */






//******************************************************************************
//*                                                                            *
//*     Apresenta informacao e sintaxe do programa			       *
//*                                                                            *
//******************************************************************************
void ajuda(char *prgname) {
    printf("\n---------------------------------------------------------------------------------\n");
    printf(" Programa de codificação de imagens baseado em decomposicao em wavelets e \n");
    printf(" quantização vectorial\n");
    printf("---------------------------------------------------------------------------------\n");
    printf(" Sintaxe: %s <taxa alvo> <imagem a comprimir> <reconstrucao da decomposicao> <reconstrucao imagem saida> <ficheiro destino>\n",
           prgname);
    printf("---------------------------------------------------------------------------------\n\n");
}


/************************************************************************************/
/*                                                                                  */
/* READS THE INFORMATION OF A PGM FILE HEADER                                       */
/* usage: read_header_pgm (int *ximg,int *yimg,char *originalfilename);             */
/*                                                                                  */
/************************************************************************************/
void read_header_pgm(int *ysize, int *xsize, char *Fname) {
    FILE *pointf;
    char pelimg[1][20];
    char dummy[12];
    char aux;
    int lido = 0;

    /*
    if ((pointf=open(Fname,O_RDONLY,0)) == -1) {
    fprintf(stderr,"\ncannot open file %s\n",Fname);
    exit(1);
    }
  */

    pointf = fopen(Fname, "r");
    if (pointf == NULL) {
        fprintf(stderr, "\nCannot open input file: %s\n", Fname);
        exit(1);
    }

    fscanf(pointf, "%s\n", dummy);


//*******************************
    do {
        aux = getc(pointf);
        //fflush(stdout);
        if (aux == '#') {
            do {
                aux = getc(pointf);
            } while ((aux != '\n') && (aux != EOF));


        } else {
            ungetc(aux, pointf);
            lido = 1;
        }
    } while (lido == 0);

    fscanf(pointf, " %d %d", xsize, ysize);
//********************************

    fclose(pointf);        /* closes file */
}

/* End of read_header_pgm function */




/************************************************************************************/
/*                                                                                  */
/* read_f_pgm: READS A PGM FILE                                                     */
/* usage: read_f_pgm (unsigned char **pelimg, int *ysize, int *xsize, char *Fname)  */
/*                                                                                  */
/* Inputs:                                                                          */
/* Fname - File name                                                                */
/*                                                                                  */
/* Returns
/* - the image in char **pellimg
   - number of rows
   - number of columns                                                              */
/*                                                                                  */
/************************************************************************************/

void read_f_pgm(int **pelimg, int *ysize, int *xsize, char *Fname) {
    int i, j;
    FILE *pointf;
    char dummy[15];
    int aux[1];
    char aux1;
    int lido = 0;


    pointf = fopen(Fname, "r");
    if (pointf == NULL) {
        fprintf(stderr, "\nCannot open input file: %s\n", Fname);
        exit(1);
    }

    fscanf(pointf, "%s\n", dummy);

//*******************************
    do {
        aux1 = getc(pointf);
        //fflush(stdout);
        if (aux1 == '#') {
            do {
                aux1 = getc(pointf);
            } while ((aux1 != '\n') && (aux1 != EOF));
        } else {
            ungetc(aux1, pointf);
            lido = 1;
        }
    } while (lido == 0);


    fscanf(pointf, " %d %d", xsize, ysize);
//********************************

    fscanf(pointf, "%d\n", aux);

    for (i = 0; i < *ysize; i++) {
        for (j = 0; j < *xsize; j++) {
            pelimg[i][j] = (unsigned char) fgetc(pointf);
        }
    }

    fclose(pointf);        /* closes file */
}
/* End of read_f_pgm function */


/*************************************************************************************/
/*                                                                                   */
/*  INTMATRIX - Allocates memory for a matrix of variables of type int               */
/*                                                                                   */
/*  Inputs:                                                                          */
/*    number of rows / nunmber of columnc                                            */
/*  Returns a poiter to a int matrix (int **)                                        */
/*                                                                                   */
/*************************************************************************************/
int **intmatrix(int nr, int nc) {
    int i;
    int **m;

    m = (int **) malloc((unsigned) (nr) * sizeof(int *));
    if (!m) {
        printf("intmatrix() - allocation failure 1 \n");
        exit(1);
    }

    for (i = 0; i < nr; i++) {
        m[i] = (int *) malloc((unsigned) (nc) * sizeof(int));
        if (!m[i]) {
            printf("intmatrix() - allocation failure 2 \n");
            exit(1);
        }
    }

    return m;
}


float **floatmatrix(int nr, int nc) {
    int i;
    float **m;

    m = (float **) malloc(nr * nc * sizeof(float *));
    if (!m) {
        printf("floatmatrix() - allocation failure 1 \n");
        exit(1);
    }

    for (i = 0; i < nr; i++) {
        m[i] = (float *) calloc(nc, sizeof(float));
        if (!m[i]) {
            printf("floatmatrix() - allocation failure 2 \n");
            exit(1);
        }
    }

    return m;
}


/************************************************************************************/
/* Peak Signal Noise Ratio                                                          */
/************************************************************************************/
double PSNR(int **origblk, int **cmpblk, int nline, int npixel) {
    int i, j;
    double psnr;

    psnr = 0.;
    for (j = 0; j < nline; j++)
        for (i = 0; i < npixel; i++)
            psnr += ((double) *(origblk[j] + i) - (double) *(cmpblk[j] + i)) *
                    ((double) *(origblk[j] + i) - (double) *(cmpblk[j] + i));

    if (psnr == 0.0) psnr = 0.000000000000000000000000000000000000000000000000000001;
    psnr = 10.0 * log10((double) (RANGEY * RANGEY) / (psnr / (double) (nline * npixel)));

    return psnr;
}
/* End of Psnr function */

/************************************************************************************/
/* Mean Squared Error                                                               */
/************************************************************************************/
double MSE(int **origblk, int **cmpblk, int nline, int npixel) {
    int i, j;
    long cnt = 0;
    double mse;

    mse = 0.;
    for (j = 0; j < nline; j++)
        for (i = 0; i < npixel; i++) {
            mse += ((double) *(origblk[j] + i) - (double) *(cmpblk[j] + i)) *
                   ((double) *(origblk[j] + i) - (double) *(cmpblk[j] + i));
            cnt++;
        }

    return (mse / cnt);
}
/* End of MSE function */




/* INITIALISE BIT OUTPUT */
void start_outputing_bits() {
    buffer = 0;           /* Bits buffered for ouput		*/
    bits_to_go = 8;       /* Number of bits free in buffer	*/
    Bits_Count = 0;
}


/* OUTPUT A BIT  */
void output_bit(bit, output_file)
        int bit;
        FILE *output_file;
{
    buffer >>= 1;                /* Put bit in top of buffer	*/
    if (bit) buffer |= 0x80;
    bits_to_go -= 1;
    Bits_Count += 1;
    if (bits_to_go == 0)        /* Output buffer if it is	*/
    {                        /* now full					*/
        putc(buffer, output_file);
/*       printf("  %x\n", buffer); */
        bits_to_go = 8;
    }
}


/* FLUSH OUT THE LAST BITS */
void done_outputing_bits(FILE *output_file) {
    putc(buffer >> bits_to_go, output_file);
    //fprintf(stderr," Total Bits: %d bits\n",Bits_Count);
}



/************************************************************************************/
/* WRITE OUTPUT LUMINANCE FILE - PGM format*/
/************************************************************************************/
void write_f_pgm(int **im_matrix, int nline, int npixel, char *filename) {
    int i;
    int pointfo;
    char header_pgm[20];
    int npixel_orig, nline_orig;

    npixel_orig = npixel;
    nline_orig = nline;

    if ((pointfo = creat(filename, PERMS)) == -1)    /* creates file */
    {
        fprintf(stderr, "Write_f_PGM - cannot create PGM file -> %s", filename);
        exit(1);
    }

    /* Defines the header of file - type pgm */
    *(header_pgm + 0) = 'P';
    *(header_pgm + 1) = '5';
    *(header_pgm + 2) = '\n';

    i = 3;
    if (npixel >= 1000) {
        *(header_pgm + i) = (unsigned char) (npixel / 1000 + 48);
        npixel = npixel % 1000;
        i++;
    }
    *(header_pgm + i) = (unsigned char) (npixel / 100 + 48);
    i++;
    *(header_pgm + i) = (unsigned char) (npixel % 100) / 10 + 48;
    i++;
    *(header_pgm + i) = (unsigned char) ((npixel % 100) % 10 + 48);
    i++;

    *(header_pgm + i) = ' ';
    i++;

    if (nline >= 1000) {
        *(header_pgm + i) = (unsigned char) (nline / 1000 + 48);
        nline = nline % 1000;
        i++;
    }
    *(header_pgm + i) = (unsigned char) (nline / 100 + 48);
    i++;
    *(header_pgm + i) = (unsigned char) ((nline % 100) / 10 + 48);
    i++;
    *(header_pgm + i) = (unsigned char) ((nline % 100) % 10 + 48);
    i++;

    *(header_pgm + i) = '\n';
    i++;

    *(header_pgm + i) = (unsigned char) (RANGE_LUMINANCE / 100 + 48);
    i++;
    *(header_pgm + i) = (unsigned char) ((RANGE_LUMINANCE % 100) / 10 + 48);
    i++;
    *(header_pgm + i) = (unsigned char) ((RANGE_LUMINANCE % 100) % 10 + 48);
    i++;

    *(header_pgm + i) = '\n';
    i++;

    write(pointfo, (char *) header_pgm, i);

/*   for (i=0;i<(nline_orig);i++) */
/*     write(pointfo, im_matrix[i], npixel_orig); */

    unsigned char **Image_tmp;
    int j;
    Image_tmp = ucmatrix(0, nline, 0, npixel);

    for (i = 0; i < nline; i++)
        for (j = 0; j < npixel; j++)
            Image_tmp[i][j] = (unsigned char) im_matrix[i][j];

    for (i = 0; i < (nline_orig); i++)
        write(pointfo, Image_tmp[i], npixel_orig);

    close(pointfo);                /* closes file */
}

/*END of write_f_pgm */

/*************************************************************************************/
/*                                                                                   */
/*  UCMATRIX - Allocates memory for a matrix of variables of type unsigned char      */
/*                                                                                   */
/*  Inputs:                                                                          */
/*    1st and last row / 1st and last column                                         */
/*  Returns a poiter to a unsigned char matrix (unsigned char **)                    */
/*                                                                                   */
/*************************************************************************************/
unsigned char **ucmatrix(int nrl, int nrh, int ncl, int nch) {
    int i;
    unsigned char **m;

    m = (unsigned char **) malloc((unsigned) (nrh - nrl + 1) * sizeof(char *));
    if (!m) {
        printf("ucmatrix() - allocation failure 1 \n");
        exit(1);
    }
    m -= nrl;

    for (i = nrl; i <= nrh; i++) {
        m[i] = (unsigned char *) malloc((unsigned) (nch - ncl + 1) * sizeof(char));
        if (!m[i]) {
            printf("ucmatrix() - allocation failure 2 \n");
            exit(1);
        }
        m[i] -= ncl;
    }
    return m;
}


//***************************************************************************************
//*
//*
//*		Funcoes da wavelets
//*
//*
//***************************************************************************************
void sub(int **Image_orig, int **Image, int xsize, int ysize) {

    int sub4anal(); /* function that performs two-band subband analysis */

    int cont;

    double *pSIMG[YIMG];
    /* array of pointers to the beginning of each line */
    /* of the image subband decomposition */


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






    /* allocates memory to store the subband decomposition - double */
//printf("\npSIMG[y]= %dx%d", yimg, ximg);
    for (y = 0; y < yimg; y++) {
        if ((pSIMG[y] = (double *) malloc(ximg * (sizeof(double)))) == NULL) {
            printf("Memory allocation for luminance transform failed at line %d", y);
            exit(1);
        }
    }

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



    /* stores subband decomposition */

    printf("\nStoring coded subband decomposition...");
    for (y = 0; y < ylum; y++) {
        for (x = 0; x < ximg; x++) {
            Image[y][x] = (int) (round(pSIMG[y][x]));
        }
    }

    printf("\n");
}


int sub4anal(double *pSIMG[], int nstg, int l_f) {

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


int sub4synt(double *pSIMG[], int nstg, int l_f) {
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


void sub_sintese_only(int **Image_out, int xsize, int ysize) {

    int cont;


    double *pSIMG[YIMG];
    /* array of pointers to the beginning of each line */
    /* of the image subband decomposition */


    unsigned short int head[4]; /* stores header of image files */



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



    /* allocates memory to store the subband decomposition - double */
//printf("\npSIMG[y]= %dx%d", yimg, ximg);
    for (y = 0; y < yimg; y++) {
        if ((pSIMG[y] = (double *) malloc(ximg * (sizeof(double)))) == NULL) {
            printf("Memory allocation for luminance transform failed at line %d", y);
            exit(1);
        }
    }


/* translates unsigned char image into double and loads it in pSIMG*/
    for (y = 0; y < ylum; y++) /* luminance */
    {
        for (x = 0; x < ximg; x++) {
            *(pSIMG[y] + x) = (double) Image_out[y][x];
        }
    }


    printf("\nSubband synthesis ...");
    sub4synt(pSIMG, NSTAGES, 1);

    /* translates restored image into unsigned char values */
    printf("\nTranslating restored image...");
    for (y = 0; y < ylum; y++) /* luminance */
    {
        for (x = 0; x < (ximg); x++) {
            Image_out[y][x] = mpel(round(*(pSIMG[y] + x)));
        }
    }

    printf("\n");
}






