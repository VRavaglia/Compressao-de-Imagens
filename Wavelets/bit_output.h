//
// Created by Victor on 22/04/2022.
//

#ifndef PPM_BIT_OUTPUT_H
#define PPM_BIT_OUTPUT_H

#include <stdio.h>

void start_outputing_bits();
void output_bit(int bit, FILE *file);
void done_outputing_bits(FILE *file);

#endif //PPM_BIT_OUTPUT_H
