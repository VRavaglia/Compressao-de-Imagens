#include <chrono>
#include <iostream>
#include "EncoderWrapper.h"
#include "WaveletHelper.h"
#include <vector>
#include <cmath>
extern "C"
{
#include "subdefs2.h"
#include "sub.h"
}

int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
//    string test_path = "./imagens_vq/teste/";
//    vector <string> names = {"barb.pgm", "cameraman.pgm", "gold.pgm", "lena.easy.pgm", "pp1205.pgm"};
//    vector<string> test_images;
//    for(const auto& name : names){
//        test_images.push_back(test_path+name);
//    }
    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
//    vector <string> names = {"aerial.pgm", "boats.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }
    printf("\n Iniciando avaliacao");
    vector<vector<vector<performance>>> all_performances;
    vector<vector<vector<vector<int>>>> all_bb_idx;

//    for (int i = 0; i < 1; ++i) {
    for (int i = 0; i < names.size(); ++i) {

        char * imgPath = const_cast<char *>(training_images[i].c_str());
        unsigned dims[3];
        intMatrix image = ImageReader::read(imgPath, dims);
        int **Image_orig = ImageReader::imatrix2ipointer(image);

        double *pSIMG[YLUM];
//        ImageReader::remove_avg(Image_orig, dims);
        only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
        double media = 0;
        for (int k = 0; k < (int)dims[0]/8; ++k) {
            for (int j = 0; j < (int)dims[1]/8; ++j) {
                media += pSIMG[k][j]/((float)dims[0]/8*(float)dims[1]/8);
            }
        }
        for (int k = 0; k < (int)dims[0]/8; ++k) {
            for (int j = 0; j < (int)dims[1]/8; ++j) {
                pSIMG[k][j] -= round(media);
            }
        }
//        fMatrix FImage_out = ImageReader::dpointer2fmatrix(pSIMG, dims);
//        ImageReader::write("teste.pgm", dims, FImage_out);
//        exit(0);


//    fMatrix decomp = ImageReader::ipointer2fmatrix(Image, dims);

        vector<fMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);

        vector<vector<vector<int>>> bb_idx;
        vector<vector<performance>> performances = VQ::evaluate_codebooks(subbands, bb_idx);
        all_performances.push_back(performances);
        all_bb_idx.push_back(bb_idx);


        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTempo de Avaliacao (s): " << float(duration.count())/pow(10,6) << endl;
        printf("\n\nFim imagem %i/9", i+1);
    }

    VQ::save_performances(all_performances, names);
    VQ::save_histograms(all_bb_idx, all_performances);

    return 0;
}
