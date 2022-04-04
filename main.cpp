#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <unistd.h>

using namespace std;

bool cmp(pair<string, float>& a,
         pair<string, float>& b)
{
    return a.second > b.second;
}

void encoder(const string& filename){
    ifstream fin(filename);

    char byte = 0;
    vector<char> source;

    // Read file byte by byte

    while (fin.get(byte)) {
        source.push_back(byte);
    }

    fin.close();

    int total = 0;
    map<char, int> symbolCount;

    // Create a map that has keys: symbols and values: occurrences

    for (char c : source){
        total += 1;
        if(symbolCount.count(c)){
            symbolCount[c] += 1;
        }
        else{
            symbolCount.insert(pair<char, int>(c, 1));
        }
    }

    map<char, float> symbolProbabilities;

    // Create a map that has keys: symbols and values: frequencies

//    cout << "Ocorrencias de cada simbolo: \n";
    for (const auto& x : symbolCount) {
//        cout << x.first << ": " << x.second << "\n";
        symbolProbabilities.insert(pair<char, float>(x.first, float(x.second)/float(total)));
    }

//    cout << "Probabilidades de cada simbolo: \n";
//    for(const auto& elem : symbolProbabilities)
//    {
//        std::cout << elem.first << " " << elem.second << "\n";
//    }



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
//    for (int i = 0; i < 2; ++i) {
        vector<pair<string, float>> newRow;
        float pSum = huffmanTree[i].rbegin()[1].second + huffmanTree[i].rbegin()[1].second;
//        cout << "Iteracao " << i << ": " <<huffmanTree[i].rbegin()[0].first << " | " << huffmanTree[i].rbegin()[1].first << "\n";
        string charSum = huffmanTree[i].rbegin()[0].first + huffmanTree[i].rbegin()[1].first;

        newRow.reserve((huffmanTree[i].size() - 2));
        for (int n=0; n<(huffmanTree[i].size() - 2); n++){
            newRow.push_back(huffmanTree[i][n]);
        }
        newRow.emplace_back(charSum, pSum);

        sort(newRow.begin(), newRow.end(), cmp);

        float pTotal = 0;
        for(const auto& elem : newRow)
        {
            pTotal += elem.second;
//        cout << elem.first << " " << elem.second << "\n";
        }

        cout << "Prob Total:  " << pTotal << "\n";

        huffmanTree.push_back(newRow);

    }

//    cout << "\n\n Final: \n";
//    for(const auto& elem : huffmanTree.back())
//    {
//        cout << elem.first << " " << elem.second << "\n";
//    }

}

int main() {
    encoder("./../biblia.txt");
    return 0;
}
