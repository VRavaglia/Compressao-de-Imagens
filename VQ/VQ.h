//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_VQ_H
#define VQ_VQ_H

#include <vector>
#include "ImageReader.h"

using namespace std;

using intMatrix = vector<vector<int>>;
using fMatrix = vector<vector<float>>;

static const unsigned vector_list[7][2] = {{1,1},
                                           {1,2},
                                           {2,2},
                                           {2,4},
                                           {4,4},
                                           {8,4},
                                           {8,8},};

class VQ {
public:
    static fMatrix LGB(const intMatrix &blocks, unsigned cbSize, double eps);
    static vector<float> vec_avg(const intMatrix &blocks, unsigned sizeBlock, unsigned sizeDim);
    static float dist_avg_c0(const vector<float> &c0, const intMatrix &blocks, unsigned size);
};


#endif //VQ_VQ_H
