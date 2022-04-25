/* THE ADAPTIVE SOURCE MODEL */

#include "model.h"
#include <stdio.h>
#include "ppm.h"


/* INITIALISE THE MODEL */

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
    initTables(cum_freq, freq, context, cSize);
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

