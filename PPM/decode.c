/* MAIN PROGRAM FOR DECODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "bit_input.h"
#include "arithmetic_decode.h"
#include "adaptive_model.h"

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
int cum_freq[No_of_symbols + 1];            /* Cummulative symbol frequencies 	*/


main() {
    start_model();                              /* Set up other modules		*/
    start_inputing_bits();
    start_decoding();

    for (;;)                                  /* Loop through characters	*/
    {
        int ch;
        int symbol;
        symbol = decode_symbol(cum_freq);      /* decode next symbol 		*/
        if (symbol == EOF_symbol) break;          /* Exit loop if EOF symbol 	*/
        ch = index_to_char[symbol];          /* translate to a character 	*/
        putc(ch, stdout);                      /* write that character 		*/
        update_model(symbol);                  /* Update the model 			*/
    }
    exit(0);
}
