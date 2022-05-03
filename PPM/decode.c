/* MAIN PROGRAM FOR DECODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_input.h"
#include "arithmetic_decode.h"
#include <time.h>
#include "ppm.h"

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
struct cum_freqs cum_freq_1[No_of_symbols + 2];            /* Cummulative symbol frequencies 	*/
struct freqs freq_1[No_of_symbols + 2];
struct cum_freqs cum_freq[No_of_symbols + 1];
struct freqs freq[No_of_symbols + 1];

void printSFreq(struct cum_freqs *sfreq){
    printf("\n\n");
    for (int i = 0; i < No_of_symbols+1; ++i) {
        printf("\n%c | %i", index_to_char[i], sfreq[i].freq);
    }
}


int main() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int maxContext = Max_context;
    int currentContext[maxContext];
    int ccSize = 0;
    int maxDepth = 0;

    char *inputFilename = "biblia_encoded.txt";
    char *outputFilename = "biblia_decoded.txt";
    FILE *fin = fopen(inputFilename, "rb");

    if(fin == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de entrada!");
        exit(1);
    }

    FILE *fout = fopen(outputFilename, "wb");

    if(fout == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");
        exit(1);
    }

    fseek(fin, 0, SEEK_END);
    long file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    start_model(freq_1, cum_freq_1);                              /* Set up other modules		*/
    start_inputing_bits();
    start_decoding(fin);

    int its = 0;
    int escapes = 0;
    unsigned percent = 0;
    for (;;)                                  /* Loop through characters	*/
    {
        int ch;
        int symbol;

        int subContextSize = maxDepth - escapes;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
            tempContext[j] = currentContext[escapes+j];
        }

        struct cum_freqs *decodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - escapes);
        if (decodeTable == NULL){
            symbol = ESC_symbol;
        }else{
            symbol = decode_symbol(decodeTable, fin);
        }



//        if (symbol == EOF_symbol) break;
        if(symbol == ESC_symbol){
//            putc('<', fout);
//            putc('E', fout);
//            putc('>', fout);
//            printf("<esc> ");
            escapes += 1;
            if(escapes > maxDepth) break;


        }
        else {
//            printf("%c", index_to_char[symbol]);
//            printf("%i ", symbol);
            ch = index_to_char[symbol];
            putc(ch, fout);                      /* write that character 		*/
            maxDepth += 1;
            if(maxDepth > maxContext) maxDepth = maxContext+1;

            update_model(freq, cum_freq, currentContext, ccSize, symbol, maxDepth);
            check_context(ch, maxContext, currentContext, &ccSize);
            escapes = 0;
            its += 1;

        }


        if(its > file_size/5){
            its = 0;
            percent += 5;
            printf("\nDecodificando: %i", percent);
        }
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


    printf("\nTempo consumido: %f", cpu_time_used);

    exit(0);
}
