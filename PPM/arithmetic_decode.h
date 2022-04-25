//
// Created by Victor on 22/04/2022.
//

#ifndef PPM_ARITHMETIC_DECODE_H
#define PPM_ARITHMETIC_DECODE_H

void start_decoding(FILE *file);
int decode_symbol(struct cum_freqs *cum_freq, FILE *file);

#endif //PPM_ARITHMETIC_DECODE_H
