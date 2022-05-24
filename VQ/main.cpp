#include <iostream>
#include "VQ.h"
#include "ImageReader.h"
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
using namespace std::chrono;


int main() {
    using namespace std;

    string training_path = "./imagens_vq/treino";
    vector<string> training_images;
    for (const auto & entry : filesystem::directory_iterator(training_path)){
        training_images.push_back(entry.path().string());
        break;
    }

    string test_path = "./imagens_vq/teste";
    vector<string> test_images;
    for (const auto & entry : filesystem::directory_iterator(test_path)){
        test_images.push_back(entry.path().string());
    }

    vector<fMatrix> codebook_list;
    vector<unsigned> idxTable;

    unsigned codebooks = 0;
    unsigned bSizeIdx = 0;
    vector<bool> skips;
    float eps = 0.1;

    auto start = high_resolution_clock::now();
    printf("\n Iniciando treinamento: %i/%i", codebooks, vl_size*cb_size_size);
    for(const unsigned *block_size : vector_list){
        fMatrix blocks;
        for (const auto& t_image : training_images) {
            unsigned temp_dims[3];
            intMatrix temp_image = ImageReader::read(t_image.c_str(), temp_dims);
            fMatrix temp_blocks = ImageReader::getBlocks(block_size, temp_image);
            for(const auto& block : temp_blocks){
                blocks.push_back(block);
            }
        }
        for (auto cbSize : cb_size_list) {
//            printf("\n%i", cbSize);
            unsigned bSize = block_size[0] * block_size[1];
            double R = log2(cbSize)/bSize;
//            if((cbSize <= 128 || (bSizeIdx > 0 && cbSize > 256))){
            if((R > 2 && R <= 7)){
                skips.push_back(false);
                fMatrix codebook = VQ::LGB(blocks, cbSize, eps);
                codebook_list.push_back(codebook);
            }
            else{
                printf("\nTamanho de codebook ignorado.");
                skips.push_back(true);
            }
            codebooks += 1;
            printf("\n[%i/%i] Treinamento: %i/%i", bSizeIdx,cbSize, codebooks, vl_size*cb_size_size);
        }
        bSizeIdx += 1;
    }

//    ImageReader::save_csv("./teste.csv", blocks);

    unsigned iIdx = 0;

    fMatrix best;

    for(const auto& test_file : test_images){
        vector<fMatrix> block_list;
        unsigned dims[3];
        intMatrix test_image = ImageReader::read(test_file.c_str(), dims);
        printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

        for(const unsigned *block_size : vector_list){
            fMatrix test_blocks = ImageReader::getBlocks(block_size, test_image);
            block_list.push_back(test_blocks);
        }

        vector<unsigned> bc = VQ::best_codebook(test_image, block_list, codebook_list, dims, iIdx, skips);
        printf(("\n" + test_file).c_str());

        fMatrix newImage  = VQ::replaceBlocks(block_list[bc[0]], codebook_list[bc[1]], vector_list[bc[0]], dims);
//        ImageReader::save_csv("./teste.csv", block_list[1], false);


        best.push_back({(float)bc[0], (float)bc[2]});

        ImageReader::write(("./imagens_vq/rec/Cod" + to_string(iIdx) + ".pgm").c_str(), dims, newImage);

        iIdx += 1;
    }
    ImageReader::save_csv("./BestCDBK.csv", best, false);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo (s): " << float(duration.count())/pow(10,6) << endl;


//    double mse = VQ::MSE(image, newImage);
//    double psnr = VQ::PSNR(image, newImage);

//    printf("\n\n MSE: %f, PSNR: %f", mse, psnr);




//    ImageReader::save_csv("./testeR.csv", fMatrix (newImage.begin(),newImage.end()));

    return 0;
}
