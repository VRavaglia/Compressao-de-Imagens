//
// Created by Victor on 16/05/2022.
//

#ifndef VQ_IMAGEREADER_H
#define VQ_IMAGEREADER_H

#include <vector>
#include <string>

using namespace std;

using intMatrix = vector<vector<int>>;
using fMatrix = vector<vector<float>>;

class ImageReader {
private:
    static int vect2int(const vector<int> &in);
public:
    static int** imatrix2ipointer(const intMatrix& input);
    static intMatrix ipointer2imatrix(const int **input, int heigth, int width);
    static int** allocIntMatrix(int rows, int cols);
    static intMatrix read(const char *filename, unsigned *dims);
    static fMatrix getBlocks(const unsigned size[2], const intMatrix &image);
    static void save_csv(const char *filename, const vector<vector<string>> &blocks, bool convert);
    static void write(const char *filename, unsigned *dims, const fMatrix &image);
};


#endif //VQ_IMAGEREADER_H
