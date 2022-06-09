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

intMatrix ImageReader::read(const char *filename, unsigned *dims) {
    intMatrix image;
    vector<int> temp;

    FILE *fin = fopen(filename, "rb");

    if (fin == nullptr){
        printf("\nArquivo de imagem nao encontrado (leitura)!");
        return image;
    }

    int ch;
    int col = 0;
    int pgm[2] = {'P', '5'};
    bool reading_comments = true;

    enum States {Type, Hs, Ws, Gs, Image};

    States state = States::Type;


    unsigned int its = 0;

    while(true){
        ch = getc(fin);
        if (ch == EOF) break;

        its += 1;

        bool comment = false;

        if(ch == '#' && reading_comments){
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
                reading_comments = false;
                if (ch == ' '){
                    dims[1] = vect2int(temp);
                    temp.clear();
                    state = States::Hs;
                }
                else{
                    temp.push_back(ch);
                }
                break;

            case States::Hs:
                if (ch == '\n' || ch == '\r') {
                    dims[0] = vect2int(temp);
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

                if (col == dims[1]){
                    col = 0;
                    image.push_back(temp);
                    temp.clear();
                }
                break;
        }
    }
    fclose(fin);

    return image;
};


fMatrix ImageReader::getBlocks(const unsigned size[2], const intMatrix &image){
    fMatrix blocks;
    unsigned w = image[0].size();
    unsigned h = image.size();
    unsigned rRead = 0;
    unsigned cRead = 0;

    while(true){
        vector<float> block;
        for (unsigned r = rRead; r < rRead + size[0]; ++r) {
            for (unsigned c = cRead; c < cRead + size[1]; ++c) {
                block.push_back(float(image[r][c]));
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

void ImageReader::save_csv(const char *filename, const intMatrix &blocks) {
    FILE *fout = fopen(filename, "wb");

    if (fout == nullptr){
        printf("\nProblema ao criar CSV!");
        return;
    }

    for (const vector<int>& row : blocks) {
        for (int i = 0; i < row.size(); i++) {
            for (char c : to_string(row[i])) {
                putc(c, fout);
            }
            if (i < row.size() - 1){
                putc(',', fout);
            }
        }
        putc('\n', fout);

    }
    fclose(fout);
}


void ImageReader::write(const char *filename, unsigned *dims, const fMatrix &image){
    FILE *fout = fopen(filename, "wb");

    if (fout == nullptr){
        printf("\nArquivo de imagem novo nao encontrado (escrita)!");
        return;
    }

    string header = "P5\n" + to_string(dims[1]) + " " + to_string(dims[0]) + "\n" + to_string(dims[2]) + "\n";

    for(char c : header){
        putc(c, fout);
    }

    unsigned rows = image.size();
    unsigned cols = image[0].size();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            putc((int)round(image[i][j]), fout);
        }
    }

    fclose(fout);
}



int** ImageReader::imatrix2ipointer(const intMatrix& input){
    unsigned rows = input.size();
    unsigned cols = input[0].size();

    int **mat = allocIntMatrix((int)rows, (int)cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = input[i][j];
        }
    }

    return mat;
}

intMatrix ipointer2imatrix(const int **input, int heigth, int width){
    unsigned rows = heigth;
    unsigned cols = width;

    intMatrix mat;

    for (int i = 0; i < rows; ++i) {
        vector<int> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(input[i][j]);
        }
        mat.push_back(row);
    }

    return mat;
}

int** ImageReader::allocIntMatrix(const int rows, const int cols){
    int **mat = (int **)malloc(rows * sizeof(int*));

    for (int i = 0; i < rows; i++) {
        mat[i] = (int *) malloc(cols * sizeof(int));
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat[i][j] = 0;
        }
    }

    return mat;
}

intMatrix ImageReader::float2int(const fMatrix &in) {
    intMatrix out;
    for (const auto & r : in) {
        vector<int> row;
        for (float i : r) {
            row.push_back((int)round(i));
        }
        out.push_back(row);
    }
    return out;
}

fMatrix ImageReader::ipointer2fmatrix(int **input, const unsigned dims[3]) {
    unsigned rows = dims[0];
    unsigned cols = dims[1];

    fMatrix mat;
    for (int i = 0; i < rows; ++i) {
        vector<float> col;
        for (int j = 0; j < cols; ++j) {
            col.push_back((float)input[i][j]);
        }
        mat.push_back(col);
    }

    return mat;
}

fMatrix ImageReader::dpointer2fmatrix(double **input, const unsigned *dims) {



    unsigned rows = dims[0];
    unsigned cols = dims[1];

    fMatrix mat;

    for (int j = 0; j < cols; ++j) {
        vector<float> col;
        for (int i = 0; i < rows; ++i) {
            col.push_back((float)input[i][j]);
        }
        mat.push_back(col);
    }

    return mat;
}

double ImageReader::remove_avg(int **Img_orig, const unsigned *dims) {
    double avg = 0;
    int rows = (int)dims[0];
    int cols = (int)dims[1];

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            avg += (double)Img_orig[i][j]/rows/cols;
        }
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Img_orig[i][j] = Img_orig[i][j] - (int)round(avg);
        }
    }

    return avg;
}

void ImageReader::add_avg(int **Img_orig, const unsigned *dims, double avg) {
    int rows = (int)dims[0];
    int cols = (int)dims[1];


    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Img_orig[i][j] = (int)round(Img_orig[i][j] + avg);
        }
    }
}

intMatrix ImageReader::ipointer2imatrix(int **input, const unsigned int *dims) {
    unsigned rows = dims[0];
    unsigned cols = dims[1];

    intMatrix mat;
    for (int i = 0; i < rows; ++i) {
        vector<int> col;
        for (int j = 0; j < cols; ++j) {
            col.push_back((int)round(input[i][j]));
        }
        mat.push_back(col);
    }

    return mat;
}
