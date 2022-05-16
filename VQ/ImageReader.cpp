//
// Created by Victor on 16/05/2022.
//

#include "ImageReader.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

using namespace std;

int ImageReader::vect2int(const vector<int> &in){
    int out = 0;
    for(int i = in.size() - 1; i >= 0; i--){
        out += (in[i]-'0')*pow(10,(in.size() - i - 1));
    }
}

vector<vector<int>> ImageReader::read(const char *filename) {
    vector<vector<int>> image;
//    int row = 0, col = 0, numrows = 0, numcols = 0;
//    ifstream infile(filename);
//    stringstream ss;
//    string inputLine;
//
//    // First line : version
//    getline(infile,inputLine);
//    if(inputLine != "P5") cerr << "Version error" << endl;
//    else cout << "Version : " << inputLine << endl;
//
//
//    // Continue with a stringstream
//    ss << infile.rdbuf();
//    // Third line : size
//    ss >> numcols >> numrows;
//    cout << numcols << " columns and " << numrows << " rows" << endl;
//
//    // Third line : gray
//    getline(infile,inputLine);
//    cout << "GrayLvl : " << inputLine << endl;
//
//    int array[numrows][numcols];
//
//    // Following lines : data
//    for(row = 0; row < numrows; ++row)
//        for (col = 0; col < numcols; ++col) ss >> array[row][col];
//
//    // Now print the array to see the result
//    for(row = 0; row < numrows; ++row) {
//        vector<int> rowVec;
//        for(col = 0; col < numcols; ++col) {
//            cout << array[row][col] << " ";
//            rowVec.push_back(array[row][col]);
//        }
//        cout << endl;
//        image.push_back(rowVec);
//    }
//    infile.close();


    vector<int> temp;

    FILE *fin = fopen(filename, "rb");

    int ch;
    int col = 0;
    int pgm[2] = {'P', '5'};

    enum States {Type, Hs, Ws, Gs, Image};

    States state = States::Type;


    unsigned int its = 0;

    while(true){
        ch = getc(fin);
        if (ch == EOF) break;

        its += 1;

        switch (state) {
            case States::Type:
                if (ch == '\n' || ch == '\r'){
                    state = States::Ws;
                }
                else{
                    if(ch!= pgm[its-1]){
                        printf("\nArquivo com formato errado. Deve ser pgm!");
                        return image;
                    }
                }
                break;

            case States::Ws:
                if (ch == ' '){
                    this->W = vect2int(temp);
                    temp.clear();
                    state = States::Hs;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Hs:
                if (ch == '\n' || ch == '\r') {
                    this->H = vect2int(temp);
                    temp.clear();
                    state = States::Gs;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Gs:
                if (ch == '\n' || ch == '\r'){
                    this->G = vect2int(temp);
                    temp.clear();
                    state = States::Image;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Image:
                temp.push_back(ch);
                col += 1;

                if (col == this->W){
                    col = 0;
                    image.push_back(temp);
                    temp.clear();
                }
                break;
        }
    }

    return image;
};

