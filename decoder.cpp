
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <memory>
#include <algorithm>
#include <iterator>
#include <chrono>

using namespace std;

// No da arvore de decodificacao
class TreeNode {
public:
    unique_ptr<TreeNode> zero = nullptr;
    unique_ptr<TreeNode> one = nullptr;
    char symbol = '\0';
    bool isLeaf = false;
    int counter = 0;
};

void insertSymbol(char symbol, const string &code, TreeNode &root, int &counter) {
    TreeNode *currentNode = &root;
    int bitIdx = 0;

    for (char bit: code) {
        bitIdx++;
        switch (bit) {
            case '0':
                if (currentNode->zero == nullptr) {
                    currentNode->zero = make_unique<TreeNode>();
                    currentNode->counter = counter;
                }
                currentNode = currentNode->zero.get();
                break;
            case '1':
                if (currentNode->one == nullptr) {
                    currentNode->one = make_unique<TreeNode>();
                    currentNode->counter = counter;
                }
                currentNode = currentNode->one.get();
                break;
            default:
                return;
        }
        counter++;
    }

    currentNode->symbol = symbol;
    currentNode->isLeaf = true;
}



void decoder(const string &filename) {
    FILE *fin = fopen(filename.c_str(), "rb");
    char byte;
    vector<char> header;
    string tempSize;
    int messageSize = 0;
    int bytesRead = 0;
    int counter = 0;

    vector<bool> encodedStream;
    string test;

    string tempCode;
    char tempSymbol;

    TreeNode root;

    string debugBits;


    TreeNode *currentNode = &root;
    string decodedFile;

    enum DecoderStates {
        ReadingSize, ReadingChar, ReadingCode, Decoding
    };

    DecoderStates state = ReadingSize;
    bool eof = false;

    while (fread(&byte, 1, 1, fin) == 1 && !eof) {
        test += byte;
        switch (state) {
            case ReadingSize:
                if ('|' != byte) {
                    tempSize += byte;
                } else {
                    messageSize = stoi(tempSize);
                    state = ReadingChar;
                }
                break;
            case ReadingChar:
                tempSymbol = byte;
                state = ReadingCode;
                break;
            case ReadingCode:
                if ('|' != byte) {
                    tempCode += byte;
                    bytesRead++;
                } else {
                    insertSymbol(tempSymbol, tempCode, root, counter);
                    counter++;
                    state = ReadingChar;
                    tempCode = "";

                }
                if (bytesRead == messageSize) {
                    insertSymbol(tempSymbol, tempCode, root, counter);

                    fseek(fin,1,SEEK_CUR);

                    state = Decoding;

                }
                break;
            case Decoding:
                unsigned char mask = 0b10000000;
                for (int i = 0; i < 8; ++i, mask >>= 1) {
                    if (byte & mask) {
                        currentNode = currentNode->one.get();
                        debugBits += "1";
                    } else {
                        currentNode = currentNode->zero.get();
                        debugBits += "0";
                    }
                    if (currentNode->isLeaf) {
                        debugBits = "";
                        if (currentNode->symbol == char_traits<char>::eof()){
                            eof = true;
                            break;
                        }else{
                            decodedFile += currentNode->symbol;
                            currentNode = &root;
                        }
                    }
                }
                break;
        }

    }



    fclose(fin);

    string decodedFilename = filename;
    decodedFilename.erase(decodedFilename.length() - 4);
    decodedFilename += "_decoded.txt";

    FILE * fout = fopen(decodedFilename.c_str(), "wb");

    fwrite(decodedFile.data(), 1, decodedFile.size(), fout);
    fclose(fout);
}


int main(int argc, char** argv) {

    // Instrucoes para o uso do programa
    if (argc <= 1){
        cout << "\nDecodifica um arquivo e gera o arquivo nomeArquivo_encoded_decoded.txt:\n\ndecoder.out [-t] nomeArquivo\n\n[-t] Exibe o tempo de execucao\nnomeArquivo: Arquivo que sera decodificado\n";
        return 0;
    }
    bool t = false;

    // Checagem de argumentos de execucao
    for (int i = 0; i < argc; ++i) {
        if(string("-t") == string(argv[i])){
            t = true;
        }
    }

    auto filename = argv[argc-1];
    if (filename == nullptr){
        cout << "Nome do arquivo vazio\n";
        return -1;
    }

    // Decodificacao de fato
    auto start = chrono::high_resolution_clock::now();
    decoder(filename);

    // Caso desejado, calcula o tempo de execucao
    if (t) {
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        cout << "\nTempo de execucao: " << duration.count() << " ms\n";
    }

    return 0;
}

