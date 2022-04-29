#include <stdio.h>
#include <stdlib.h>
#include "model.h"


struct cum_freqs [*]gotoTable(int escapeCount, struct cum_freqs cum_freq[], struct cum_freqs cum_freq_1[], const int context[], int maxContext, int maxDepth){

    int tabIndex = maxDepth - escapeCount;

    if (tabIndex == 0) return cum_freq_1;

    struct cum_freqs *currentPointer = cum_freq;

    for (int i = 0; i < tabIndex-1; ++i) {
        currentPointer = currentPointer[context[maxDepth - i - 1]].next;
    }
    return lsita de tabelas percorridas


    //    if (escapeCount == maxDepth){
//        return cum_freq_1;
//    }
//
//    struct cum_freqs *currentPointer = cum_freq;
//    for (int i = maxDepth-escapeCount-1; i >= 0; --i) {
//        currentPointer = currentPointer[context[i]].next;
//    }
//    return currentPointer;

}

void check_context(int ch, int maxContext, int currentContext[], int *ccSize){
    if (maxContext > 0){
        if(*ccSize < maxContext){
            currentContext[*ccSize] = char_to_index[ch];
            *ccSize = *ccSize + 1;
        }
        else{
            for (int i = 0; i < maxContext; ++i) {
                currentContext[i] = currentContext[i+1];
            }
            currentContext[maxContext-1] = char_to_index[ch];
        }
    }
}

//// Se necessario, inicializa as tabelas que indicam as frequencias de cada sequencia de simbolos
//void initTables(struct cum_freqs cum_freq_1[], struct freqs freq_1[], const int currentContext[], int ccSize){
//    struct freqs *currentPointerF = &freq_1[currentContext[0]];
//    struct cum_freqs *currentPointerCF = &cum_freq_1[currentContext[0]];
//
//    for (int i = 0; i < ccSize-1; ++i) {
//        if(currentPointerF->next == NULL){
//            currentPointerF->next = (struct freqs*)malloc(sizeof(struct freqs)*(No_of_symbols + 2));
//            currentPointerCF->next = (struct cum_freqs*)malloc(sizeof(struct cum_freqs)*(No_of_symbols + 2));
//
//            currentPointerF->next[currentContext[i+1]].freq_1 = 1;
//            currentPointerCF->next[currentContext[i+1]].freq_1 = 2;
//
//            currentPointerF = currentPointerF->next;
//            currentPointerCF = currentPointerCF->next;
//
//
//        }else{
//            currentPointerF->next[currentContext[i+1]].freq_1 += 1;
//            currentPointerCF->next[currentContext[i+1]].freq_1 += 1;
//        }
//    }
//}

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
    fPointer[ESC_symbol].freq = 1;
    fcPointer[ESC_symbol].freq = 1;
    fcPointer[ESC_symbol-1].freq = 2;
}

int escape_count(struct freqs freq[], const int currentContext[], int ccSize, int maxContext, int maxDepth){
    int escapes = maxDepth;
    if (maxDepth < 1){
        return 0;
    }

    struct freqs *freqPointer = freq;
//    if(freqPointer == NULL) return  escapes;

    for (int i = ccSize-1; i >= 0; --i) {
        if(freqPointer[currentContext[i]].freq > 0){
            escapes -= 1;
            if(freqPointer[currentContext[i]].next != NULL){
                freqPointer = freqPointer[currentContext[i]].next;
            }
            else{
                return escapes;
            }
        }
        else{
            return escapes;
        }
    }
    return escapes;
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
    freq[0].freq = 0;                                        /* freq_1[0] must not be the	*/
}                                                /* same as freq_1[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


void update_model(struct freqs freq[], struct cum_freqs cum_freq[], const int context[], int cSize)
{
    struct freqs *freqPointer = freq;
    struct cum_freqs *cumPointer = cum_freq;

    for (int i = cSize-1; i >= 0; --i) {
//            if (i > 0) freqPointer[context[i]].freq_1 += 1;
        int symbol =  context[i];
        if(freqPointer[symbol].next == NULL){
            create_table(&freqPointer[symbol], &cumPointer[symbol]);
        }


        if (cumPointer[0].freq == Max_frequency)                /* See if frequency counts	*/
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

        freqPointer[symbol].freq += 1;
        if (freqPointer[symbol].freq == 1){
            cumPointer[ESC_symbol].freq += 1;
            cumPointer[0].freq += 1;
            freqPointer[ESC_symbol].freq += 1;
        }
        /* Increment the frequency	*/
        int j = symbol+1;

        while (j > 0)                                    /* count for the symbol and	*/
        {                                            /* update the cumulative	*/
            j -= 1;                                    /* frequencies 				*/
            cumPointer[j].freq += 1;
        }

        freqPointer = freqPointer[symbol].next;
        cumPointer = cumPointer[symbol].next;
    }
}

