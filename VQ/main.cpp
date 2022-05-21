#include <iostream>
#include "VQ.h"
#include "ImageReader.h"
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
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

    unsigned iIdx = 2;
    unsigned dims[3];
//    intMatrix image = ImageReader::read("./teste.pgm", dims);
    intMatrix test_image = ImageReader::read(test_images[iIdx].c_str(), dims);
    printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

    vector<fMatrix> block_list;
    vector<fMatrix> codebook_list;
    vector<unsigned> idxTable;
    unsigned vector_idx = 0;

    unsigned codebooks = 0;
    float eps = 0.1;

    auto start = high_resolution_clock::now();

    for(const unsigned *block_size : vector_list){
        for (auto cbSize : cb_size_list) {
            fMatrix blocks;
            for (const auto& t_image : training_images) {
                unsigned temp_dims[3];
                intMatrix temp_image = ImageReader::read(t_image.c_str(), temp_dims);
                fMatrix temp_blocks = ImageReader::getBlocks(block_size, temp_image);
                for(const auto& block : temp_blocks){
                    blocks.push_back(block);
                }
            }
            fMatrix test_blocks = ImageReader::getBlocks(block_size, test_image);
            block_list.push_back(test_blocks);
            fMatrix codebook = VQ::LGB(blocks, cbSize, eps);
            codebook_list.push_back(codebook);
            idxTable.push_back(vector_idx);
            codebooks += 1;
            printf("\n Codebook calculado %i/%i", codebooks, 7*4);
        }
        vector_idx += 1;

    }

//    ImageReader::save_csv("./teste.csv", blocks);

    unsigned bc = VQ::best_codebook(test_image, block_list, codebook_list, dims);

    printf("\nMelhor codebook: %i", bc);

    fMatrix newImage  = VQ::replaceBlocks(block_list[bc], codebook_list[bc], vector_list[idxTable[bc]], dims);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Tempo: " << duration.count() << endl;

//    double mse = VQ::MSE(image, newImage);
//    double psnr = VQ::PSNR(image, newImage);

//    printf("\n\n MSE: %f, PSNR: %f", mse, psnr);

    ImageReader::write(("./imagens_vq/rec/Cod" + to_string(iIdx) + ".pgm").c_str(), dims, newImage);

//    ImageReader::save_csv("./testeC.csv", codebook);
//    ImageReader::save_csv("./testeR.csv", fMatrix (newImage.begin(),newImage.end()));

    return 0;
}
