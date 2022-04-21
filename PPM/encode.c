/* MAIN PROGRAM FOR ENCODING */

#include <stdio.h>
#include "model.h"

main()
{  
 start_model();			                 /* set up other modules.	*/
 start_outputing_bits();
 start_encoding();

 for (;;) 
	{
     int ch; int symbol;
     ch = getc(stdin);			         /* Reads the next character */
     if (ch==EOF) break;		         /* Exit loop on end-of-file */
     symbol = char_to_index[ch];	     /* Translates to an index 	 */
     encode_symbol(symbol,cum_freq);	 /* Encode that symbol.	 	 */
     update_model(symbol);		         /* Update the model 	 	 */
    }
 encode_symbol(EOF_symbol,cum_freq);	 /* Encodes the EOF symbol 	 */
 done_encoding();			             /* Send the last few bits	 */
 done_outputing_bits();
 exit(0);
}


