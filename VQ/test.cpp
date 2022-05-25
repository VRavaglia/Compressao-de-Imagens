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

    string test_path = "./imagens_vq/teste/";
    vector <string> names2 = {"barb.pgm", "cameraman.pgm", "gold.pgm", "lena.easy.pgm", "pp1205.pgm"};
    vector<string> test_images;
    for(const auto& name : names2){
        test_images.push_back(test_path+name);
    }

    vector<fMatrix> codebook_list = VQ::load_codebooks("codebooks.txt");

    vector<unsigned> idxTable;
    unsigned codebooks = 0;
    vector<bool> skips;

    auto start = high_resolution_clock::now();
    printf("\n Iniciando testes");

//    ImageReader::save_csv("./teste.csv", blocks);

    unsigned iIdx = 0;

    vector<vector<string>> best;

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
        printf("%s", ("\n" + test_file).c_str());
//        printf("\n\n %i %i", bc[0], bc[1]);

        fMatrix newImage  = VQ::replaceBlocks(block_list[bc[0]], codebook_list[bc[1]], vector_list[bc[0]], dims);


        best.push_back({to_string(bc[3]), to_string(bc[4]), to_string(bc[0]), to_string(bc[2])});

        ImageReader::write(("./imagens_vq/rec/Cod" + to_string(iIdx) + ".pgm").c_str(), dims, newImage);

        iIdx += 1;
    }
    ImageReader::save_csv("./desempenhos/BestCDBK.csv", best, false);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo de Teste (s): " << float(duration.count())/pow(10,6) << endl;


    return 0;
}
