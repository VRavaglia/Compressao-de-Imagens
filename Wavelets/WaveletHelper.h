//
// Created by Victor on 31/05/2022.
//

#ifndef WAVELETS_WAVELETHELPER_H
#define WAVELETS_WAVELETHELPER_H
#include "ImageReader.h"
#include "VQ.h"
#include "subdefs2.h"

class WaveletHelper {
public:
    static vector<fMatrix> splitSubbands(double **InputImg, int ximg, int yimg, int nsubs);
    static intMatrix quantize_1(const vector<fMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS], int imgSize, int R_method, unsigned best_idx_list[NBANDS]);
    static intMatrix quantize_2(const vector<fMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS]);
    static vector<int> convex(const vector<performance> &performances);
    static int best_lambda(const vector<performance> &performances, float lambda);
};


#endif //WAVELETS_WAVELETHELPER_H
