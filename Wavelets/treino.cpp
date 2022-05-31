#include <iostream>

#include "ImageReader.h"
extern "C"
{
    #include "subdefs2.h"
    #include "sub.h"
}


int main() {
    using namespace std;

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    unsigned dims[3];
    intMatrix image = ImageReader::read(training_images[2].c_str(), dims);
    int **Image_orig = ImageReader::imatrix2ipointer(image);
    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
    int **Image = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);

    sub(Image_orig, Image_out, Image, (int)dims[1], (int)dims[0]);

    fMatrix Image_outF;
    fMatrix ImageF;
    for (int i = 0; i < dims[0]; ++i) {
        vector<float> row;
        vector<float> row2;
        for (int j = 0; j < dims[1]; ++j) {
            row.push_back((float)Image_out[i][j]);
            row2.push_back((float)Image[i][j]);
        }
        Image_outF.push_back(row);
        ImageF.push_back(row2);
    }

    ImageReader::write("teste.pgm", dims, Image_outF);
    ImageReader::write("teste2.pgm", dims, ImageF);

    return 0;
}
