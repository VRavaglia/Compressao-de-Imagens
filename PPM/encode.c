/* MAIN PROGRAM FOR ENCODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_output.h"
#include "arithmetic_encode.h"
#include <time.h>
#include "ppm.h"

#define DEBUG 0

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
struct cum_freqs cum_freq_1[No_of_symbols + 1];
struct freqs freq_1[No_of_symbols + 1];
struct cum_freqs cum_freq[No_of_symbols + 1];
struct freqs freq[No_of_symbols + 1];

void printFreqs(){
    for (int i = 0; i < No_of_symbols + 1; ++i) {
        if(freq[i].freq > 0) {
            printf("\n%c | %i", index_to_char[i], freq[i].freq);
            if(freq[i].next != NULL){
                for (int j = 0; j < No_of_symbols + 1; ++j) {
                    if (freq[i].next[j].freq > 0){
                        printf("\n%c%c | %i", index_to_char[i], index_to_char[j], freq[i].next[j].freq );
//                        if(freq[i].next[j].next != NULL){
//                            for (int k = 0; k < No_of_symbols + 2; ++k) {
//                                if (freq[i].next[j].next[k].freq > 0){
//                                    printf("\n%c%c%c | %i", index_to_char[i], index_to_char[j], index_to_char[k], freq[i].next[j].next[k].freq );
//                                }
//                            }
//                        }
                    }
                }
            }
        }
    }
//    printf("\n\n");
//    for (int i = 0; i < No_of_symbols+1; ++i) {
//        printf("\n%c | %i", index_to_char[i], cum_freq[i].freq);
//    }
}

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


    char *inputFilename = "biblia.txt";
    char *outputFilename = "biblia_encoded.txt";
    int  encodedText[1000];
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

    start_model(freq_1, cum_freq_1);                             /* set up other modules.	*/
    start_outputing_bits();
    start_encoding();

    int its = 0;
    int lastChar;
    for (;;) {
        int ch;
        int symbol;
        ch = getc(fin);
        if (ch == EOF) break;                 /* Exit loop on end-of-file */


//        int escapes = escape_count(freq, currentContext, ccSize, maxContext, maxDepth);
        symbol = char_to_index[ch];
        if(its < 1000) encodedText[its] = symbol;


//        struct cum_freqs **escapeTableList = getTables(maxDepth, freq, cum_freq, cum_freq_1, currentContext, ccSize, &tSize);
        int escapes = 0;
        for (int i = 0; i < maxDepth; ++i) {
//            int subContextSize = ccSize - i - maxContext - 1 + maxDepth;
//            printf("\nFunciona: %i, nao funciona: %i", subContextSize, maxDepth - i - 1);
            int subContextSize = maxDepth - i - 1;
            int tempContext[subContextSize];
            for (int j = 0; j < subContextSize; ++j) {
                tempContext[j] = currentContext[i+j];
            }
            if (findInTable(freq, cum_freq, cum_freq_1, tempContext, subContextSize, symbol)) break;
            struct cum_freqs *encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - i);
            if (encodeTable != NULL) encode_symbol(ESC_symbol,encodeTable, fout);
//            if (encodeTable != NULL) printf("<esc>");
//            if (encodeTable == NULL) printf("<null>");

            escapes += 1;
        }
//        if (maxDepth == 2) printSFreq(gotoTable(freq, cum_freq, cum_freq_1, currentContext, maxDepth - escapes));
//        if (its == 2){
//            printSFreq(gotoTable(freq, cum_freq, cum_freq_1, currentContext, maxDepth - escapes));
//            exit(0);
//        }
        int subContextSize = ccSize - escapes;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
            tempContext[j] = currentContext[escapes+ j];
        }
        struct cum_freqs *encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - escapes);
        encode_symbol(symbol, encodeTable, fout);     /* Encode that symbol.	 	 */
//        printf("%c", index_to_char[symbol]);

//        free(escapeTableList);
        maxDepth += 1;
        if(maxDepth > maxContext) maxDepth = maxContext+1;

        update_model(freq, cum_freq, currentContext, ccSize, symbol, maxDepth);                 /* Update the model 	 	 */

        if (its == 1){
//            printSFreq(cum_freq);
//            exit(0);
        }

        lastChar = currentContext[0];
        check_context(ch, maxContext, currentContext, &ccSize);

        its += 1;
//        printf("\n");
    }

    int escapes = 0;
    for (int i = 0; i < maxDepth+1; ++i) {
        int subContextSize = maxDepth - i - 1;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
//            if (j == 0) {
//                tempContext[j] = lastChar;
//            }
//            else{
//                tempContext[j] = currentContext[i+j-1];
//            }
            tempContext[j] = currentContext[i+j];
        }
        struct cum_freqs *encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - i);
        if (encodeTable != NULL) encode_symbol(ESC_symbol,encodeTable, fout);
//        printf("\nFinal ESCAPE");
        escapes += 1;
    }

//    encode_symbol(EOF_symbol, cum_freq_1, fout);     /* Encodes the EOF symbol 	 */
    done_encoding(fout);                         /* Send the last few bits	 */
    done_outputing_bits(fout);

    fclose(fin);
    fclose(fout);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\nTempo consumido: %f", cpu_time_used);

//    printFreqs();

    exit(0);
}


