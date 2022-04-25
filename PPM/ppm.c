#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "adaptive_model.h"


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

            currentPointerF->next[currentContext[i+1]].freq = 2;
            currentPointerCF->next[currentContext[i+1]].freq = 2;

            currentPointerF = currentPointerF->next;
            currentPointerCF = currentPointerCF->next;


        }else{
            currentPointerF->next[currentContext[i+1]].freq += 1;
            currentPointerCF->next[currentContext[i+1]].freq += 1;
        }
    }
}

