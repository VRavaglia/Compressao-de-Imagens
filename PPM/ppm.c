#include <stdio.h>
#include <stdlib.h>
#include "model.h"


void check_context(int ch, struct cum_freqs cum_freq[], int maxContext, int currentContext[], int *ccSize){
    if (maxContext > 0){
        if(*ccSize < maxContext + 1){
            currentContext[*ccSize] = char_to_index[ch];
            *ccSize = *ccSize + 1;
        }
        else{
            for (int i = 0; i < maxContext; ++i) {
                currentContext[i] = currentContext[i+1];
            }
            currentContext[maxContext] = char_to_index[ch];
        }
    }
}

// Se necessario, inicializa as tabelas que indicam as frequencias de cada sequencia de simbolos
void initTables(struct cum_freqs cum_freq[], struct freqs freq[], const int currentContext[], int ccSize){
    struct freqs *currentPointerF = &freq[currentContext[0]];
    struct cum_freqs *currentPointerCF = &cum_freq[currentContext[0]];

    for (int i = 0; i < ccSize-1; ++i) {
        if(currentPointerF->next == NULL){
            currentPointerF->next = (struct freqs*)malloc(sizeof(struct freqs)*(No_of_symbols + 2));
            currentPointerCF->next = (struct cum_freqs*)malloc(sizeof(struct cum_freqs)*(No_of_symbols + 2));

            currentPointerF->next[currentContext[i+1]].freq = 1;
            currentPointerCF->next[currentContext[i+1]].freq = 2;

            currentPointerF = currentPointerF->next;
            currentPointerCF = currentPointerCF->next;


        }else{
            currentPointerF->next[currentContext[i+1]].freq += 1;
            currentPointerCF->next[currentContext[i+1]].freq += 1;
        }
    }
}

void start_model(struct freqs freq[], struct cum_freqs cum_freq[]) {
    int i;
    for (i = 0; i < No_of_chars; i++) {              /* Set up tables that		*/
        char_to_index[i] = i + 1;                    /* translate between symbol	*/
        index_to_char[i + 1] = i;                    /* indexes and characters	*/
    }
    for (i = 0; i <= No_of_symbols; i++) {          /* Set up initial frequency	*/
        freq[i].freq = 1;                                /* counts to be one for all	*/
        freq[i].next = NULL;
        cum_freq[i].freq = No_of_symbols - i;        /* symbols					*/
        cum_freq[i].next = NULL;
    }
    freq[0].freq = 0;                                        /* freq[0] must not be the	*/
}                                                /* same as freq[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


void update_model(int symbol, struct freqs freq[], struct cum_freqs cum_freq[], int context[], int cSize)
{
//    initTables(cum_freq, freq, context, cSize);
    int i;                                            /* New index for symbol		*/
    if (cum_freq[0].freq == Max_frequency)                /* See if frequency counts	*/
    {                                            /* are at their maximum		*/
        int cum;
        cum = 0;
        for (i = No_of_symbols; i >= 0; i--)        /* If so, halve all the 	*/
        {                                        /* counts ( keeping them	*/
            freq[i].freq = (freq[i].freq + 1) / 2;                /* non-zero).				*/
            cum_freq[i].freq = cum;
            cum += freq[i].freq;
        }
    }
    for (i = symbol; freq[i].freq == freq[i - 1].freq; i--);    /* Find symbol's new index	*/
    if (i < symbol) {
        int ch_i, ch_symbol;
        ch_i = index_to_char[i];                    /* Update the translation 	*/
        ch_symbol = index_to_char[symbol];            /* tables if the symbol has	*/
        index_to_char[i] = ch_symbol;                /* moved					*/
        index_to_char[symbol] = ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i].freq += 1;                                    /* Increment the frequency	*/
    while (i > 0)                                    /* count for the symbol and	*/
    {                                            /* update the cumulative	*/
        i -= 1;                                    /* frequencies 				*/
        cum_freq[i].freq += 1;
    }
}

