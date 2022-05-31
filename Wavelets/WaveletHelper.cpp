//
// Created by Victor on 31/05/2022.
//

#include <cmath>
#include "WaveletHelper.h"

vector<intMatrix> WaveletHelper::splitSubbands(int **InputImg, int ximg, int yimg, int nsubs){
    vector<intMatrix> subbands;
    intMatrix lll;

    for (int i = 0; i < ximg/(pow(2, nsubs)); ++i) {
        vector<int> row;
        for (int j = 0; j < yimg/(pow(2, nsubs)); ++j) {
            row.push_back(InputImg[i][j]);
        }
        lll.push_back(row);
    }
    subbands.push_back(lll);

    for (int i = 1; i < nsubs; ++i) {
        for (int j = 0; j < 3; ++j) {
            intMatrix temp;
            int xLims = (int)round(ximg/(pow(2, i)));
            int yLims = (int)round(yimg/(pow(2, i)));
            for (int k = (int)round(ximg/(pow(2, i)) + 1); k < ximg/(pow(2, i-1)); ++k) {
                vector<int> row;
                for (int l = 0; l < yimg/(pow(2, i)); ++l) {
                    row.push_back(InputImg[k][l]);
                }
                temp.push_back(row);
            }
            subbands.push_back(temp);
        }
    }
    
    return subbands;
}