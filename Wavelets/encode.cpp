#include <chrono>
#include <iostream>
#include "EncoderWrapper.h"
#include <vector>
#include <cmath>


int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    string test_path = "./imagens_vq/teste/";
//    vector <string> names = {"barb.pgm", "cameraman.pgm", "gold.pgm", "lena.easy.pgm", "pp1205.pgm"};
    vector <string> names = {"cameraman.pgm"};
    vector<string> test_images;
    for(const auto& name : names){
        test_images.push_back(test_path+name);
    }
    printf("\n Iniciando codificacao");

    int R_method = 2;

    int cnt = 0;
    for (float lb : lambdas) {
//        for (int i = 0; i < 1; ++i) {
    for (int i = 0; i < test_images.size(); ++i) {
            string encode_path = "./imagens_vq/enc/";

            int imgIdx = i;
            string encoded_filename = encode_path + names[imgIdx] + "_encoded_" + to_string(lb) + ".txt";
            EncoderWrapper::encode(test_images[imgIdx], encoded_filename, lb, R_method);

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "\nTempo de Encode (s): " << float(duration.count())/pow(10,6) << endl;
            cnt += 1;
            printf("\n\nFim imagem %i/%i", cnt, names.size()*7);
        }
    }








    return 0;
}
