//
// Created by Victor on 16/05/2022.
//

#include <cmath>
#include "VQ.h"
#include "ImageReader.h"
#include <map>
#include <random>

extern "C"
{
#include "subdefs2.h"
}

fMatrix VQ::LGB(const fMatrix &blocks, const unsigned cbSize, const float eps){
    fMatrix codebook;

    unsigned sizeBlock = blocks.size();
    unsigned sizeDim = blocks[0].size();

    vector<float> c0 = vec_avg(blocks, sizeBlock, sizeDim);

    codebook.push_back(c0);

    double avg_dist = dist_avg_c0(c0, blocks);

    while (codebook.size() < cbSize){
        split_codebook(blocks, codebook, eps, avg_dist);
    }

    vector<float> czero(sizeDim,  0.0);
    codebook.push_back(czero);

    return codebook;
}

vector<float> VQ::vec_avg(const fMatrix &blocks, const unsigned sizeBlock, const unsigned sizeDim){
    vector<float> avg(sizeDim, 0.0);

    for(vector<float> block : blocks){
        for (int i = 0; i < sizeDim; ++i) {
            avg[i] += float(block[i])/float(sizeBlock);
        }
    }

    return avg;
}

double VQ::dist_avg_c0(const vector<float> &c0, const fMatrix &blocks){
    double avg_d = 0;
    unsigned block_size = blocks.size();

    for (vector<float> block : blocks) {
        avg_d += euclid_squared(c0, vector<float>(block.begin(), block.end()))/block_size;
    }

    return avg_d;
}

double VQ::euclid_squared(const vector<float> &a, const vector<float> &b){
    double d = 0;

    for (int i = 0; i < a.size(); ++i) {
        d += pow(round(a[i])-round(b[i]), 2);
    }

    return d;
}
void VQ::split_codebook(const fMatrix &blocks, fMatrix &codebook, const float eps, double &initial_avg_dist){

    fMatrix  new_codevectors;

    for (const vector<float> &c : codebook){
        vector<float> c1 = new_codevector(c, eps);
        vector<float> c2 = new_codevector(c, -eps);
        new_codevectors.push_back(c1);
        new_codevectors.push_back(c2);
    }

    codebook = new_codevectors;

    double avg_dist = 0;
    double err = eps + 1;
    unsigned bSize = blocks.size();
    unsigned cSize = codebook.size();
    double lastErr = err;

    while(err > eps){
        if ((lastErr - err)/lastErr > 0.1){
            lastErr = err;
//            printf("\nConvergindo eps: %f/%f", err, eps);
        }

        map<unsigned , vector<float>> closest_c_list;
        map<unsigned , vector<vector<float>>> vecs_near_c;
        map<unsigned , vector<unsigned >> vecs_idxs_near_c;

        unsigned its = 0;

        for (int i = 0; i < bSize; ++i) {
            double min_dist = -1;
            int closest_c_index;
            for (int j = 0; j < cSize; ++j) {
                double d = euclid_squared(blocks[i],codebook[j]);
                if (min_dist < 0 || d < min_dist){
                    min_dist = d;
                    closest_c_list[i] = codebook[j];
                    closest_c_index = j;
                }
            }

            vecs_near_c[closest_c_index].push_back(blocks[i]);
            vecs_idxs_near_c[closest_c_index].push_back(i);

        }

        for (int i = 0; i < cSize; ++i) {
            vector<vector<float>> vecs = vecs_near_c[i];
            unsigned vSize = vecs.size();

            if (vSize > 0){
                vector<float> new_c = vec_avg(vecs, vecs.size(), vecs[0].size());
                codebook[i] = new_c;

                for (unsigned j : vecs_idxs_near_c[i]) {
                    closest_c_list[j] = new_c;
                }
            }
        }

        double prev_avg_dist = initial_avg_dist;
        if (avg_dist > 0){
            prev_avg_dist = avg_dist;
        }
        avg_dist = avg_dist_c_list(closest_c_list, blocks, bSize);
        initial_avg_dist = avg_dist;

        err = (prev_avg_dist - avg_dist)/prev_avg_dist;

        its += 1;
    }
}

