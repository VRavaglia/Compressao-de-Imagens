/* MAIN PROGRAM FOR DECODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_input.h"
#include "arithmetic_decode.h"
#include <time.h>
#include <string.h>
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


int main(int argc, char**argv) {
    // Instrucoes para o uso do programa
    if (argc <= 1){
        printf("\nDecodifica um arquivo e gera o arquivo nomeArquivo_decoded.txt:)");
        printf("\n\ndecoder.out [-v] [-t] nomeArquivo");
        printf("\n\n[-v] Exibe informacoes sobre a execucao na tela.");
        printf("\n[-t] Exibe o tempo de execucao");
        printf("\nnomeArquivo: Arquivo que sera codificado sem o tipo (assume-se .txt)\n");
        return 0;
    }

    bool v = false;
    bool t = false;

    // Checagem de argumentos de execucao
    for (int i = 0; i < argc-1; ++i) {
        if(strcmp("-v",argv[i]) == 0){
            v = true;
        }
        if(strcmp("-t",argv[i]) == 0){
            t = true;
        }

    }

    char inputFilename[256];
    char outputFilename[256];

    snprintf(outputFilename, sizeof(outputFilename), "%s%s", argv[argc-1], "_decoded.txt");
    snprintf(inputFilename, sizeof(inputFilename), "%s%s", argv[argc-1], ".txt");


    clock_t start, end;
    double cpu_time_used;
    if (t){
        start = clock();
    }



//    char *inputFilename = "biblia_encoded.txt";
//    char *outputFilename = "biblia_decoded.txt";
    FILE *fin = fopen(inputFilename, "rb");

    if(fin == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de entrada!");
        printf("\nNome fornecido: %s", inputFilename);
        return -1;
    }

    FILE *fout = fopen(outputFilename, "wb");

    if(fout == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");
        printf("\nNome gerado: %s", outputFilename);
        return -1;
    }


    fseek(fin, 0, SEEK_END);
    long file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    int maxContext = getc(fin) - 1;
    int currentContext[maxContext];
    int ccSize = 0;
    int maxDepth = 0;

    printf("\nArgumentos: \n");
    printf("\nT: %i", t);
    printf("\nV: %i", v);
    printf("\nInput: %s", inputFilename);
    printf("\nOutput: %s", outputFilename);
    printf("\nContexto: %i\n", maxContext);

    start_model(freq_1, cum_freq_1);                              /* Set up other modules		*/
    start_inputing_bits();
    start_decoding(fin);

    int its = 0;
    int escapes = 0;
    unsigned percent = 0;
    struct cum_freqs *newTable = NULL;
    bool ignoredSymbols[No_of_symbols + 1];
    struct cum_freqs *decodeTable = NULL;

    if (v){
        printf("\nDecodificando: %i%%", percent);
    }

    for (;;)                                  /* Loop through characters	*/
    {
        int ch;
        int symbol;

        int subContextSize = maxDepth - escapes;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
            tempContext[j] = currentContext[escapes+j];
        }

        if (escapes == 0){
            startIgnored(ignoredSymbols);
            newTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - escapes);
            decodeTable = newTable;
        }
        else{
            if (decodeTable != NULL){
                getNonZeroChars(decodeTable, ignoredSymbols);
            }
            decodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - escapes);
            newTable = createExludedTable(ignoredSymbols, decodeTable);
        }

        if (newTable == NULL){
            symbol = ESC_symbol;
        }else{
            symbol = decode_symbol(newTable, fin);
            if (escapes > 0) free(newTable);
        }

        if(symbol == ESC_symbol){

            escapes += 1;
            if(escapes > maxDepth) break;

        }
        else {
            decodeTable = NULL;

            startIgnored(ignoredSymbols);

            ch = index_to_char[symbol];
            putc(ch, fout);                      /* write that character 		*/

            maxDepth += 1;
            if(maxDepth > maxContext) maxDepth = maxContext+1;

            update_model(freq, cum_freq, currentContext, ccSize, symbol, maxDepth);
            check_context(ch, maxContext, currentContext, &ccSize);
            escapes = 0;
            its += 1;

        }

        if (v){
            if(its > file_size/5){
                its = 0;
                percent += 5;
                printf("\nDecodificando: %i%%", percent);
            }
        }
    }


    if (t){
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("\n\nTempo consumido: %.2f s", cpu_time_used);
    }

    printf("\n");

    return 0;
}
