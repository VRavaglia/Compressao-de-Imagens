#include <chrono>
#include <iostream>
#include "EncoderWrapper.h"
#include <vector>
#include <cmath>


int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    printf("\n Iniciando codificacao");

    string training_path = "./imagens_vq/treino/";
    vector <string> names = {"aerial.pgm", "boats.pgm", "bridge.pgm", "D108.pgm", "f16.pgm", "lena.256.pgm", "peppers.pgm", "pp1209.pgm", "zelda.pgm"};
    vector<string> training_images;
    for(const auto& name : names){
        training_images.push_back(training_path+name);
    }

    string encode_path = "./imagens_vq/enc/";

    int imgIdx = 8;
    string encoded_filename = encode_path + names[imgIdx] + "_encoded.txt";
    EncoderWrapper::encode(training_images[imgIdx], encoded_filename);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "\nTempo de Encode (s): " << float(duration.count())/pow(10,6) << endl;

    return 0;
}
