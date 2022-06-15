//
// Created by Victor on 31/05/2022.
//

#include <cmath>
#include "WaveletHelper.h"
#include "VQ.h"
#include "subdefs2.h"

vector<fMatrix> WaveletHelper::splitSubbands(double **InputImg, int ximg, int yimg, int nsubs){
    vector<fMatrix> subbands;
    fMatrix lll;

    for (int j = 0; j < yimg/(pow(2, nsubs)); ++j) {
        vector<float> row;
        for (int i = 0; i < ximg/(pow(2, nsubs)); ++i) {
            row.push_back((float)InputImg[j][i]);
        }
        lll.push_back(row);
    }
    subbands.push_back(lll);

    for (int i = 1; i < nsubs+1; ++i) {
        for (int j = 0; j < 3; ++j) {
            fMatrix temp;
            int mWidth = (int)round(ximg/(pow(2, i)));
            int mHeigth = (int)round(yimg/(pow(2, i)));
            int startPosMult[3][2] = {{1, 0}, {1,1}, {0, 1}};
            int startX = startPosMult[j][0]*mWidth;
            int startY = startPosMult[j][1]*mHeigth;
            for (int l = startY; l < startY+mHeigth; ++l) {
                vector<float> col;
                for (int k = startX; k < startX + mWidth; ++k) {
                    col.push_back((float)InputImg[l][k]);
                }
                temp.push_back(col);
            }
            printf("\n SBSize: %i", mWidth*mHeigth);
            subbands.push_back(temp);
        }
    }
    
    return subbands;
}

intMatrix WaveletHelper::quantize_1(const vector<fMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS]){
    intMatrix newBlocks;
    for (int i = 0; i < NBANDS; ++i) {
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(i) + ".txt");
        vector<int> convex = WaveletHelper::convex(performances[i]);
//        unsigned minIdx =  WaveletHelper::best_lambda(performances[i], lambda);
        double minJ = pow(10, 4);
//        double minJ = performances[i][minIdx].MSE + lambda*performances[i][minIdx].R;
        unsigned minIdx = 0;
        for (int k : convex) {
            double R = performances[i][k].R;

            if(R > 0){
                unsigned dims[3] = {(unsigned)oldSubbands[i].size(), (unsigned)oldSubbands[i][0].size(), 255};
                vector<int> bestblockList;
                fMatrix old_blocks = ImageReader::getBlocks(performances[i][k].block_size, oldSubbands[i]);
                fMatrix newImage = VQ::replaceBlocks(old_blocks, codebook_list[performances[i][k].codebook_idx], performances[i][k].block_size, dims, bestblockList);

                double mse = VQ::MSE(oldSubbands[i], newImage);

                double J = mse + lambda*R;
                if (J < minJ){
                    minJ = J;
                    minIdx = k;
//                    printf("\nNewminJ %f", minJ);
                }
            }
        }
//        printf("\nBestJ: %f/%i", minJ, minIdx);
        unsigned dims[3] = {(unsigned)oldSubbands[i].size(), (unsigned)oldSubbands[i][0].size(), 255};
        vector<int> bestblockList;


//        printf("\nCodebook loaded: %i/%i", i,NBANDS);
        fMatrix old_blocks = ImageReader::getBlocks(performances[i][minIdx].block_size, oldSubbands[i]);
//        printf("\nImage read");
        fMatrix newImage = VQ::replaceBlocks(old_blocks, codebook_list[performances[i][minIdx].codebook_idx], performances[i][minIdx].block_size, dims, bestblockList);
//        newBlocks.push_back(performances[i][minIdx].blockList);
        newBlocks.push_back(bestblockList);
        bestCodebooks[i] = performances[i][minIdx].codebook_idx;

        printf("\n A: MinJ: %f - MSE: %f R: %f - cbSize: %i", minJ, performances[i][minIdx].MSE, performances[i][minIdx].R, performances[i][minIdx].codebook_size);
//        for (int j = 0; j < performances[i][minIdx].blockList.size(); ++j) {
//            printf(" %i", performances[i][minIdx].blockList[j]);
//        }
    }

    return newBlocks;
}

intMatrix WaveletHelper::quantize_2(const vector<fMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS]){
    intMatrix newBlocks;
    for (int i = 0; i < NBANDS; ++i) {
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(i) + ".txt");
        unsigned minIdx =  WaveletHelper::best_lambda(performances[i], lambda);
//        printf("\nBestJ: %f/%i", minJ, minIdx);
        unsigned dims[3] = {(unsigned)oldSubbands[i].size(), (unsigned)oldSubbands[i][0].size(), 255};
        vector<int> bestblockList;


//        printf("\nCodebook loaded: %i/%i", i,NBANDS);
        fMatrix old_blocks = ImageReader::getBlocks(performances[i][minIdx].block_size, oldSubbands[i]);
//        printf("\nImage read");
        fMatrix newImage = VQ::replaceBlocks(old_blocks, codebook_list[performances[i][minIdx].codebook_idx], performances[i][minIdx].block_size, dims, bestblockList);
//        newBlocks.push_back(performances[i][minIdx].blockList);
        newBlocks.push_back(bestblockList);
        bestCodebooks[i] = performances[i][minIdx].codebook_idx;

        printf("\n B: - MSE: %f R: %f - cbSize: %i", performances[i][minIdx].MSE, performances[i][minIdx].R, performances[i][minIdx].codebook_size);
//        for (int j = 0; j < performances[i][minIdx].blockList.size(); ++j) {
//            printf(" %i", performances[i][minIdx].blockList[j]);
//        }
    }

    return newBlocks;
}

vector<int> WaveletHelper::convex(const vector<performance> &performances) {
    vector<int> conv;
    float lambda_list[7] = {200, 100, 25, 10, 5, 2.5, 1};
    for (float lb : lambda_list) {
        conv.push_back((int)WaveletHelper::best_lambda(performances, lb));
    }

    return conv;
}

int WaveletHelper::best_lambda(const vector<performance> &performances, float lambda) {
    double minJ = pow(10, 4);
    unsigned minIdx = 0;
    for (int k = 0; k< performances.size(); k++) {
        double R = performances[k].R;
        if(R > 0){
            double J = performances[k].MSE + lambda*R;
            if (J < minJ){
                minJ = J;
                minIdx = k;
//                    printf("\nNewminJ %f", minJ);
            }
        }
    }
    return minIdx;
}
