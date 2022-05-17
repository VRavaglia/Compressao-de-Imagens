//
// Created by Victor on 16/05/2022.
//

#include "ImageReader.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

int ImageReader::vect2int(const vector<int> &in){
    int out = 0;
    for(int i = in.size() - 1; i >= 0; i--){
        out += (in[i]-'0')*pow(10,(in.size() - i - 1));
    }

    return out;
}

intMatrix ImageReader::read(const char *filename, int *dims) {
    intMatrix image;
    vector<int> temp;

    FILE *fin = fopen(filename, "rb");

    if (fin == nullptr){
        printf("Arquivo de imagem nao encontrado!");
        return image;
    }

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

        bool comment = false;

        if(ch == '#'){
            comment = true;
        }

        while (comment){
            while(ch != '\n' && ch != '\r'){
                ch = getc(fin);
            }
            ch = getc(fin);
            comment = false;
            if(ch == '#') comment = true;
        }

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
                    dims[0] = vect2int(temp);
                    temp.clear();
                    state = States::Hs;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Hs:
                if (ch == '\n' || ch == '\r') {
                    dims[1] = vect2int(temp);
                    temp.clear();
                    state = States::Gs;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Gs:
                if (ch == '\n' || ch == '\r'){
                    dims[2] = vect2int(temp);
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

                if (col == dims[0]){
                    col = 0;
                    image.push_back(temp);
                    temp.clear();
                }
                break;
        }
    }

    return image;
};


intMatrix ImageReader::getBlocks(const unsigned size[2], intMatrix &image){
    intMatrix blocks;
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
        }
        cRead += size[1];
        blocks.push_back(block);
        if (cRead >= w){
            rRead += size[0];
            if (rRead*cRead == w*h) break;
            cRead = 0;
        }
    }

    return blocks;
};

