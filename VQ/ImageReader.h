//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_IMAGEREADER_H
#define VQ_IMAGEREADER_H

#include <vector>
#include<tuple>

using namespace std;

using imageType = vector<vector<int>>;

class ImageReader {
private:
    static int vect2int(const vector<int> &in);
public:
    int H;
    int W;
    int G;
    imageType read(const char *filename);
    static vector<vector<int>> getBlocks(const unsigned size[2], imageType &image);
};


#endif //VQ_IMAGEREADER_H