double VQ::avg_dist_c_list(const map<unsigned , vector<float>> &c_list, const fMatrix &blocks, const unsigned &bSize){
    map<unsigned , vector<float>>::const_iterator itr;

    double dist = 0;

    for (itr = c_list.begin(); itr != c_list.end(); ++itr) {
        dist += euclid_squared(itr->second, blocks[itr->first])/bSize;
    }

    return dist;
}

vector<float> VQ::new_codevector(const vector<float> &c, float eps){
    vector<float> nc;

    for(float x : c){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(0, 1);
        float e = eps*(float)dist(gen);
        float newValtemp = x * (1 + e);
//        float newVal;
//        if (newValtemp > 255){
//            if(warp){
//                newVal = 0;
//            }else{
//                newVal = x * (1 - e);
//            }
//        }
//        else{
//            if(newValtemp < 0){
//                if (warp){
//                    newVal = 255;
//                }else{
//                    newVal = x * (1 - e);
//                }
//            }
//            else{
//                newVal = newValtemp;
//            }
//        }
        nc.push_back(newValtemp);
    }

    return nc;
}

fMatrix VQ::replaceBlocks(const fMatrix &blocks, const fMatrix &codebook, const unsigned *bDims, const unsigned *iDims, vector<int> &blockList){

    float arrayImg[iDims[0]][iDims[1]];
    fMatrix newBlocks;
//    printf("\nDims: %i/%i", iDims[0], iDims[1]);



    for (const auto &block : blocks) {
        double minD = -1;
        unsigned minIdx = 0;
        for (int c = 0; c < codebook.size(); c++) {
            double d = euclid_squared(block, codebook[c]);
            if (minD == -1 || minD > d){
                minIdx = c;
                minD = d;
            }
        }
        newBlocks.push_back(codebook[minIdx]);
        blockList.push_back(minIdx);
    }

    unsigned iWCounter = 0;
    unsigned iHCounter = 0;


    for (const auto &block : newBlocks){
        unsigned i = 0;
        for (unsigned r = iHCounter; r < iHCounter + bDims[0]; ++r) {
            vector<float> row;
            for (unsigned c = iWCounter; c < iWCounter + bDims[1]; ++c) {
                arrayImg[r][c] = block[i];
                i += 1;

            }
        }
//        printf("\n %i \n", iDims[1]);
//        iHCounter += bDims[1];
        iWCounter += bDims[1];
        if (iWCounter >= iDims[1]){
            iHCounter += bDims[0];
            iWCounter = 0;
        }
    }

    fMatrix newImage;

    for (int i = 0; i < iDims[0]; ++i) {
        vector<float> row;

        for (int j = 0; j < iDims[1]; ++j) {
            row.push_back(arrayImg[i][j]);

        }
        newImage.push_back(row);
    }
    return newImage;
}

double VQ::MSE(const intMatrix &oldI, const fMatrix &newI){
    double err = 0;
    unsigned rows = oldI.size();
    unsigned cols = oldI[0].size();
    unsigned its = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            err += pow((float)oldI[i][j] - round(newI[i][j]), 2)/(cols*rows);
        }
    }

    return err;
}

double VQ::PSNR(const intMatrix &oldI, const fMatrix &newI){
    return 10*log10(255*255/MSE(oldI, newI));
}

