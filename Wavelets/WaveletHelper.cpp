//
// Created by Victor on 31/05/2022.
//

#include <cmath>
#include "WaveletHelper.h"
#include "VQ.h"
#include "subdefs2.h"

vector<intMatrix> WaveletHelper::splitSubbands(double **InputImg, int ximg, int yimg, int nsubs){
    vector<intMatrix> subbands;
    intMatrix lll;

    for (int j = 0; j < yimg/(pow(2, nsubs)); ++j) {
        vector<int> row;
        for (int i = 0; i < ximg/(pow(2, nsubs)); ++i) {
            row.push_back((int)round(InputImg[j][i]));
        }
        lll.push_back(row);
    }
    subbands.push_back(lll);

    for (int i = 1; i < nsubs+1; ++i) {
        for (int j = 0; j < 3; ++j) {
            intMatrix temp;
            int mWidth = (int)round(ximg/(pow(2, i)));
            int mHeigth = (int)round(yimg/(pow(2, i)));
            int startPosMult[3][2] = {{1, 0}, {1,1}, {0, 1}};
            int startX = startPosMult[j][0]*mWidth;
            int startY = startPosMult[j][1]*mHeigth;
            for (int l = startY; l < startY+mHeigth; ++l) {
                vector<int> col;
                for (int k = startX; k < startX + mWidth; ++k) {
                    col.push_back((int)round(InputImg[l][k]));
                }
                temp.push_back(col);
            }
            printf("\n SBSize: %i", mWidth*mHeigth);
            subbands.push_back(temp);
        }
    }
    
    return subbands;
}

intMatrix WaveletHelper::quantize(const vector<intMatrix> &oldSubbands, const vector<vector<performance>> &performances, float lambda, unsigned bestCodebooks[NBANDS]){
    intMatrix newBlocks;
    for (int i = 0; i < NBANDS; ++i) {
        double minJ = pow(10, 4);
        unsigned minIdx = 0;
        for (int k  = 0; k < performances[i].size(); k++) {
            double J = performances[i][k].MSE + lambda*performances[i][k].R;
            if (J < minJ){
                minJ = J;
                minIdx = k;
            }
        }

        newBlocks.push_back(performances[i][minIdx].blockList);
        bestCodebooks[i] = performances[i][minIdx].codebook_idx;

        printf("\n MinJ: %f - MSE: %f R: %f - cbSize: %i, bsize: %i", minJ, performances[i][minIdx].MSE, performances[i][minIdx].R, performances[i][minIdx].codebook_size, performances[i][minIdx].block_size[0]*performances[i][minIdx].block_size[1]);
//        for (int j = 0; j < performances[i][minIdx].blockList.size(); ++j) {
//            printf(" %i", performances[i][minIdx].blockList[j]);
//        }
    }

    return newBlocks;
}
