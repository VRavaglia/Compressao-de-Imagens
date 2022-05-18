//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_IMAGEREADER_H
#define VQ_IMAGEREADER_H

#include <vector>

using namespace std;

using intMatrix = vector<vector<int>>;
using fMatrix = vector<vector<float>>;

class ImageReader {
private:
    static int vect2int(const vector<int> &in);
public:
    static intMatrix read(const char *filename, int *dims);
    static fMatrix getBlocks(const unsigned size[2], intMatrix &image);
    static void save_csv(const char *filename, const intMatrix &image);
};


#endif //VQ_IMAGEREADER_H
