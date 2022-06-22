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
//    vector <string> names = {"boats.pgm", "aerial.pgm"};
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
        int **Image = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
        int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);


        ImageReader::remove_avg(Image_orig, dims);
        double *pSIMG[YLUM];
        only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
//        sub2(Image_orig, Image_out, Image, (int)dims[1], (int)dims[0]);


//        for (int y = 0; y < (int)dims[0]; y++) {
//            if ((pSIMG[y] = (double *) malloc((int)dims[1] * (sizeof(double)))) == NULL) {
//                printf("Memory allocation for luminance transform failed at line %d", y);
//                exit(1);
//            }
//        }
//        for (int f = 0; f < (int)dims[0]; ++f) {
//            for (int j = 0; j < (int)dims[1]; ++j) {
//                pSIMG[f][j] = (double)Image[f][j];
////                printf("\npSIMG: %f", pSIMG[i][j]);
//            }
//        }



//        double *pSIMG[YLUM];
//        ImageReader::remove_avg(Image_orig, dims);
//        sub2(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
//        only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
//        double media = 0;
//        for (int k = 0; k < (int)dims[0]/8; ++k) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                media += pSIMG[k][j]/((float)dims[0]/8*(float)dims[1]/8);
//            }
//        }
//        for (int k = 0; k < (int)dims[0]/8; ++k) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                pSIMG[k][j] -= round(media);
//            }
//        }
//        fMatrix FImage_out = ImageReader::dpointer2fmatrix(pSIMG, dims);
//        ImageReader::write("teste.pgm", dims, FImage_out);
//        exit(0);


//    fMatrix decomp = ImageReader::ipointer2fmatrix(Image, dims);

        vector<fMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);

        vector<vector<vector<int>>> bb_idx;
        vector<vector<int*>>temp;
        vector<vector<performance>> performances = VQ::evaluate_codebooks(subbands, bb_idx, false, all_bb_idx, temp);
        all_performances.push_back(performances);
        all_bb_idx.push_back(bb_idx);


        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTempo de Avaliacao (s): " << float(duration.count())/pow(10,6) << endl;
        printf("\n\nFim imagem %i/9", i+1);
    }
    VQ::save_performances(all_performances, names, false);
    VQ::save_histograms(all_bb_idx, all_performances);
    VQ::save_histograms2(all_bb_idx, all_performances);
    vector<vector<vector<performance>>> all_performances2;

    vector<vector<int*>> all_models;
    if(TWOPASSES) {
        for (int j = 0; j < NBANDS; ++j) {
            vector<int*> sband_models;
            int cidx = 0;
            for (const auto& block_size : bsize_list(j)) {
                for (int cb_size : csize_list(j)) {
                    sband_models.push_back(VQ::load_model2(j,cidx));
                    cidx += 1;
                }
            }
            all_models.push_back(sband_models);
        }
        for (int i = 0; i < names.size(); ++i) {

            char *imgPath = const_cast<char *>(training_images[i].c_str());
            unsigned dims[3];
            intMatrix image = ImageReader::read(imgPath, dims);
            int **Image_orig = ImageReader::imatrix2ipointer(image);

            double *pSIMG[YLUM];
            ImageReader::remove_avg(Image_orig, dims);
            only_anal(Image_orig, pSIMG, (int) dims[1], (int) dims[0]);
//        double media = 0;
//        for (int k = 0; k < (int)dims[0]/8; ++k) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                media += pSIMG[k][j]/((float)dims[0]/8*(float)dims[1]/8);
//            }
//        }
//        for (int k = 0; k < (int)dims[0]/8; ++k) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                pSIMG[k][j] -= round(media);
//            }
//        }
//        fMatrix FImage_out = ImageReader::dpointer2fmatrix(pSIMG, dims);
//        ImageReader::write("teste.pgm", dims, FImage_out);
//        exit(0);


//    fMatrix decomp = ImageReader::ipointer2fmatrix(Image, dims);

            vector<fMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int) dims[1], (int) dims[0], NSTAGES);

            vector<vector<vector<int>>> bb_idx;
            vector<vector<performance>> performances = VQ::evaluate_codebooks(subbands, bb_idx, true, all_bb_idx, all_models);
            all_performances2.push_back(performances);

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "\nTempo de Avaliacao (s): " << float(duration.count()) / pow(10, 6) << endl;
            printf("\n\nFim imagem %i/9", i + 1);
        }

        VQ::save_performances(all_performances2, names, true);
    }

    return 0;
}
