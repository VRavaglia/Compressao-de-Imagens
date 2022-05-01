#pragma once
/* INTERFACE TO THE MODEL */



/* THE SET OF SYMBOLS THAT MAY BE ENCODED */

#define No_of_chars    256                        /* Number of character symbols */

#define ESC_symbol    (1)        /* Index of ESCAPE symbol	*/
#define EOF_symbol    (No_of_chars + 2)        /* Index of EOF symbol	*/

#define No_of_symbols    (No_of_chars + 2)    /* Total number of symbols */

#define Max_frequency    16383            /* Maximum allowed frequency count 	*/

#define Max_context -1

struct cum_freqs{
    int freq;
    struct cum_freqs *next;
};

struct freqs{
    int freq;
    struct freqs *next;
};


extern int char_to_index[No_of_chars];                   /* To index from charater */
extern unsigned char index_to_char[No_of_symbols + 1];  /* To character from index */

