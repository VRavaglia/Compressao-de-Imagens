/* THE ADAPTIVE SOURCE MODEL */

#include "model.h"

int freq[No_of_symbols+1];		/* Symbol frequencies		*/


/* INITIALISE THE MODEL */

start_model()
{  
 int i;
 for (i=0; i<No_of_chars; i++)
	{					 						/* Set up tables that		*/
     char_to_index[i] = i+1;					/* translate between symbol	*/
     index_to_char[i+1] = i;					/* indexes and characters	*/	
    }
 for (i=0; i<=No_of_symbols;i++) 
	{											/* Set up initial frequency	*/
     freq[i] = 1;								/* counts to be one for all	*/
     cum_freq[i] = No_of_symbols-i;				/* symbols					*/
    }
 freq[0]=0;										/* freq[0] must not be the	*/
}												/* same as freq[1]			*/


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */


update_model(symbol)
int symbol;										/* index of new symbol		*/
{ 
 int i;											/* New index for symbol		*/
 if (cum_freq[0]==Max_frequency) 				/* See if frequency counts	*/
	{											/* are at their maximum		*/
     int cum;					
     cum = 0;
     for (i = No_of_symbols; i>=0; i--) 		/* If so, halve all the 	*/
		{										/* counts ( keeping them	*/
         freq[i] = (freq[i]+1)/2;				/* non-zero).				*/
         cum_freq[i] = cum;						
         cum += freq[i];
        }
    }
 for (i = symbol; freq[i]==freq[i-1]; i--) ;	/* Find symbol's new index	*/
 if (i<symbol) 
	{
     int ch_i, ch_symbol;
     ch_i = index_to_char[i];					/* Update the translation 	*/
     ch_symbol = index_to_char[symbol];			/* tables if the symbol has	*/
     index_to_char[i] = ch_symbol;				/* moved					*/
     index_to_char[symbol] = ch_i;
     char_to_index[ch_i] = symbol;
     char_to_index[ch_symbol] = i;
   }
 freq[i] += 1;									/* Increment the frequency	*/
 while (i>0)									/* count for the symbol and	*/ 
	{											/* update the cumulative	*/
     i -= 1;									/* frequencies 				*/
     cum_freq[i] += 1;			
    }
}

