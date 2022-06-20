

#include <iostream>

#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
#include "cb_list.h"
//#include <chrono>
extern "C"
{
    #include "subdefs2.h"
    #include "sub.h"
}


int main(int argc, char **argv) {
    using namespace std;

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    vector<vector<fMatrix>> all_subbands;

    for (const auto& img_name : training_images) {
        unsigned dims[3];
        intMatrix image = ImageReader::read(img_name.c_str(), dims);
        int **Image_orig = ImageReader::imatrix2ipointer(image);
        double *pSIMG[YLUM];
        ImageReader::remove_avg(Image_orig, dims);
        only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);
//        double media = 0;
//        for (int i = 0; i < (int)dims[0]/8; ++i) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                media += pSIMG[i][j]/((float)dims[0]/8*(float)dims[1]/8);
//            }
//        }
//        for (int i = 0; i < (int)dims[0]/8; ++i) {
//            for (int j = 0; j < (int)dims[1]/8; ++j) {
//                pSIMG[i][j] -= round(media);
//            }
//        }
        vector<fMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);
        all_subbands.push_back(subbands);
    }

//    using namespace std::chrono;
//    auto start = high_resolution_clock::now();

//    ImageReader::save_csv("aaaa.csv", subbands[0]);
    vector<int> bands;

    if (argc != 2){
        for (int i = 0; i < NBANDS; ++i) {
            bands.push_back(i);
        }
    }else{
        bands.push_back(stoi(argv[1]));
    }

    for (int i : bands) {
        printf("\n Iniciando treinamento: %i/%i - %i/%i", 0, bsize_list(i).size()*csize_list(i).size(), 0, NBANDS);
//    for(int i = banda; i < banda+1; i++){
        vector<fMatrix> codebook_list;
        vector<unsigned> idxTable;

        unsigned codebooks = 0;
        unsigned bSizeIdx = 0;
        vector<bool> skips;
        intMatrix codebook_dim_list;
        float eps = 1;
        float parada = 0.1;


        for(const auto& block_size : bsize_list(i)){
            fMatrix blocks;
            for (int j = 0; j < training_images.size(); j++) {
//            for (int j = 0; j < 1; j++) {

                    fMatrix temp_blocks = ImageReader::getBlocks(block_size, all_subbands[j][i]);
                    for(const auto& block : temp_blocks){
                        blocks.push_back(block);
                }
            }
//            if (i == 0 && bSizeIdx == 0){
//                ImageReader::save_csv("aaaa.csv", ImageReader::float2int(blocks));
//            }
            for (auto cbSize : csize_list(i)) {
                unsigned bSize = block_size[0] * block_size[1];
                double R = log2(cbSize)/bSize;
                if((R <= MAXR)){
                    skips.push_back(false);
                    bool finish = false;
//                    if (codebooks < 44) finish = true;
                    while(!finish){
                        fMatrix codebook;
                        int result = VQ::LGB(blocks, cbSize, eps, parada, codebook);
                        if(result == 0){
                            codebook_list.push_back(codebook);
                            codebook_dim_list.push_back({(int)bSize, (int)cbSize + 1});
                            finish = true;
                        }
                    }
                }
                else{
                    printf("\nTamanho de codebook ignorado.");
                    skips.push_back(true);
                }
                codebooks += 1;
                printf("\n[%i/%i] Treinamento: %i/%i - %i/%i", bSizeIdx,cbSize, codebooks, bsize_list(i).size()*csize_list(i).size(), i+1, bands.size());
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
