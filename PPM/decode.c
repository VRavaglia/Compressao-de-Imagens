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


int main() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int maxContext = 5;
    int currentContext[maxContext];
    int ccSize = 0;

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

    start_model(freq_1, cum_freq_1);                              /* Set up other modules		*/
    start_inputing_bits();
    start_decoding(fin);

    struct cum_freqs *currentTable;

    for (;;)                                  /* Loop through characters	*/
    {
        int ch;
        int symbol;
        currentTable = cum_freq_1;
        symbol = decode_symbol(currentTable, fin);
//        printf("%c", index_to_char[symbol]);
//        printf("%i ", symbol);
        if (symbol == EOF_symbol) break;
        if(symbol == ESC_symbol){
            putc('<', fout);
            putc('E', fout);
            putc('>', fout);
            currentTable = currentTable[]next;
        }
        else {
            check_context(symbol, cum_freq_1, maxContext, currentContext, &ccSize);
            ch = index_to_char[symbol];          /* translate to a character 	*/
            putc(ch, fout);                      /* write that character 		*/
            update_model(symbol, freq_1, cum_freq_1, currentContext, ccSize);                 /* Update the model 			*/
        }
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tempo consumido: %f", cpu_time_used);

    exit(0);
}
