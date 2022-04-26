/* MAIN PROGRAM FOR ENCODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_output.h"
#include "arithmetic_encode.h"
#include <time.h>
#include "ppm.h"

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
struct cum_freqs cum_freq[No_of_symbols + 2];           /* Cummulative symbol frequencies 	*/
struct freqs freq[No_of_symbols + 2];        /* Symbol frequencies		*/

void printFreqs(){
    for (int i = 0; i < No_of_symbols + 2; ++i) {
        if(freq[i].freq > 1) {
            printf("\n%c | %i", index_to_char[i], freq[i].freq);
            if(freq[i].next != NULL){
                for (int j = 0; j < No_of_symbols + 2; ++j) {
                    if (freq[i].next[j].freq > 0){
                        printf("\n%c%c | %i", index_to_char[i], index_to_char[j],  freq[i].next[j].freq );
                        if(freq[i].next[j].next != NULL){
                            for (int k = 0; k < No_of_symbols + 2; ++k) {
                                if (freq[i].next[j].next[k].freq > 0){
                                    printf("\n%c%c%c | %i", index_to_char[i], index_to_char[j], index_to_char[k],  freq[i].next[j].next[k].freq );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int maxContext = 2;
    int currentContext[maxContext];
    int ccSize = 0;

    start_model(freq, cum_freq);                             /* set up other modules.	*/
    start_outputing_bits();
    start_encoding();

    char *inputFilename = "biblia.txt";
    char *outputFilename = "biblia_encoded.txt";
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

    for (;;) {
        int ch;
        int symbol;
        ch = getc(fin);
        if (ch == EOF) break;                 /* Exit loop on end-of-file */
        check_context(ch, cum_freq, maxContext, currentContext, &ccSize);
        symbol = char_to_index[ch];         /* Translates to an index 	 */
        encode_symbol(symbol, cum_freq, fout);     /* Encode that symbol.	 	 */
        update_model(symbol, freq, cum_freq, currentContext, ccSize);                 /* Update the model 	 	 */
    }
    encode_symbol(EOF_symbol, cum_freq, fout);     /* Encodes the EOF symbol 	 */
    done_encoding(fout);                         /* Send the last few bits	 */
    done_outputing_bits(fout);

    fclose(fin);
    fclose(fout);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nTempo consumido: %f", cpu_time_used);

    printFreqs();

    exit(0);
}


