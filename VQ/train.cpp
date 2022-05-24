#include <iostream>
#include "VQ.h"
#include "ImageReader.h"
//#include <filesystem>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
using namespace std::chrono;


int main() {
    using namespace std;

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    vector<fMatrix> codebook_list;
    vector<unsigned> idxTable;

    unsigned codebooks = 0;
    unsigned bSizeIdx = 0;
    vector<bool> skips;
    intMatrix codebook_dim_list;
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
            unsigned bSize = block_size[0] * block_size[1];
            double R = log2(cbSize)/bSize;
            if((R <= 7)){
                skips.push_back(false);
                fMatrix codebook = VQ::LGB(blocks, cbSize, eps);
                codebook_list.push_back(codebook);
                codebook_dim_list.push_back({(int)bSize, (int)cbSize});
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

    VQ::save_codebooks("./codebooks.txt", codebook_list, codebook_dim_list);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo de Treino (s): " << float(duration.count())/pow(10,6) << endl;

    return 0;
}
