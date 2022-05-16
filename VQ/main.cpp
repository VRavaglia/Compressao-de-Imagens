#include <iostream>
#include "VQ.h"
#include "ImageReader.h"

int main() {
    using namespace std;

    ImageReader IR{};



    vector<vector<int>> image = IR.read("./imagens_vq/treino/lena.256.pgm");

    printf("\nLinhas X Colunas X Max: %i x %i x %i", IR.W, IR.H, IR.G);

    return 0;
}
