/* MAIN PROGRAM FOR ENCODING */

#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "adaptive_model.h"
#include "bit_output.h"
#include "arithmetic_encode.h"

int char_to_index[No_of_chars];                   /* To index from charater */
unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */
int cum_freq[No_of_symbols + 1];            /* Cummulative symbol frequencies 	*/

int main() {
    start_model();                             /* set up other modules.	*/
    start_outputing_bits();
    start_encoding();

    for (;;) {
        int ch;
        int symbol;
        ch = getc(stdin);                     /* Reads the next character */
        if (ch == EOF) break;                 /* Exit loop on end-of-file */
        symbol = char_to_index[ch];         /* Translates to an index 	 */
        encode_symbol(symbol, cum_freq);     /* Encode that symbol.	 	 */
        update_model(symbol);                 /* Update the model 	 	 */
    }
    encode_symbol(EOF_symbol, cum_freq);     /* Encodes the EOF symbol 	 */
    done_encoding();                         /* Send the last few bits	 */
    done_outputing_bits();
    exit(0);
}


