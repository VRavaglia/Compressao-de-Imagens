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
    float eps = 0.1;

    auto start = high_resolution_clock::now();
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

            fMatrix codebook = VQ::LGB(blocks, cbSize, eps);
            codebook_list.push_back(codebook);
            codebooks += 1;
            printf("\n Treinamento: %i/%i", codebooks, vl_size*cb_size_size);
        }
    }

//    ImageReader::save_csv("./teste.csv", blocks);

    unsigned iIdx = 0;


    for(const auto& test_file : test_images){
        vector<fMatrix> block_list;
        unsigned dims[3];
        intMatrix test_image = ImageReader::read(test_file.c_str(), dims);
        printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

        for(const unsigned *block_size : vector_list){
            fMatrix test_blocks = ImageReader::getBlocks(block_size, test_image);
            block_list.push_back(test_blocks);
        }

        vector<unsigned> bc = VQ::best_codebook(test_image, block_list, codebook_list, dims, iIdx);
        printf(("\n" + test_file).c_str());

        fMatrix newImage  = VQ::replaceBlocks(block_list[bc[0]], codebook_list[bc[1]], vector_list[bc[0]], dims);
//        ImageReader::save_csv("./teste.csv", block_list[1], false);
//        ImageReader::save_csv("./testeC.csv", codebook_list[6], false);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTempo (s): " << float(duration.count())/pow(10,6) << endl;

        ImageReader::write(("./imagens_vq/rec/Cod" + to_string(iIdx) + ".pgm").c_str(), dims, newImage);

        iIdx += 1;
    }



//    double mse = VQ::MSE(image, newImage);
//    double psnr = VQ::PSNR(image, newImage);

//    printf("\n\n MSE: %f, PSNR: %f", mse, psnr);




//    ImageReader::save_csv("./testeR.csv", fMatrix (newImage.begin(),newImage.end()));

    return 0;
}