vector<unsigned> VQ::best_codebook(const intMatrix &image, const vector<fMatrix> &block_list, const vector<fMatrix> &codebook_list, const unsigned *dims, unsigned testIdx){
    unsigned bcb = 0;
    unsigned bestcbSize;
    double maxPSNR = -1;
    double maxR = 0;
    unsigned cIdx = 0;
    unsigned bcIdx = 0;
    vector<vector<string>> performance;

    printf("\nCalculo do melhor codebook:");

    for (int i = 0; i < block_list.size(); ++i) {
        for (unsigned int cb_size : cb_size_list) {
            unsigned bSize = vector_list[i][0] * vector_list[i][1];
            double R = log2(cb_size+1)/bSize;
            if(R <= 7) {

                vector<int> newBlocks;
                fMatrix newImage = VQ::replaceBlocks(block_list[i], codebook_list[cIdx], vector_list[i], dims, newBlocks);
                double psnr = VQ::PSNR(image, newImage);


                printf("\n[%i|%i] PSNR = %f R = %f", i,cIdx, psnr, R);

                vector<string> p_row = {to_string(vector_list[i][0]), to_string(vector_list[i][1]), to_string(cb_size),
                                        to_string(psnr), to_string(R)};
                performance.push_back(p_row);

                if (psnr < 0 || (psnr > maxPSNR && maxPSNR < minPSNR) || (psnr > minPSNR && R < maxR)) {
                    maxPSNR = psnr;
                    bestcbSize = cb_size;
                    maxR = R;
                    bcb = i;
                    bcIdx = cIdx;
                }
                cIdx += 1;
            }
        }

    }


//    ImageReader::save_csv(("./desempenhos/performance_" + to_string(testIdx) + "_" + to_string(minPSNR) + ".csv").c_str(), performance, false);

    printf("\nMelhor codebook: [%i/%i] PSNR = %f R = %f", bcb, bcIdx, maxPSNR, maxR);

    vector<unsigned > best = {bcb, bcIdx, bestcbSize, vector_list[bcb][0], vector_list[bcb][1]};

    return best;
}

 void VQ::save_codebooks(const string& filename, const vector<fMatrix> &codebook_list, const intMatrix& dims){
     FILE *fout = fopen(filename.c_str(), "wb");

     if (fout == nullptr){
         printf("\nArquivo de codebooks nao encontrado (escrita)!");
         return;
     }

     unsigned i = 0;
     for(const auto &codebook : codebook_list){
//         printf("\nDims: %i/%i", dims[i][0], dims[i][1]);
         for(const char &c : to_string(dims[i][0])){
             putc(c, fout);
         }
         putc(',', fout);
         for(const char &c : to_string(dims[i][1])){
             putc(c, fout);
         }
         putc(',', fout);
         i += 1;
         for (const auto & row : codebook) {
             for(const auto &point : row){
                 string temp = to_string(point);
                 for(char c : temp){
                     putc(c, fout);
                 }
                 putc(',', fout);
             }
         }
         putc('\n', fout);
     }

     fclose(fout);
}

vector<fMatrix> VQ::load_codebooks(const string& filename){
    FILE *fin = fopen(filename.c_str(), "rb");
    vector<fMatrix> codebook_list;

    if (fin == nullptr){
        printf("\nArquivo de codebooks nao encontrado (leitura)!");
        return codebook_list;
    }

    enum States {bsize, csize, vect};
    States state = States::bsize;
    string buffer;
    unsigned bsizeRead = 0;
    unsigned csizeRead = 0;
    bool first = false;

    while(true){
        char ch;
        if(state != States::vect){
            ch = (char)getc(fin);
            if(ch == EOF) break;
        }

        switch (state) {
            case States::bsize:
                if (ch == ','){
                    state = States::csize;
                    bsizeRead = stoi(buffer);
                    buffer = "";
                }
                else{
                    buffer += ch;
                }
                break;
            case States::csize:
                if (ch == ','){
                    state = States::vect;
                    csizeRead = stoi(buffer);
                    buffer = "";
                }
                else{
                    buffer += ch;
                }
                break;
            case States::vect:
                fMatrix codebook;
                for (int i = 0; i < csizeRead; i++) {
                    vector<float> row;
                    for (int j = 0; j < bsizeRead; j++) {
                        string temp;
                        while(true){
                            char c = (char)getc(fin);
                            if(c == ',') break;
                            temp += c;
                        }
                        row.push_back(stof(temp));
                    }
                    codebook.push_back(row);
//                    getc(fin);
                }
                codebook_list.push_back(codebook);
                state = States::bsize;
                break;
        }
    }

    fclose(fin);

    return codebook_list;
}

