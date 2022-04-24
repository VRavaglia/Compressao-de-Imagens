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

    char *inputFilename = "biblia_facil.txt";
    char *outputFilename = "biblia_encoded.txt";
    FILE *fin = fopen(inputFilename, "rb");

    if(fin == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de entrada!");
        exit(1);
    }

    FILE *fout = fopen(outputFilename, "wb");

    if(fout == NULL)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");
        exit(1);
    }

    for (;;) {
        int ch;
        int symbol;
        ch = getc(fin);
        if (ch == EOF) break;                 /* Exit loop on end-of-file */
        symbol = char_to_index[ch];         /* Translates to an index 	 */
        encode_symbol(symbol, cum_freq, fout);     /* Encode that symbol.	 	 */
        update_model(symbol);                 /* Update the model 	 	 */
    }
    encode_symbol(EOF_symbol, cum_freq, fout);     /* Encodes the EOF symbol 	 */
    done_encoding(fout);                         /* Send the last few bits	 */
    done_outputing_bits(fout);

    fclose(fin);
    fclose(fout);

    exit(0);
}


