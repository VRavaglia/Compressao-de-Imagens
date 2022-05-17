//
// Created by Victor on 16/05/2022.
// inpired by https://github.com/internaut/py-lbg/blob/master/lbg.py
//

#include <cmath>
#include "VQ.h"
#include "ImageReader.h"

fMatrix VQ::LGB(const intMatrix &blocks, const unsigned cbSize, const double eps){
    fMatrix codebook;

    unsigned sizeBlock = blocks.size();
    unsigned sizeDim = blocks[0].size();

    vector<float> c0 = vec_avg(blocks, sizeBlock, sizeDim);

    codebook.push_back(c0);

    float avg_dist = dis_avg();

    return codebook;
}

vector<float> VQ::vec_avg(const intMatrix &blocks, const unsigned sizeBlock, const unsigned sizeDim){
    vector<float> avg(sizeDim, 0.0);

    for(vector<int> block : blocks){
        for (int i = 0; i < sizeDim; ++i) {
            avg[i] += float(block[i])/float(sizeBlock);
        }
    }

    return avg;
}

float VQ::dist_avg_c0(const vector<float> &c0, const intMatrix &blocks, const unsigned size){
    return reduce(lambda s, d: s + d/size, (euclid_squared(c0, vec) for vec in data), 0.0)
}