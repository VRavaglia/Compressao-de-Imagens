#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include <stdbool.h>



void check_context(int ch, int maxContext, int currentContext[], int *ccSize){
    if (maxContext > 0){
        if(*ccSize < maxContext){
            currentContext[*ccSize] = char_to_index[ch];
            *ccSize = *ccSize + 1;
        }
        else{
            for (int i = 0; i < maxContext-1; ++i) {
                currentContext[i] = currentContext[i+1];
            }
            currentContext[maxContext-1] = char_to_index[ch];
        }
    }
}

void create_table(struct freqs freq[], struct cum_freqs cum_freq[]) {
    freq->next = (struct freqs*)malloc(sizeof(struct freqs)*(No_of_symbols + 2));
    cum_freq->next = (struct cum_freqs*)malloc(sizeof(struct cum_freqs)*(No_of_symbols + 2));

    struct freqs *fPointer = freq->next;
    struct cum_freqs *fcPointer = cum_freq->next;

    for (int i = 0; i <= No_of_symbols; i++) {
        fPointer[i].freq = 0;
        fPointer[i].next = NULL;
        fcPointer[i].freq = 0;
        fcPointer[i].next = NULL;
    }
}


void start_model(struct freqs freq[], struct cum_freqs cum_freq[]) {
    int i;
    for (i = 0; i < No_of_chars; i++) {              /* Set up tables that		*/
        char_to_index[i] = i + 1 + 1;                    /* translate between symbol	*/
        index_to_char[i + 1 + 1] = i;                    /* indexes and characters	*/
    }

    for (i = 0; i <= No_of_symbols; i++) {          /* Set up initial frequency	*/
        freq[i].freq = 1;                                /* counts to be one for all	*/
        freq[i].next = NULL;
        cum_freq[i].freq = No_of_symbols - i;        /* symbols					*/
        cum_freq[i].next = NULL;
    }
//    cum_freq[0].freq = 1;
    freq[0].freq = 0;                                        /* freq_1[0] must not be the	*/
}                                                /* same as freq_1[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


void update_model(struct freqs freq[], struct cum_freqs cum_freq[], const int context[], int cSize, int symbol, int maxDepth)
{


    for (int i = 0; i < maxDepth; ++i) {
        struct freqs *freqPointer = freq;
        struct cum_freqs *cumPointer = cum_freq;

        int ccSize = cSize - i;
        int currentContext[ccSize];
        for (int j = 0; j < ccSize; ++j) {
            currentContext[j] = context[i+j];
        }

        for (int j = 0; j < maxDepth-1-i; ++j) {
            int tempSymbol = currentContext[j];

            if(freqPointer[tempSymbol].next == NULL){
                create_table(&freqPointer[tempSymbol], &cumPointer[tempSymbol]);
            }
            freqPointer = freqPointer[tempSymbol].next;
            cumPointer = cumPointer[tempSymbol].next;
        }

        int tempSymbol =  symbol;

        if (cumPointer[1].freq == Max_frequency)                /* See if frequency counts	*/
        {                                            /* are at their maximum		*/
            int cum;
            cum = 0;
            for (int j = No_of_symbols; j >= 0; j--)        /* If so, halve all the 	*/
            {                                        /* counts ( keeping them	*/
                freqPointer[j].freq = (freqPointer[j].freq + 1) / 2;                /* non-zero).				*/
                cumPointer[j].freq = cum;
                cum += freqPointer[j].freq;
            }
        }

        freqPointer[tempSymbol].freq += 1;
        if (freqPointer[tempSymbol].freq == 1){
            cumPointer[0].freq += 1;
            freqPointer[ESC_symbol].freq += 1;
        }
        /* Increment the frequency	*/
        int j = tempSymbol;

        while (j > 0)                                    /* count for the symbol and	*/
        {                                            /* update the cumulative	*/
            j -= 1;                                    /* frequencies 				*/
            cumPointer[j].freq += 1;
        }

    }
}

struct cum_freqs *gotoTable(struct freqs freq[], struct cum_freqs cum_freq[], struct cum_freqs cum_freq_1[], const int context[], int depth){
    if(depth == 0) return cum_freq_1;
    if(depth == 1) return cum_freq;

    struct cum_freqs *currentPointerCF = cum_freq;
    struct freqs *currentPointerF = freq;

    for (int i = 0; i < depth - 1; ++i) {
        int tempSymbol = context[i];

        if(currentPointerCF[tempSymbol].next == NULL){
            return NULL;
        }

        currentPointerCF = currentPointerCF[tempSymbol].next;
        currentPointerF = currentPointerF[tempSymbol].next;
    }

    return currentPointerCF;
}

struct cum_freqs *findInTable(struct freqs freq[], struct cum_freqs cum_freq[], struct cum_freqs cum_freq_1[], const int context[], int cSize, int symbol, bool *found){
    struct cum_freqs *cfPointer = gotoTable(freq, cum_freq, cum_freq_1, context, cSize+1);
    *found = true;
    if(cfPointer == NULL){
        *found = false;
        return NULL;
    }
    if(cfPointer[symbol-1].freq == cfPointer[symbol].freq){
        *found = false;
    }
    return cfPointer;
}

void startIgnored(bool ignoredSymbols[]){
    for (int i = 0; i < No_of_symbols + 1; ++i) {
        ignoredSymbols[i] = false;
    }
}

void getNonZeroChars(struct cum_freqs cum_freq[], bool ignoredSymbols[]){
    for (int i = 2; i <= No_of_symbols; ++i) {
        if(!ignoredSymbols[i]){
            ignoredSymbols[i] = (cum_freq[i-1].freq != cum_freq[i].freq);
        }
    }
}

struct cum_freqs *createExludedTable(const bool *excludedSymbols, struct cum_freqs *inputTable){
    if(inputTable == NULL) return NULL;

    struct cum_freqs *newTable = (struct cum_freqs*)malloc(sizeof(struct cum_freqs)*(No_of_symbols + 1));

    newTable[No_of_symbols].freq = 0;
    if(excludedSymbols[No_of_symbols]) {
        newTable[No_of_symbols-1].freq = 0;
    }
    else{
        newTable[No_of_symbols-1].freq = inputTable[No_of_symbols-1].freq;
    }

    for (int i = No_of_symbols-1; i > 0; i--) {
        if(excludedSymbols[i]){
            newTable[i-1].freq = newTable[i].freq;
        }else{
            newTable[i-1].freq = newTable[i].freq + (inputTable[i-1].freq - inputTable[i].freq);
        }
        newTable[i-1].next = NULL;
    }

    return newTable;
}
