
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

// Comparacao para ordenar os vetores contendo simbolos e probabilidades
bool cmp(pair<string, float> &a,
         pair<string, float> &b) {
    return a.second > b.second;
}

// Le o arquivo original e gera um mapa de frequencias de cada simbolo contido no arquivo
map<char, int> generateSymbols(const string &filename, float &totalCount, vector<char> &source) {
    ifstream fin(filename, ios_base::binary);

    char byte = 0;

    // Ler o arquivo byte por byte

    while (fin.get(byte)) {
        source.push_back(byte);
    }

    fin.close();

    map<char, int> symbolCount;

    // Cria um mapa com chaves sendo os simbolos e as valores sendo a quantidade de ocorrencias

    for (char c: source) {
        totalCount += 1;
        if (symbolCount.count(c)) {
            symbolCount[c] += 1;
        } else {
            symbolCount.insert(pair<char, int>(c, 1));
        }
    }

    symbolCount.insert(pair<char, int>(char_traits<char>::eof(), 1));

    return symbolCount;
}


// Dadas as ocorrencias, gera um mapa de probabilidades para cada simbolo
map<char, float> calculateProbabilities(const map<char, int> &symbolCount, const float totalCount) {
    // Create a map that has keys: symbols and values: frequencies
    map<char, float> symbolProbabilities;

    for (const auto &x: symbolCount) {
        symbolProbabilities.insert(pair<char, float>(x.first, float(x.second) / float(totalCount)));
    }

    return symbolProbabilities;
}


//Cria a a tabela de Huffman, de modo que cada nova coluna contem os simbolos da anterior mas com os dois de maior
// probabilidade agrupados
vector<vector<pair<string, float>>> createHuffmanTable(const map<char, float> &symbolProbabilities) {
    vector<vector<pair<string, float>>> huffmanTree;
    vector<pair<string, float>> tempVec;

    for (auto &it: symbolProbabilities) {
        string s(1, it.first);
        tempVec.emplace_back(pair<string, float>(s, it.second));
    }

    // Sort based on probabilities
    sort(tempVec.begin(), tempVec.end(), cmp);

    huffmanTree.push_back(tempVec);

    for (int i = 0; i < (tempVec.size() - 2); ++i) {
        vector<pair<string, float>> newRow;
        float pSum = huffmanTree[i].rbegin()[0].second + huffmanTree[i].rbegin()[1].second;
        string charSum = huffmanTree[i].rbegin()[0].first + huffmanTree[i].rbegin()[1].first;

        newRow.reserve((huffmanTree[i].size() - 2));
        for (int n = 0; n < (huffmanTree[i].size() - 2); n++) {
            newRow.push_back(huffmanTree[i][n]);
        }
        newRow.emplace_back(charSum, pSum);

        sort(newRow.begin(), newRow.end(), cmp);

        huffmanTree.push_back(newRow);

    }

    return huffmanTree;
}

// Percorre a tabela de Huffman para gerar os codigos de cada simbolo
map<char, string> createSymbolTable(vector<vector<pair<string, float>>> &huffmanTree, bool verbose) {
    map<char, string> symbolCodes;

    for (auto &s: huffmanTree[0]) {
        char charTest = s.first[0];
        string code;
        bool endCode = false;
        auto idxColumn = huffmanTree.size() - 1;

        while (!endCode) {
            int idxSymbol = 1;
            for (auto &symbols: huffmanTree[idxColumn]) {
                string::size_type idx = symbols.first.find(charTest);

                if (idx != string::npos) {
                    if (symbols.first.size() == 1) {
                        endCode = true;
                    }
                    int codeToAppend = 1 - (huffmanTree[idxColumn].size() - idxSymbol);
                    if (codeToAppend >= 0) {
                        code += to_string(codeToAppend);
                    }

                }
                idxSymbol++;
            }
            idxColumn--;
        }
        if (verbose){
            cout << "Simbolo: " << int(charTest) << " | Codigo: " << code << "\n";
        }
        symbolCodes.insert(pair<char, string>(charTest, code));
    }

    return symbolCodes;
}

// Calcula a entropia do arquivo original
float calculateEntropy(map<char, float> &symbolProbabilities) {
    float H = 0;

    for (auto &symbol: symbolProbabilities) {
        H -= symbol.second * log2(symbol.second);
    }

    return H;
}

// Calcula a quantiade media de bits/simbolo depois da codificacao
float calculateBits(map<char, string> &symbolTable, map<char, float> &symbolProbabilities) {
    float Bits = 0;

    for (auto &symbol: symbolTable) {
        for (auto &symbol2: symbolProbabilities) {
            if (symbol.first == symbol2.first) {
                Bits += float(symbol.second.size()) * symbol2.second;
            }
        }
    }

    return Bits;
}

