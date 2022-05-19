#include <iostream>
#include "VQ.h"
#include "ImageReader.h"
#include <filesystem>
#include <string>
#include <vector>


int main() {
    using namespace std;

    string training_path = "./imagens_vq/treino";
    vector<string> training_images;
    for (const auto & entry : filesystem::directory_iterator(training_path)){
        training_images.push_back(entry.path().string());
    }

    string test_path = "./imagens_vq/teste";
    vector<string> test_images;
    for (const auto & entry : filesystem::directory_iterator(test_path)){
        test_images.push_back(entry.path().string());
    }

    int dims[3];
//    intMatrix image = ImageReader::read("./teste.pgm", dims);
    intMatrix image = ImageReader::read(training_images[0].c_str(), dims);
    fMatrix blocks = ImageReader::getBlocks(vector_list[1], image);

    ImageReader::save_csv("./teste.csv", blocks);

    printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

    fMatrix clusters = VQ::LGB(blocks, 32, 0.01);

    ImageReader::save_csv("./testeC.csv", clusters);

    return 0;
}
