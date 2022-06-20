#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

//#define Max_frequency    16383
#define Max_frequency    131071

void start_model(int freq[], int cum_freq[], int freq_size, int subband) {
    for (int i = 0; i <= freq_size; i++) {          /* Set up initial frequency	*/
        freq[i]= 1;                                /* counts to be one for all	*/
//        if(subband > 0 && i == freq_size){
//            freq[i]= 3000;
//        }
    }
    cum_freq[freq_size] = 0;
    for (int i = freq_size-1; i >= 0; i--) {          /* Set up initial frequency	*/         /* counts to be one for all	*/
        cum_freq[i] = cum_freq[i+1] + freq[i];         /* symbols					*/
    }
//    cum_freq[0].freq = 1;
    freq[0] = 0;                                        /* freq_1[0] must not be the	*/
}                                                /* same as freq_1[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


void update_model(int *freq, int *cum_freq, int freq_size, int symbol)
{

    if (cum_freq[0] == Max_frequency)                /* See if frequency counts	*/
    {                                            /* are at their maximum		*/
        int cum = 0;
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