vector<vector<performance>> VQ::evaluate_codebooks(const vector<intMatrix> &subbands) {
    unsigned count = 0;
    vector<vector<performance>> allPerformances;
    printf("\nAvaliando codebooks:");
    for (int j = 0; j < NBANDS; ++j) {
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(j) + ".txt");

        vector<fMatrix> block_list;

        for(const unsigned *block_size : vector_list){
            fMatrix test_blocks = ImageReader::getBlocks(block_size, subbands[j]);
            block_list.push_back(test_blocks);
        }

        unsigned cIdx = 0;

        vector<performance> subbandPer;

        for (int i = 0; i < block_list.size(); ++i) {
            for (unsigned int cb_size : cb_size_list) {
                unsigned bSize = vector_list[i][0] * vector_list[i][1];
                double R = log2(cb_size+1)/bSize;
                if(R <= MAXR) {
                    unsigned dims[3] = {(unsigned)subbands[j].size(), (unsigned)subbands[j][0].size(), 255};
                    vector<int> bestblockList;
                    fMatrix newImage = VQ::replaceBlocks(block_list[i], codebook_list[cIdx], vector_list[i], dims, bestblockList);
                    if(cb_size == 16 && vector_list[i][0] == 2 && j == 0){
                        ImageReader::save_csv("teste.csv", ImageReader::float2int(block_list[i]));
                        ImageReader::save_csv("teste2.csv", ImageReader::float2int(codebook_list[cIdx]));
                    }

                    double mse = VQ::MSE(subbands[j], newImage);
                    double psnr = 10*log10(255*255/mse);

                    printf("\n%i/%i MSE = %f PSNR = %f R = %f", count, j, mse, psnr, R);

                    performance per;
                    per.R = R;
                    per.MSE = mse;
                    per.PSNR = psnr;
                    per.block_size[0] = vector_list[i][0];
                    per.block_size[1] = vector_list[i][1];
                    per.codebook_size = cb_size+1;
                    per.codebook_idx = cIdx;
                    per.blockList = bestblockList;
                    subbandPer.push_back(per);

                    count += 1;
                    cIdx += 1;
                }
            }

        }
        allPerformances.push_back(subbandPer);

//        vector<unsigned> bc = VQ::best_codebook(subbands[i], block_list, codebook_list, dims, iIdx);
//        fMatrix newImage  = VQ::replaceBlocks(block_list[bc[0]], codebook_list[bc[1]], vector_list[bc[0]], dims);
//        best.push_back({to_string(bc[3]), to_string(bc[4]), to_string(bc[0]), to_string(bc[2])});
//        iIdx += 1;

    }
    return allPerformances;
}

fMatrix VQ::fill_image(const intMatrix &allBlocks, const vector<fMatrix> &selected_codebooks, const vector<codebookInfo>& selected_infos, unsigned *dims) {
    fMatrix newImage(dims[0],vector<float>(dims[1], 0));
    vector<int> levelCounter(NSTAGES, 0);

    for (int subband = 0; subband < NBANDS; ++subband) {
        int level = (int)floor((subband-1)/3) + 1;
        if (subband == 0){
            level = (NBANDS-1)/3;
        }

        fMatrix codebook = selected_codebooks[subband];
        codebookInfo info = selected_infos[subband];
        unsigned bH = info.blockH;
        unsigned bW = info.blockW;
        unsigned subH = dims[0]/(unsigned) pow(2, level);
        unsigned subW = dims[1]/(unsigned) pow(2, level);
        unsigned subStartX;
        unsigned subStartY;
        int startPosMult[3][2] = {{1, 0}, {1,1}, {0, 1}};
        if (subband == 0){
            subStartX = 0;
            subStartY = 0;
        }
        else{
            subStartX = startPosMult[levelCounter[level-1]][0]*subW;
            subStartY = startPosMult[levelCounter[level-1]][1]*subH;
            levelCounter[level-1] += 1;
        }

        unsigned startY = 0;
        unsigned startX = 0;

//        printf("\n\nDebug H: %i/%i/%i", level, subH, subStartY);
//        printf("\nDebug W: %i/%i/%i", level, subW, subStartX);
//        printf("\n\n");

        for (int i = 0; i < allBlocks[subband].size(); ++i) {
            vector<float> block = codebook[allBlocks[subband][i]];
            unsigned bCounter = 0;
            for (unsigned k = subStartX + startX; k < subStartX + startX + bW; ++k) {
                for (unsigned j = subStartY + startY; j < subStartY + startY + bH; ++j) {
                    newImage[j][k] = (float)block[bCounter];
                    bCounter += 1;
//                    printf("%i/%i ", j, k);
                }
            }
//            printf("\n");
            startX += bW;
            if (startX >= subW){
                startX = 0;
                startY += bH;
            }

        }
    }




    return newImage;
}

