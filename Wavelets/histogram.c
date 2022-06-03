#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Max_frequency    16383

void start_model(int freq[], int cum_freq[], int freq_size) {
    for (int i = 0; i <= freq_size; i++) {          /* Set up initial frequency	*/
        freq[i]= 1;                                /* counts to be one for all	*/
        cum_freq[i] = freq_size - i;        /* symbols					*/
    }
//    cum_freq[0].freq = 1;
    freq[0] = 0;                                        /* freq_1[0] must not be the	*/
}                                                /* same as freq_1[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


void update_model(int *freq, int *cum_freq, int freq_size, int symbol)
{

    if (cum_freq[0] == Max_frequency)                /* See if frequency counts	*/
    {                                            /* are at their maximum		*/
        int cum;
        cum = 0;
        for (int j = freq_size; j >= 0; j--)        /* If so, halve all the 	*/
        {                                        /* counts ( keeping them	*/
            freq[j] = (freq[j] + 1) / 2;                /* non-zero).				*/
            cum_freq[j] = cum;
            cum += freq[j];
        }
    }

    freq[symbol] += 1;

    /* Increment the frequency	*/
    int j = symbol;

    while (j > 0)                                    /* count for the symbol and	*/
    {                                            /* update the cumulative	*/
        j -= 1;                                    /* frequencies 				*/
        cum_freq[j] += 1;
    }


}

