#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <unistd.h>

using namespace std;

bool cmp(pair<string, float>& a,
         pair<string, float>& b)
{
    return a.second > b.second;
}


map<char, int> generateSymbols(const string& filename, float &totalCount){
    ifstream fin(filename);

    char byte = 0;
    vector<char> source;

    // Read file byte by byte

    while (fin.get(byte)) {
        source.push_back(byte);
    }

    fin.close();

    map<char, int> symbolCount;

    // Create a map that has keys: symbols and values: occurrences

    for (char c : source){
        totalCount += 1;
        if(symbolCount.count(c)){
            symbolCount[c] += 1;
        }
        else{
            symbolCount.insert(pair<char, int>(c, 1));
        }
    }

    return symbolCount;
}

map<char, float> calculateProbabilities(const map<char, int> &symbolCount, const float totalCount){
    // Create a map that has keys: symbols and values: frequencies
    map<char, float> symbolProbabilities;

    for (const auto& x : symbolCount) {
        symbolProbabilities.insert(pair<char, float>(x.first, float(x.second)/float(totalCount)));
    }

    return symbolProbabilities;
}

vector<vector<pair<string, float>>> createHuffmanTable(const map<char, float> &symbolProbabilities){
    vector<vector<pair<string, float>>> huffmanTree;
    vector<pair<string, float>> tempVec;

    // Copy key-value pair from Map
    // to vector of pairs
    for (auto& it : symbolProbabilities) {
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
        for (int n=0; n<(huffmanTree[i].size() - 2); n++){
            newRow.push_back(huffmanTree[i][n]);
        }
        newRow.emplace_back(charSum, pSum);

        sort(newRow.begin(), newRow.end(), cmp);

        huffmanTree.push_back(newRow);


    }

    return huffmanTree;
}

map<char, string> createSymbolTable(vector<vector<pair<string, float>>> &huffmanTree){
    map<char, string> symbolCodes;

    for (auto& s : huffmanTree[0])
    {
        char charTest = s.first[0];
        string code;
        bool endCode = false;
        vector<vector<pair<string, float>>>::size_type idxColumn = huffmanTree.size();

        while (!endCode)
        {
            int idxSymbol = 1;
            for (auto& symbols : huffmanTree[idxColumn])
            {
                string::size_type idx = symbols.first.find(charTest);

                if (idx != string::npos)
                {
//                    cout << symbols.first << " | " << idxSymbol << " | " << huffmanTree[idxColumn].size() <<"\n";
                    if(symbols.first.size() == 1)
                    {
                        endCode = true;
                    }
                    int codeToAppend = 1 - (huffmanTree[idxColumn].size() - idxSymbol);
                    if(codeToAppend >= 0){
                        code += to_string(codeToAppend);
                    }

                }
                idxSymbol++;
            }
            idxColumn--;
        }

        cout << "Simbolo: " << int(charTest) << " | Codigo: " << code << "\n";

        symbolCodes.insert(pair<char, string>(charTest, code));
    }

    return symbolCodes;
}

float calculateEntropy(map<char, float> &symbolProbabilities){
    float H = 0;

    for(auto& symbol : symbolProbabilities){
        H -= symbol.second*log2(symbol.second);
    }

    return H;
}

float calculateBits(map<char, string> &symbolTable, map<char, float> &symbolProbabilities){
    float Bits = 0;

    for(auto& symbol : symbolTable){
        for(auto& symbol2 : symbolProbabilities){
            if(symbol.first == symbol2.first){
                Bits += float(symbol.second.size())*symbol2.second;
            }
        }
    }

    return Bits;
}

void encoder(const string& filename, map<char, string> &symbolTable){


}

map<char, string> sourceAnalysis(const string& filename){
    float totalCount = 0;
    map<char, int> symbolCount = generateSymbols(filename, totalCount);
    map<char, float> symbolProbabilities = calculateProbabilities(symbolCount, totalCount);
    vector<vector<pair<string, float>>> huffmanTree = createHuffmanTable(symbolProbabilities);
    map<char, string> symbolTable = createSymbolTable(huffmanTree);

    float H = calculateEntropy(symbolProbabilities);
    float bitsPerSymbol = calculateBits(symbolTable, symbolProbabilities);

    cout << "\n\nEntropia da fonte: " << H << "\n";
    cout << "Bits/s apos Huffman: " << bitsPerSymbol << "\n";
    cout << "Redundancia: " << bitsPerSymbol - H << "\n";

//    cout << "\n\n Final: \n";
//    for(const auto& elem : huffmanTree.back())
//    {
//        cout << elem.first << " " << elem.second << "\n";
//    }

    return  symbolTable;
}

int main() {
    map<char, string> symbolTable = sourceAnalysis("./../biblia.txt");
    encoder("./../biblia.txt", symbolTable);
    return 0;
}
