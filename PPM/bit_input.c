/* BIT INPUT ROUTINES */

#include <stdio.h>
#include "arithmetic_coding.h"

/* The BIT BUFFER */

static int buffer;			/* Bits waiting to be input 		*/
static int bits_to_go;		/* Number of bits still in buffer	*/
static int garbage_bits;	/* Number of bits past end-of-file 	*/


/* INITIALISE BIT INPUT */

start_inputing_bits()
{ 
 bits_to_go = 0;						/* Buffer starts out with		*/
 garbage_bits = 0;						/* no bits in it.			*/
}


/* INPUT A BIT  */

int input_bit()
{ 
 int t;
 if (bits_to_go==0) 		            		/* read the next byte if no	*/
    {  
	 buffer = getc(stdin);	            		/* bits are left in buffer	*/
     if (buffer==EOF) 
		{
         garbage_bits +=1;						/* Return arbitrary bits 	*/
         if (garbage_bits>Code_value_bits-2) 	/* after EOF, but check		*/
			{									/* for too many such	*/
             fprintf(stderr,"Bad input file\n");	
             exit(-1);
            }
        }
     bits_to_go = 8;
    }
 t = buffer&1;									/* Return the next bit from	*/
 buffer >>= 1;									/* the bottom of the byte 	*/
 bits_to_go -= 1;
 return(t);
}
