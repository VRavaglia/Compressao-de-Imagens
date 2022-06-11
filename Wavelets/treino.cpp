

#include <iostream>

#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
//#include <chrono>
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

    vector<vector<intMatrix>> all_subbands;

    for (const auto& img_name : training_images) {
        unsigned dims[3];
        intMatrix image = ImageReader::read(img_name.c_str(), dims);
        int **Image_orig = ImageReader::imatrix2ipointer(image);
        double *pSIMG[YLUM];
        ImageReader::remove_avg(Image_orig, dims);
        only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
        vector<intMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);
        all_subbands.push_back(subbands);
    }

//    using namespace std::chrono;
//    auto start = high_resolution_clock::now();

//    ImageReader::save_csv("aaaa.csv", subbands[0]);
    printf("\n Iniciando treinamento: %i/%i - %i/%i", 0, vl_size*cb_size_size, 0, NBANDS);
    for (int i = 0; i < NBANDS; ++i) {
        vector<fMatrix> codebook_list;
        vector<unsigned> idxTable;

        unsigned codebooks = 0;
        unsigned bSizeIdx = 0;
        vector<bool> skips;
        intMatrix codebook_dim_list;
        float eps = 0.1;


        for(const unsigned *block_size : vector_list){
            fMatrix blocks;
//            for (int j = 0; j < training_images.size(); j++) {
            for (int j = 0; j < 1; j++) {
                for (const auto &subbands : all_subbands) {
                    fMatrix temp_blocks = ImageReader::getBlocks(block_size, subbands[i]);
                    for(const auto& block : temp_blocks){
                        blocks.push_back(block);
                    }
                }
            }
//            if (i == 0 && bSizeIdx == 0){
//                ImageReader::save_csv("aaaa.csv", ImageReader::float2int(blocks));
//            }
            for (auto cbSize : cb_size_list) {
                unsigned bSize = block_size[0] * block_size[1];
                double R = log2(cbSize)/bSize;
                if((R <= MAXR)){
                    skips.push_back(false);
                    fMatrix codebook = VQ::LGB(blocks, cbSize, eps);
//                    if (i == 0 && bSizeIdx == 0 && cbSize == 16){
//                        ImageReader::save_csv("bbbb.csv", ImageReader::float2int(codebook));
//                    }
                    codebook_list.push_back(codebook);
                    codebook_dim_list.push_back({(int)bSize, (int)cbSize + 1});
                }
                else{
                    printf("\nTamanho de codebook ignorado.");
                    skips.push_back(true);
                }
                codebooks += 1;
                printf("\n[%i/%i] Treinamento: %i/%i - %i/%i", bSizeIdx,cbSize, codebooks, vl_size*cb_size_size, i+1, NBANDS);
            }
            bSizeIdx += 1;
        }

        VQ::save_codebooks("./codebooks/codebooks_" + to_string(i) + ".txt", codebook_list, codebook_dim_list);

    }

//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<microseconds>(stop - start);
//    cout << "\nTempo de Treino (s): " << float(duration.count())/pow(10,6) << endl;


//    fMatrix Image_outF;
//    fMatrix ImageF;
//    for (int i = 0; i < dims[0]; ++i) {
//        vector<float> row;
//        vector<float> row2;
//        for (int j = 0; j < dims[1]; ++j) {
//            row.push_back((float)Image_out[i][j]);
//            row2.push_back((float)Image[i][j]);
//        }
//        Image_outF.push_back(row);
//        ImageF.push_back(row2);
//    }
//
//    ImageReader::write("teste.pgm", dims, Image_outF);
//    ImageReader::write("teste2.pgm", dims, ImageF);

    return 0;
}
