//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_IMAGEREADER_H
#define VQ_IMAGEREADER_H

#include <vector>
#include<tuple>

using namespace std;

class ImageReader {
private:
    static int vect2int(const vector<int> &in);
public:
    int H;
    int W;
    int G;
    vector<vector<int>> read(const char *filename);
};


#endif //VQ_IMAGEREADER_H
