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
    static vector<intMatrix> splitSubbands(int **InputImg, int ximg, int yimg, int nsubs);
    static intMatrix quantize(const vector<intMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS]);
};


#endif //WAVELETS_WAVELETHELPER_H
