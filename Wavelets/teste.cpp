#include <iostream>

#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
#include <chrono>
extern "C"
{
#include "subdefs2.h"
#include "sub.h"
}


int main() {
    using namespace std::chrono;

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    unsigned dims[3];
    intMatrix image = ImageReader::read(training_images[8].c_str(), dims);
    int **Image_orig = ImageReader::imatrix2ipointer(image);
    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
    int **Image = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);

    sub(Image_orig, Image_out, Image, (int)dims[1], (int)dims[0]);

    vector<intMatrix> subbands = WaveletHelper::splitSubbands(Image, (int)dims[1], (int)dims[0], NSTAGES);
    vector<vector<string>> best;

    vector<unsigned> idxTable;
    unsigned codebooks = 0;
    vector<bool> skips;

    auto start = high_resolution_clock::now();
    printf("\n Iniciando testes");

    unsigned iIdx = 0;

    for (int i = 0; i < NBANDS; ++i) {
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(i) + ".txt");

        vector<fMatrix> block_list;
//        printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

        for(const unsigned *block_size : vector_list){
            fMatrix test_blocks = ImageReader::getBlocks(block_size, subbands[i]);
            block_list.push_back(test_blocks);
        }

        vector<unsigned> bc = VQ::best_codebook(subbands[i], block_list, codebook_list, dims, iIdx);
//        printf("%s", ("\n" + training_images[8]).c_str());
        fMatrix newImage  = VQ::replaceBlocks(block_list[bc[0]], codebook_list[bc[1]], vector_list[bc[0]], dims);
        best.push_back({to_string(bc[3]), to_string(bc[4]), to_string(bc[0]), to_string(bc[2])});
//        ImageReader::write(("./imagens_vq/rec/Cod" + to_string(iIdx) + "_" + to_string(minPSNR) + ".pgm").c_str(), dims, newImage);
        iIdx += 1;

    }
    ImageReader::save_csv("./desempenhos/BestCDBK_30.csv", best, false);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo de Teste (s): " << float(duration.count())/pow(10,6) << endl;

    return 0;
}
