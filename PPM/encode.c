/* MAIN PROGRAM FOR ENCODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_output.h"
#include "arithmetic_encode.h"
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include <asm-generic/errno.h>
#include <errno.h>
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

int main(int argc, char**argv) {

    // Instrucoes para o uso do programa
    if (argc <= 1){
        printf("\nCodifica um arquivo e gera o arquivo nomeArquivo_encoded.txt:)");
        printf("\n\nencoder.out [-v] [-t] nomeArquivo tamanhoContexto");
        printf("\n\n[-v] Exibe informacoes sobre a execucao na tela.");
        printf("\n[-t] Exibe o tempo de execucao");
        printf("\nnomeArquivo: Arquivo que sera codificado sem o tipo (assume-se .txt)\n");
        printf("\ntamanhoContexto: Tamanho do contexto que sera utilizado [-1, 254]\n");
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

    snprintf(outputFilename, sizeof(outputFilename), "%s%s", argv[argc-2], "_encoded.txt");
    snprintf(inputFilename, sizeof(inputFilename), "%s%s", argv[argc-2], ".txt");

    int maxContext = (int) strtoimax(argv[argc-1], NULL, 10);
    if (maxContext == UINTMAX_MAX && errno == ERANGE) return -2;
    if (maxContext < -1 || maxContext > 254){
        printf("\nTamanho do contexto não suportado.");
        return -3;
    }



    printf("\nArgumentos: \n");
    printf("\nT: %i", t);
    printf("\nV: %i", v);
    printf("\nInput: %s", inputFilename);
    printf("\nOutput: %s", outputFilename);
    printf("\nContexto: %i\n", maxContext);


    clock_t start, end;
    double cpu_time_used;

    if(t){

        start = clock();
    }

    int currentContext[maxContext];
    int ccSize = 0;
    int maxDepth = 0;

//    inputFilename = "biblia.txt";

     FILE *fin = fopen(inputFilename, "rb");

    if(fin == NULL)
    {
        printf("\nNao foi possivel abrir o arquivo de entrada!");
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

    putc(maxContext + 1, fout);

    start_model(freq_1, cum_freq_1);                             /* set up other modules.	*/
    start_outputing_bits();
    start_encoding();

    unsigned its = 0;
    unsigned percent = 0;
    printf("\nCodificando: %i%%", percent);
    for (;;) {
        int ch;
        int symbol;
        ch = getc(fin);
        if (ch == EOF) break;                 /* Exit loop on end-of-file */

        symbol = char_to_index[ch];

        bool foundInTable;

        bool ignoredSymbols[No_of_symbols + 1];
        startIgnored(ignoredSymbols);

        int escapes = 0;
        struct cum_freqs *encodeTable = NULL;
        struct cum_freqs *newTable = NULL;

        for (int i = 0; i < maxDepth; ++i) {
            int subContextSize = maxDepth - i - 1;
            int tempContext[subContextSize];
            for (int j = 0; j < subContextSize; ++j) {
                tempContext[j] = currentContext[i+j];
            }

            struct cum_freqs *lastTable = findInTable(freq, cum_freq, cum_freq_1, tempContext, subContextSize, symbol, &foundInTable);
            if (foundInTable) break;

            if (i == 0){
                newTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - i);
                encodeTable = newTable;
            }
            else{
                if (encodeTable != NULL){
                    getNonZeroChars(encodeTable, ignoredSymbols);
                }
                encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - i);
                if (i > 1){
                    free(newTable);
                }
                newTable = createExludedTable(ignoredSymbols, encodeTable);
            }

            if (newTable != NULL) {
                encode_symbol(ESC_symbol,newTable, fout);
            }

            escapes += 1;
        }
        int subContextSize = ccSize - escapes;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
            tempContext[j] = currentContext[escapes+ j];
        }
        if (newTable != NULL){
            getNonZeroChars(newTable, ignoredSymbols);
        }

        encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - escapes);

        newTable = createExludedTable(ignoredSymbols, encodeTable);

        encode_symbol(symbol, newTable, fout);     /* Encode that symbol.	 	 */

        free(newTable);
        maxDepth += 1;
        if(maxDepth > maxContext) maxDepth = maxContext+1;

        update_model(freq, cum_freq, currentContext, ccSize, symbol, maxDepth);                 /* Update the model 	 	 */

        check_context(ch, maxContext, currentContext, &ccSize);

        if(v){
            its += 1;

            if(its*4 > file_size/5){
                its = 0;
                percent += 5;
                printf("\nCodificando: %i%%", percent);
            }
        }
    }

    int escapes = 0;
    for (int i = 0; i < maxDepth+1; ++i) {
        int subContextSize = maxDepth - i - 1;
        int tempContext[subContextSize];
        for (int j = 0; j < subContextSize; ++j) {
            tempContext[j] = currentContext[i+j];
        }
        struct cum_freqs *encodeTable = gotoTable(freq, cum_freq, cum_freq_1, tempContext, maxDepth - i);
        if (encodeTable != NULL) encode_symbol(ESC_symbol,encodeTable, fout);
        escapes += 1;
    }

    done_encoding(fout);                         /* Send the last few bits	 */
    done_outputing_bits(fout);

    if (v){
        fseek(fout, 0, SEEK_END);
        long file_size_out = ftell(fout);

        printf("\n\n Taxa de compressao: %.2fx", (float) file_size/ (float) file_size_out);
    }

    fclose(fin);
    fclose(fout);

    if(t){
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("\n\nTempo consumido: %.2f s", cpu_time_used);
    }
    printf("\n");


    return 0;
}


