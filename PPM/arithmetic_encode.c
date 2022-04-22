/* ARITHMETIC ENCODING ALGORITHM.	*/

#include "arithmetic_coding.h"
#include "bit_output.h"

static void bit_plus_follow();        /* Routine that follows	*/


/* CURRENT STATE OF THE ENCODING */

static code_value low, high;        /* ends of the current code-region */
static long bits_to_follow;        /* Number of opposite bits to output after	*/
/* the next bit.				            */

/* START ENCODING A STREAM OF SYMBOLS */

void start_encoding() {
    low = 0;                        /* Full code range */
    high = Top_value;
    bits_to_follow = 0;            /* No bits to follow next */
}


/* ENCODE A SYMBOL */

void encode_symbol(symbol, cum_freq)
        int symbol;                        /* Symbol to encode 				*/
        int cum_freq[];                    /* Cummulative symbol frequencies	*/
{
    long range;                    /* size of the current_code region	*/
    range = (long) (high - low) + 1;                            /* Narrow the code  */
    high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1; /* region to that   */
    low = low + (range * cum_freq[symbol]) / cum_freq[0];     /* allotted to this */
/* symbol           */

    for (;;)
    {                                            /* Loop to output bits	*/
        if (high<Half)
        {
            bit_plus_follow(0);                    /* Output 0 if in low half */
        }
        else if (low>=Half)
        {                                    /* Output 1 if in high half	*/
            bit_plus_follow(1);
            low -= Half;
            high -= Half;                        /* Subtract offset to top */
        }
        else if (low>=First_qtr && high<Third_qtr)
        {                                /* Output an opposite bit   */
/* later if in middle half.	*/
            bits_to_follow += 1;
            low -= First_qtr;                /* Subtract offset to middle */
            high -= First_qtr;
        }
        else break;                        /* Otherwise exit loop */
        low = 2 * low;
        high = 2 * high + 1;                        /* Scale up code range */
    }
}

/* FINISH ENCODING THE STREAM */

void done_encoding() {
    bits_to_follow += 1;                        /* Output two bits that 		*/
    if (low < First_qtr) bit_plus_follow(0);    /* select the quarter that 		*/
    else bit_plus_follow(1);                    /* the current code range		*/
}                                            /* contains						*/


/* OUTPUT BITS PLUS THE FOLLOWING OPPOSITE BITS */

static void bit_plus_follow(bit)
        int bit;
{
    output_bit(bit);                            /* Output the bit				*/
    while (bits_to_follow > 0) {
        output_bit(!bit);                        /* Ouput bits_to_follow			*/
        bits_to_follow -= 1;                    /* opposite bits. Set			*/
    }                                        /* bits_to_follow to zero		*/
}
            




