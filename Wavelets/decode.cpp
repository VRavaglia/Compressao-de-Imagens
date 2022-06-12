#include <chrono>
#include <iostream>
#include "EncoderWrapper.h"
#include <vector>
#include <cmath>


int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    printf("\n Iniciando decodificacao");


    string test_path = "./imagens_vq/teste/";
    vector <string> names = {"barb.pgm", "cameraman.pgm", "gold.pgm", "lena.easy.pgm", "pp1205.pgm"};
    vector<string> test_images;
    for(const auto& name : names){
        test_images.push_back(test_path+name);
    }

    for (int i = 0; i < 5; ++i) {
        string encode_path = "./imagens_vq/enc/";
        string decode_path = "./imagens_vq/dec/";

        int imgIdx = i;
        string encoded_filename = encode_path + names[imgIdx] + "_encoded_" + to_string(LAMBDA) + ".txt";
        string decoded_filename = decode_path + names[imgIdx] + "_decoded_" + to_string(LAMBDA) + ".pgm";
        EncoderWrapper::decode(encoded_filename, decoded_filename);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTempo de Decode (s): " << float(duration.count())/pow(10,6) << endl;
        printf("\n\nFim imagem %i/5", i+1);
    }



    return 0;
}
