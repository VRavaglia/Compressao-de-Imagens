#include <iostream>
#include "VQ.h"
#include "ImageReader.h"

int main() {
    using namespace std;

    ImageReader IR{};

//    imageType image = IR.read("./imagens_vq/treino/lena.256.pgm");
    imageType image = IR.read("./teste.pgm");
    vector<vector<int>> blocks = ImageReader::getBlocks(vector_list[6], image);

    printf("\nLinhas X Colunas X Max: %i x %i x %i", IR.W, IR.H, IR.G);

    return 0;
}
