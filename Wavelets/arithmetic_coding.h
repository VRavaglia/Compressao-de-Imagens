/* DECLARATIONS USED FOR ARITHMETIC ENCODING AND DECODING */


/* SIZE OF ARITHMETIC CODE VALUES */

#include <stdio.h>
#include <stdint.h>

typedef uint64_t code_value;        /* Type of an arithmetic code value */

//#define Code_value_bits    32        /* Number of bits in a code value */
#define Code_value_bits    32        /* Number of bits in a code value */
//#define Top_value    (((long)1<<Code_value_bits)-1)    /* Largest code value */
#define Top_value    (((uint64_t)1<<Code_value_bits)-1)    /* Largest code value */
#define    First_qtr    (Top_value/4+1)    /* Point after first quarter 	*/
#define Half        (2*First_qtr)    /* Point after first half 		*/
#define Third_qtr    (3*First_qtr)    /* Point after third quarter 	*/


