//
// Created by Victor on 25/04/2022.
//

#ifndef PPM_PPM_H
#define PPM_PPM_H

void check_context(int ch, struct cum_freqs *cum_freq, int maxContext, int *currentContext, int *ccSize);
int escape_count(int symbol, struct freqs *freq, int maxContext, int *currentContext, int ccSize);
void start_model(struct freqs *freq, struct cum_freqs *cum_freq);
void update_model(int symbol, struct freqs *freq, struct cum_freqs *cum_freq, int *context, int cSize);

#endif //PPM_PPM_H
