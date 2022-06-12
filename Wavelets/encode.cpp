#include <chrono>
#include <iostream>
#include "EncoderWrapper.h"
#include <vector>
#include <cmath>


int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    string test_path = "./imagens_vq/teste/";
    vector <string> names = {"barb.pgm", "cameraman.pgm", "gold.pgm", "lena.easy.pgm", "pp1205.pgm"};
    vector<string> test_images;
    for(const auto& name : names){
        test_images.push_back(test_path+name);
    }
    printf("\n Iniciando codificacao");

    for (int i = 0; i < 5; ++i) {


        string encode_path = "./imagens_vq/enc/";

        int imgIdx = i;
        string encoded_filename = encode_path + names[imgIdx] + "_encoded_" + to_string(LAMBDA) + ".txt";
        EncoderWrapper::encode(test_images[imgIdx], encoded_filename);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTempo de Encode (s): " << float(duration.count())/pow(10,6) << endl;
        printf("\n\nFim imagem %i/5", i+1);
    }



    return 0;
}
