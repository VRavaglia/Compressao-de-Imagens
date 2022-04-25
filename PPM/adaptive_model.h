//
// Created by Victor on 22/04/2022.
//

#ifndef PPM_ADAPTIVE_MODEL_H
#define PPM_ADAPTIVE_MODEL_H

void start_model(struct freqs *freq, struct cum_freqs *cum_freq);

void update_model(int symbol, struct freqs *freq, struct cum_freqs *cum_freq, int *context, int cSize);

#endif //PPM_ADAPTIVE_MODEL_H
