//
// Created by Victor on 22/04/2022.
//

#ifndef PPM_ARITHMETIC_ENCODE_H
#define PPM_ARITHMETIC_ENCODE_H

void start_encoding(int max_bits);
void encode_symbol(int symbol, const int *cum_freq, FILE *file);
void done_encoding(FILE *file);
void bit_plus_follow(int bit, FILE *file);
void escreve_indice(int indice, int bits_indice, FILE* pointf_file);

#endif //PPM_ARITHMETIC_ENCODE_H
