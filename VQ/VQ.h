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

// Dimensoes de blocos que serao testados/treinados
static const unsigned vl_size = 10;
static const unsigned vector_list[vl_size][2] = {{1,1},
                                           {1,2},
                                           {2,1},
                                           {2,2},
                                           {2,4},
                                           {4,2},
                                           {4,4},
                                           {4,8},
                                           {8,4},
                                           {8,8},};


// Tamanhos de codebooks
static const unsigned cb_size_size = 11;
static const unsigned cb_size_list[cb_size_size] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2, 4096*4};
static const bool warp = false;

// PSNR minima utilizada na hora de julgar o codebook como sendo o melhor para uma dada imagem
static const unsigned minPSNR = 30;

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
    static void save_codebooks(const string& filename, const vector<fMatrix> &codebook_list, const intMatrix& dims);
    static vector<fMatrix> load_codebooks(const string& filename);
    static vector<unsigned> best_codebook(const intMatrix &image, const vector<fMatrix> &block_list, const vector<fMatrix> &codebook_list, const unsigned *dims, unsigned testIdx);
    static double MSE(const intMatrix &oldI, const fMatrix &newI);
    static double PSNR(const intMatrix &oldI, const fMatrix &newI);
};


#endif //VQ_VQ_H
