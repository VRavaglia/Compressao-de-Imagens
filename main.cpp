// Agradecimentos ao meu amigo de CC que sabe como a merda do leitor de arquivos do C++ funciona

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

#define GRAPH 0
#define DEBUG 1

using namespace std;

bool cmp(pair<string, float> &a,
         pair<string, float> &b) {
    return a.second > b.second;
}


map<char, int> generateSymbols(const string &filename, float &totalCount, vector<char> &source) {
    ifstream fin(filename, ios_base::binary);

    char byte = 0;

    // Read file byte by byte

    while (fin.get(byte)) {
        source.push_back(byte);
    }

    fin.close();

    map<char, int> symbolCount;

    // Create a map that has keys: symbols and values: occurrences

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

map<char, float> calculateProbabilities(const map<char, int> &symbolCount, const float totalCount) {
    // Create a map that has keys: symbols and values: frequencies
    map<char, float> symbolProbabilities;

    for (const auto &x: symbolCount) {
        symbolProbabilities.insert(pair<char, float>(x.first, float(x.second) / float(totalCount)));
    }

    return symbolProbabilities;
}

vector<vector<pair<string, float>>> createHuffmanTable(const map<char, float> &symbolProbabilities) {
    vector<vector<pair<string, float>>> huffmanTree;
    vector<pair<string, float>> tempVec;

    // Copy key-value pair from Map
    // to vector of pairs
    for (auto &it: symbolProbabilities) {
        string s(1, it.first);
        tempVec.emplace_back(pair<string, float>(s, it.second));
    }

    // Sort using comparator function
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

map<char, string> createSymbolTable(vector<vector<pair<string, float>>> &huffmanTree) {
    map<char, string> symbolCodes;

    for (auto &s: huffmanTree[0]) {
        char charTest = s.first[0];
        string code;
        bool endCode = false;
        auto idxColumn = huffmanTree.size()-1;

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
#if DEBUG == 1
        cout << "Simbolo: " << int(charTest) << " | Codigo: " << code << "\n";
#endif
        symbolCodes.insert(pair<char, string>(charTest, code));
    }

    return symbolCodes;
}

float calculateEntropy(map<char, float> &symbolProbabilities) {
    float H = 0;

    for (auto &symbol: symbolProbabilities) {
        H -= symbol.second * log2(symbol.second);
    }

    return H;
}

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

void encoder(const string &filename, map<char, string> &symbolTable, vector<char> &source) {
    string encodedFilename = filename;
    encodedFilename.erase(encodedFilename.length() - 4);
    ofstream encoded_file(encodedFilename + "_encoded.txt", ios_base::binary);

    unsigned totalBytes = 0;
    string header;

    for (auto &symbol: symbolTable) {
        totalBytes += symbol.second.size();
        header += symbol.first + symbol.second + "|";
    }

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
#if DEBUG == 1
            cout << "Comprimindo: " << totalProgress << "%\n";
#endif
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
#if DEBUG == 1
    cout << "\nTamanho original: " << in.tellg() << "\n";
#endif
    ifstream out(encodedFilename + "_encoded.txt", std::ifstream::ate | std::ifstream::binary);
#if DEBUG == 1
    cout << "Tamanho codificado: " << out.tellg() << "\n";
    cout << "Compressao: " << float(in.tellg()) / float(out.tellg()) << "x\n";
#endif

}

map<char, string> sourceAnalysis(const string &filename, vector<char> &source) {
    float totalCount = 0;

    map<char, int> symbolCount = generateSymbols(filename, totalCount, source);
    map<char, float> symbolProbabilities = calculateProbabilities(symbolCount, totalCount);
    vector<vector<pair<string, float>>> huffmanTree = createHuffmanTable(symbolProbabilities);
    map<char, string> symbolTable = createSymbolTable(huffmanTree);

    float H = calculateEntropy(symbolProbabilities);
    float bitsPerSymbol = calculateBits(symbolTable, symbolProbabilities);
#if DEBUG == 1
    cout << "\n\nEntropia da fonte: " << H << "\n";
    cout << "Bits/s apos Huffman: " << bitsPerSymbol << "\n";
    cout << "Redundancia: " << bitsPerSymbol - H << "\n\n";
#endif


    return symbolTable;
}

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
#if DEBUG == 1
                cout << "Simbolo com codigo incorreto no cabecalho!";
#endif
                return;
        }
        counter++;
    }

    currentNode->symbol = symbol;
    currentNode->isLeaf = true;
}

string recurse(string father, TreeNode &currentNode, int side) {
    string currentNodeName =
            "A" + to_string(currentNode.symbol) + "X" + to_string(currentNode.counter) + "X" + to_string(side);
    string tempString;
    if (currentNode.zero) {
        tempString += recurse(currentNodeName, *currentNode.zero, 0);
    }
    if (currentNode.one) {
        tempString += recurse(currentNodeName, *currentNode.one, 1);
    }
    return father + " -> " + currentNodeName + ";\n" + tempString;
}

string generateGraphviz(TreeNode &root) {
    string out = "digraph G {\n";

    out += recurse("nome", *root.zero, 0);
    out += recurse("nome", *root.one, 1);

    out += "}";

    return out;
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
#if GRAPH == 1
                    cout << generateGraphviz(root) << "\n\n\n";
#endif
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


int main() {
    auto start = chrono::high_resolution_clock::now();
    vector<char> source;
    map<char, string> symbolTable = sourceAnalysis("./../biblia.txt", source);
    encoder("./../biblia.txt", symbolTable, source);
    decoder("./../biblia_encoded.txt");
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "\nTempo de execucao: " << duration.count() << " ms\n";

    return 0;
}

