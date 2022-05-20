//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_VQ_H
#define VQ_VQ_H

#include <vector>
#include <string>
#include "ImageReader.h"
#include <map>

using namespace std;

static const unsigned vector_list[7][2] = {{1,1},
                                           {1,2},
                                           {2,2},
                                           {2,4},
                                           {4,4},
                                           {8,4},
                                           {8,8},};

class VQ {
private:
    static vector<float> vec_avg(const fMatrix &blocks, unsigned sizeBlock, unsigned sizeDim);
    static double dist_avg_c0(const vector<float> &c0, const fMatrix &blocks);
    static double euclid_squared(const vector<float> &a, const vector<float> &b);
    static void split_codebook(const fMatrix &blocks, fMatrix &codebook, float eps, double &initial_avg_dist);
    static vector<float> new_codevector(const vector<float> &c, float e);
    static double avg_dist_c_list(const map<unsigned , vector<float>> &c_list, const fMatrix &blocks, const unsigned &bSize);
public:
    static fMatrix LGB(const fMatrix &blocks, unsigned cbSize, float eps);
    static fMatrix replaceBlocks(const fMatrix &blocks, const fMatrix &codebook, const unsigned *bDims, const unsigned *fDims);
};


#endif //VQ_VQ_H
