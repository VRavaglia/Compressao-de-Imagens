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


vector<vector<int>> ImageReader::getBlocks(const unsigned size[2], imageType &image){
    vector<vector<int>> blocks;
    unsigned w = image[0].size();
    unsigned h = image.size();
    unsigned rRead = 0;
    unsigned cRead = 0;

    while(true){
        vector<int> block;
        for (unsigned r = rRead; r < rRead + size[0]; ++r) {
            for (unsigned c = cRead; c < cRead + size[1]; ++c) {
                block.push_back(image[r][c]);
            }
            cRead += size[1];
        }
        blocks.push_back(block);
        if (cRead >= w){
            rRead += size[0];
            if (rRead*cRead == w*h) break;
            cRead = 0;
        }
    }

    return blocks;
};

