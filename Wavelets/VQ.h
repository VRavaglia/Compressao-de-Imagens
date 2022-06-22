//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_VQ_H
#define VQ_VQ_H

#include <vector>
#include <string>
#include "ImageReader.h"
#include <map>
#include "cb_list.h"

using intMatrix = vector<vector<int>>;
using fMatrix = vector<vector<float>>;
using namespace std;

// Dimensoes de blocos que serao testados/treinados
//static const unsigned vl_size = 16;
//static const unsigned vector_list[vl_size][2] = {{1,1},
//                                           {1,2},
//                                           {2,1},
//                                           {2,2},
//                                          {1,4},
//                                          {4,1},
//                                           {2,4},
//                                           {4,2},
//                                           {4,4},
//                                         {1,8},
//                                         {8,1},
//                                         {2,8},
//                                         {8,2},
//                                           {4,8},
//                                           {8,4},
//                                           {8,8},};
//static const unsigned vl_size = 4;
//static const unsigned vector_list[vl_size][2] = {{1,1},
//                                                 {2,1},};
//static const unsigned vector_list[vl_size][2] = {{1,1},
//                                                 {2, 1},
//                                                 {1, 2},
//                                                 {2, 2},};

// Tamanhos de codebooks
//static const unsigned cb_size_size = 10;
//static const unsigned cb_size_list[cb_size_size] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
//static const unsigned cb_size_size = 5;
//static const unsigned cb_size_list[cb_size_size] = {16, 32, 64, 128, 256};
//25, 10
//#define LAMBDA 10
#define  MAXR 15
#define USADIST false
#define TWOPASSES true
#define  MAXVQITS 20
#define MAXBLOCK 30
//static const float lambdas[7] = {200, 100, 25, 10, 5, 2.5, 1};
static const float lambdas[1] = {100};

struct performance{
    unsigned codebook_size = 0;
    unsigned codebook_idx = 0;
    double MSE = 0;
    double R = 0;
    vector<int> block_size{0, 0};
//    vector<int> blockList;
};

struct codebookInfo
{
    unsigned cbSize = 0;
    unsigned blockH = 0;
    unsigned blockW = 0;
    unsigned blocks = 0;
};



class VQ {
private:
    static vector<float> vec_avg(const fMatrix &blocks, unsigned sizeBlock, unsigned sizeDim);
    static double dist_avg_c0(const vector<float> &c0, const fMatrix &blocks);
    static double euclid_squared(const vector<float> &a, const vector<float> &b);
    static int split_codebook(const fMatrix &blocks, fMatrix &codebook, float eps, float parada, double &initial_avg_dist);
    static vector<float> new_codevector(const vector<float> &c, float e);
    static double avg_dist_c_list(const map<unsigned , vector<float>> &c_list, const fMatrix &blocks, const unsigned &bSize);
public:
    static int LGB(const fMatrix &blocks, unsigned cbSize, float eps, float parada, fMatrix &codebook);
    static fMatrix replaceBlocks(const fMatrix &blocks, const fMatrix &codebook, const vector<int>& bDims, const unsigned *fDims, vector<int> &blockList);
    static void save_codebooks(const string& filename, const vector<fMatrix> &codebook_list, const intMatrix& dims);
    static vector<fMatrix> load_codebooks(const string& filename);
//    static vector<unsigned> best_codebook(const fMatrix &image, const vector<fMatrix> &block_list, const vector<fMatrix> &codebook_list, const unsigned *dims, unsigned testIdx);
    static double MSE(const fMatrix &oldI, const fMatrix &newI);
    static double PSNR(const fMatrix &oldI, const fMatrix &newI);
    static vector<vector<performance>> evaluate_codebooks(const vector<fMatrix> &subbands, vector<intMatrix>& bb_idx, bool second_pass, const vector<vector<vector<vector<int>>>>& all_bb_idx, vector<vector<int*>> all_models);
    static fMatrix fill_image(const intMatrix &allBlocks, const vector<fMatrix> &selected_codebooks, const vector<codebookInfo>& selected_infos, unsigned *dims);
    static void save_performances(const vector<vector<vector<performance>>> &performances, const vector<string>& img_name, bool second_pass);
    static vector<vector<performance>> load_performances(const string& filename);
    static void save_histograms(const vector<vector<vector<vector<int>>>> &idx_list, const vector<vector<vector<performance>>>& performances);
    static void save_histograms2(const vector<vector<vector<vector<int>>>> &idx_list, const vector<vector<vector<performance>>>& performances);
    static int *load_model(int sband, int cb_idx);
    static int *load_model2(int sband, int cb_idx);
    static int *load_model_known(int sband, int cb_idx, vector<vector<vector<vector<int>>>> all_bb_idx, unsigned cbSize);
};


#endif //VQ_VQ_H
