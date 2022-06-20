//
// Created by Victor on 25/04/2022.
//

#ifndef PPM_PPM_H
#define PPM_PPM_H

#include <stdbool.h>

void start_model(int freq[], int cum_freq[], int freq_size, int subband);
void update_model(int *freq, int *cum_freq, int freq_size, int symbol);

#endif //PPM_PPM_H
