//
// Created by Victor on 31/05/2022.
//

#ifndef WAVELETS_WAVELETHELPER_H
#define WAVELETS_WAVELETHELPER_H
#include "ImageReader.h"

class WaveletHelper {
public:
    static vector<intMatrix> splitSubbands(int **InputImg, int ximg, int yimg, int nsubs);
};


#endif //WAVELETS_WAVELETHELPER_H
