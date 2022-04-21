/* THE FIXED SOURCE MODEL */

#include "model.h"

int freq[No_of_symbols+1] = {
     0,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  124,1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 

/*       !   "   #   $   %   &   '   (   )   *   +   ,   -   .   / */
  1236,  1, 21,  9,  3,  1,  25,15,  2,  2,  2,  1, 79, 19, 60,  1,

/*   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ? */
    15, 15,  8,  5,  4,  7,   5, 4,  4,  6,  3,  2,  1,  1,  1,  1,

/*   @    A  B   C   D   E   F   G   H   I   J   K   L   M   N   O */
     1,  24,15, 22, 12, 15,  10, 9, 16, 16,  8,  6, 12, 23, 13, 11,

/*   P    Q  R  S    T   U    V  W   X   Y   Z   [   \   ]   ^   _ */ 
    14,   1,14,28,  29,  6,   3,11,  1,  3,  1,  1,  1,  1,  1,  3, 

/*   `    a  b   c   d   e    f  g   h   i   j   k   l   m   n   o */  
     1, 491,85,173,232,744, 127,11,293,418,  6, 39,250,139,429,446,

/*   p    q    r   s   t   u   v   w   x   y   z   {   |   }   ~    */
   111,   5, 388,375,531,152,  57,97, 12,101,  5,  2,  1,  2,  3,  1,

     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 
     1
};


/* INITITALISE THE MODEL */

start_model()
{ 
 int i;
 for (i=0; i<No_of_chars; i++) 
	{										/* set up tables that		*/
     char_to_index[i] = i+1;				/* translate between symbol	*/
     index_to_char[i+1] = i;				/* indexes and characters	*/
    }
 cum_freq[No_of_symbols] = 0;
 for (i = No_of_symbols; i >0; i--) 
	{ 										/* set up cumulative		*/
     cum_freq[i-1] = cum_freq[i] + freq[i];	/* frequency counts			*/
    }
 if (cum_freq[0] > Max_frequency) abort();	/* check counts within limits 	*/
}


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL */

update_model(symbol)
 int symbol;
{
 /* Do nothing 			*/
}


   

