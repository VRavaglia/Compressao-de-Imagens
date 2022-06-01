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

    for (int i = 1; i < nsubs+1; ++i) {
        for (int j = 0; j < 3; ++j) {
            intMatrix temp;
            int mWidth = (int)round(ximg/(pow(2, i)));
            int mHeigth = (int)round(yimg/(pow(2, i)));
            int startPosMult[3][2] = {{1, 0}, {1,1}, {0, 1}};
            int startX = startPosMult[j][0]*mWidth;
            int startY = startPosMult[j][1]*mHeigth;
//            printf("\nX: %i - %i", startX, startX+mWidth-1);
//            printf("\nY: %i - %i", startY, startY+mHeigth-1);
            for (int k = startX; k < startX + mWidth; ++k) {
                vector<int> row;
                for (int l = startY; l < startY+mHeigth; ++l) {
                    row.push_back(InputImg[l][k]);
                }
                temp.push_back(row);
            }
            subbands.push_back(temp);
        }
    }
    
    return subbands;
}