// Codificadorem si, cria o cabecalho do arquivo codificado e reescreve os simbolos do arquivo original com seus
// respectivos codigos binarios
void encoder(const string &filename, map<char, string> &symbolTable, vector<char> &source, bool verbose) {
    string encodedFilename = filename;
    encodedFilename.erase(encodedFilename.length() - 4);
    ofstream encoded_file(encodedFilename + "_encoded.txt", ios_base::binary);

    unsigned totalBytes = 0;
    string header;

    // Escreve os pares simbolo/codigo no cabecalho
    for (auto &symbol: symbolTable) {
        totalBytes += symbol.second.size();
        header += symbol.first + symbol.second + "|";
    }

    // Tambem adiciona a contagem de bytes no cabecalho
    encoded_file << to_string(totalBytes) + "|" + header;

    string bitstream;

    for (auto &symbol: source) {
        bitstream += symbolTable[symbol];
    }
    bitstream += symbolTable[char_traits<char>::eof()];

    int remainingBits = 0;
    int progress = 0;
    int totalProgress = 0;
    unsigned char buffer = 0;

    // Escreve o resto do arquivo codificado bit a bit utilizando um buffer de 8 bits
    // ao final, insere um caractere de EOF
    for (char c: bitstream) {
        buffer <<= 1;

        if (int(c - '0') == 1) buffer |= 1;

        remainingBits++;

        if (remainingBits == 8) {
            remainingBits = 0;
            encoded_file << buffer;
            buffer = 0;
        }
        progress++;
        if (progress >= bitstream.length() / 10) {
            progress = 0;
            totalProgress += 10;
            if (verbose) {
                cout << "Comprimindo: " << totalProgress << "%\n";
            }
        }
    }
    if (remainingBits != 0) {
        for (int i = 0; i < (8 - remainingBits); ++i) {
            buffer <<= 1;
        }
        encoded_file << buffer;
    }

    encoded_file.close();

    ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    if (verbose) {
        cout << "\nTamanho original: " << in.tellg() << "\n";
    }
    ifstream out(encodedFilename + "_encoded.txt", std::ifstream::ate | std::ifstream::binary);
    if (verbose) {
        cout << "Tamanho codificado: " << out.tellg() << "\n";
        cout << "Compressao: " << float(in.tellg()) / float(out.tellg()) << "x\n";
    }

}

// Analisa o arquivo original para montar a tabela de Huffman
map<char, string> sourceAnalysis(const string &filename, vector<char> &source, bool verbose) {
    float totalCount = 0;

    map<char, int> symbolCount = generateSymbols(filename, totalCount, source);
    map<char, float> symbolProbabilities = calculateProbabilities(symbolCount, totalCount);
    vector<vector<pair<string, float>>> huffmanTree = createHuffmanTable(symbolProbabilities);
    map<char, string> symbolTable = createSymbolTable(huffmanTree, verbose);

    float H = calculateEntropy(symbolProbabilities);
    float bitsPerSymbol = calculateBits(symbolTable, symbolProbabilities);
    if (verbose) {
        cout << "\n\nEntropia da fonte: " << H << "\n";
        cout << "Bits/s apos Huffman: " << bitsPerSymbol << "\n";
        cout << "Redundancia: " << bitsPerSymbol - H << "\n\n";
    }


    return symbolTable;
}



int main(int argc, char** argv) {

    // Instrucoes para o uso do programa
    if (argc <= 1){
        cout << "\nCodifica um arquivo e gera o arquivo nomeArquivo_encoded.txt:\n\nencoder.out [-v] [-t] nomeArquivo\n\n[-v] Exibe informacoes sobre a execucao na tela.\n[-t] Exibe o tempo de execucao\nnomeArquivo: Arquivo que sera codificado\n";
        return 0;
    }
    bool v = false;
    bool t = false;

    // Checagem de argumentos de execucao
    for (int i = 0; i < argc; ++i) {
        if(string("-v") == string(argv[i])){
            v = true;
        }
        if(string("-t") == string(argv[i])){
            t = true;
        }

    }

    auto filename = argv[argc-1];
    if (filename == nullptr){
        cout << "Nome do arquivo vazio\n";
        return -1;
    }

    // Codificacao de fato
    auto start = chrono::high_resolution_clock::now();
    vector<char> source;
    map<char, string> symbolTable = sourceAnalysis(filename, source, v);
    encoder(filename, symbolTable, source, v);

    // Caso desejado, calcula o tempo de execucao
    if (t) {
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        cout << "\nTempo de execucao: " << duration.count() << " ms\n";
    }

    return 0;
}

