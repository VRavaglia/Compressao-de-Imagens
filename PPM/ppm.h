//
// Created by Victor on 25/04/2022.
//

#ifndef PPM_PPM_H
#define PPM_PPM_H

#include <stdbool.h>

void check_context(int ch, int maxContext, int *currentContext, int *ccSize);
void start_model(struct freqs *freq, struct cum_freqs *cum_freq);
void create_table(struct freqs freq[], struct cum_freqs cum_freq[]);
void update_model(struct freqs *freq, struct cum_freqs *cum_freq, const int *context, int cSize, int symbol, int maxDepth);
struct cum_freqs *gotoTable(struct freqs freq[], struct cum_freqs cum_freq[], struct cum_freqs cum_freq_1[], const int context[], int depth);
struct cum_freqs * findInTable(struct freqs *freq, struct cum_freqs *cum_freq, struct cum_freqs *cum_freq_1, const int *context, int cSize, int symbol);

#endif //PPM_PPM_H
