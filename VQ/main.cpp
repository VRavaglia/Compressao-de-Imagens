#include <iostream>
#include "VQ.h"
#include "ImageReader.h"

int main() {
    using namespace std;

//    imageType image = IR.read("./imagens_vq/treino/lena.256.pgm");
    int dims[3];
    intMatrix image = ImageReader::read("./teste.pgm", dims);
    fMatrix blocks = ImageReader::getBlocks(vector_list[6], image);

    printf("\nLinhas X Colunas X Max: %i x %i x %i", dims[0], dims[1], dims[2]);

    return 0;
}
