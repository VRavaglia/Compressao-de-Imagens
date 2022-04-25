/* MAIN PROGRAM FOR DECODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_input.h"
#include "arithmetic_decode.h"
#include "adaptive_model.h"
#include <time.h>
#include "ppm.h"

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
struct cum_freqs cum_freq[No_of_symbols + 2];            /* Cummulative symbol frequencies 	*/
struct freqs freq[No_of_symbols + 2];


int main() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int maxContext = 2;
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

    start_model(freq, cum_freq);                              /* Set up other modules		*/
    start_inputing_bits();
    start_decoding(fin);

    for (;;)                                  /* Loop through characters	*/
    {
        int ch;
        int symbol;
        symbol = decode_symbol(cum_freq, fin);      /* decode next symbol 		*/
        check_context(symbol, cum_freq, maxContext, currentContext, &ccSize);
        if (symbol == EOF_symbol) break;          /* Exit loop if EOF symbol 	*/
        ch = index_to_char[symbol];          /* translate to a character 	*/
        putc(ch, fout);                      /* write that character 		*/
        update_model(symbol, freq, cum_freq, currentContext, ccSize);                 /* Update the model 			*/
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Tempo consumido: %f", cpu_time_used);

    exit(0);
}
