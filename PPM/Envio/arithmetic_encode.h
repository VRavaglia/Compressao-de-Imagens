//
// Created by Victor on 22/04/2022.
//

#ifndef PPM_ARITHMETIC_ENCODE_H
#define PPM_ARITHMETIC_ENCODE_H

void start_encoding();
void encode_symbol(int symbol, struct cum_freqs *cum_freq, FILE *file);
void done_encoding(FILE *file);

#endif //PPM_ARITHMETIC_ENCODE_H
