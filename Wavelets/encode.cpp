#include <iostream>

#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
#include <chrono>
extern "C"
{
#include "subdefs2.h"
#include "sub.h"
}


int main() {
    using namespace std::chrono;

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    int imgIdx = 8;
    char *imgPath = const_cast<char *>(training_images[imgIdx].c_str());

    unsigned dims[3];
    intMatrix image = ImageReader::read(imgPath, dims);
    int **Image_orig = ImageReader::imatrix2ipointer(image);
    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
    int **Image = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);

    sub(Image_orig, Image_out, Image, (int)dims[1], (int)dims[0]);

    vector<intMatrix> subbands = WaveletHelper::splitSubbands(Image, (int)dims[1], (int)dims[0], NSTAGES);
    vector<vector<string>> best;

    vector<unsigned> idxTable;
    unsigned codebooks = 0;
    vector<bool> skips;

    auto start = high_resolution_clock::now();
    printf("\n Iniciando testes");

    VQ::evaluate_codebooks(subbands, image, dims);

//    ImageReader::save_csv("./desempenhos/BestCDBK_30.csv", best, false);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo de Teste (s): " << float(duration.count())/pow(10,6) << endl;

    return 0;
}
