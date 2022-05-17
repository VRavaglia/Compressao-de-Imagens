//
// Created by Victor on 16/05/2022.
//https://github.com/internaut/py-lbg/blob/53c1c77f67192745c27f1dfbee9ef976e9156f35/lbg.py#L145
//

#include <cmath>
#include "VQ.h"
#include "ImageReader.h"

fMatrix VQ::LGB(const fMatrix &blocks, const unsigned cbSize, const float eps){
    fMatrix codebook;

    unsigned sizeBlock = blocks.size();
    unsigned sizeDim = blocks[0].size();

    vector<float> c0 = vec_avg(blocks, sizeBlock, sizeDim);

    codebook.push_back(c0);

    double avg_dist = dist_avg_c0(c0, blocks);

    while (codebook.size() < cbSize){
        split_codebook(blocks, codebook, eps, avg_dist);
    }


    return codebook;
}

vector<float> VQ::vec_avg(const fMatrix &blocks, const unsigned sizeBlock, const unsigned sizeDim){
    vector<float> avg(sizeDim, 0.0);

    for(vector<float> block : blocks){
        for (int i = 0; i < sizeDim; ++i) {
            avg[i] += float(block[i])/float(sizeBlock);
        }
    }

    return avg;
}

double VQ::dist_avg_c0(const vector<float> &c0, const fMatrix &blocks){
    double avg_d = 0;
    unsigned block_size = blocks.size();

    for (vector<float> block : blocks) {
        avg_d += euclid_squared(c0, vector<float>(block.begin(), block.end()))/block_size;
    }

    return avg_d;
}

double VQ::euclid_squared(const vector<float> &a, const vector<float> &b){
    double d = 0;

    for (int i = 0; i < a.size(); ++i) {
        d += pow(a[i]-b[i], 2);
    }

    return d;
}
void VQ::split_codebook(const fMatrix &blocks, fMatrix &codebook, const float eps, double &initial_avg_dist){

    fMatrix  new_codevectors;

    for (const vector<float> &c : codebook){
        vector<float> c1 = new_codevector(c, eps);
        vector<float> c2 = new_codevector(c, -eps);
        new_codevectors.push_back(c1);
        new_codevectors.push_back(c2);
    }

    codebook = new_codevectors;

    double avg_dist = 0;
    double err = eps + 1;
    unsigned bSize = blocks.size();
    unsigned cSize = codebook.size();

    while(err > eps){
        fMatrix closest_c_list tem que inicializar;

        for (int i = 0; i < bSize; ++i) {
            double min_dist = -1;
            int closest_c_index = -1;

            for (int j = 0; j < cSize; ++j) {
                double d = euclid_squared(blocks[i],codebook[j]);
                if (min_dist < 0 || d < min_dist){
                    min_dist = d;
                    closest_c_list[i] = codebook[j];
                    closest_c_index = j;
                }
            }
        }
    }

}

vector<float> VQ::new_codevector(const vector<float> &c, float e){
    vector<float> nc;

    for(float x : c){
        nc.push_back(x * (1 + e));
    }

    return nc;
}