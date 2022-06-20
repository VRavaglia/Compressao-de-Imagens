//
// Created by Victor on 16/05/2022.
//

#include <cmath>
#include "VQ.h"
#include "ImageReader.h"
#include <map>
#include <random>
#include "cb_list.h"
#include <fstream>

extern "C"
{
#include "subdefs2.h"
}

int VQ::LGB(const fMatrix &blocks, const unsigned cbSize, const float eps, float parada, fMatrix &codebook){

    unsigned sizeBlock = blocks.size();
    unsigned sizeDim = blocks[0].size();

    vector<float> c0 = vec_avg(blocks, sizeBlock, sizeDim);

    codebook.push_back(c0);

    double avg_dist = dist_avg_c0(c0, blocks);

    while (codebook.size() < cbSize){
        int result = split_codebook(blocks, codebook, eps, parada, avg_dist);
        printf("\nGerando Codebook: %i/%i", codebook.size(), cbSize);
        if (result != 0) return result;
    }

    vector<float> czero(sizeDim,  0.0);
    codebook.push_back(czero);

    return 0;
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
        d += pow(a[i]-b[i], 2);
    }

    return d;
}
int VQ::split_codebook(const fMatrix &blocks, fMatrix &codebook, const float eps, const float parada, double &initial_avg_dist){

    fMatrix  new_codevectors;

    for (const vector<float> &c : codebook){
        vector<float> c1 = new_codevector(c, eps);
        vector<float> c2 = new_codevector(c, -eps);
        new_codevectors.push_back(c1);
        new_codevectors.push_back(c2);
    }

    codebook = new_codevectors;

    double avg_dist = 0;
    double err = parada + 1;
    unsigned bSize = blocks.size();
    unsigned cSize = codebook.size();
    double lastErr = err;
    unsigned its = 0;

    while(err > parada){

        map<unsigned , vector<float>> closest_c_list;
        map<unsigned , vector<vector<float>>> vecs_near_c;
        map<unsigned , vector<unsigned >> vecs_idxs_near_c;



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
//        if (its > MAXVQITS) printf("\nMaior que 20");
//        if (its > MAXVQITS) return 0;
    }
    return 0;
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
        float delta = abs(eps);
        std::normal_distribution<> dist(0, delta);
//        float e = eps*(float)dist(gen);
        auto e = (float)dist(gen);
//        float newValtemp = x + e + eps;
        float newValtemp = x + e;
        if (eps < 0) newValtemp = x - e;
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

fMatrix VQ::replaceBlocks(const fMatrix &blocks, const fMatrix &codebook, const vector<int>& bDims, const unsigned *iDims, vector<int> &blockList){

    float **arrayImg = ImageReader::allocfMatrix((int)iDims[0], (int)iDims[1]);
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
        blockList.push_back((int)minIdx);
    }

    unsigned iWCounter = 0;
    unsigned iHCounter = 0;


    for (const auto &block : newBlocks){
        unsigned i = 0;
        for (unsigned r = iHCounter; r < iHCounter + bDims[0]; ++r) {
            vector<float> row;
            for (unsigned c = iWCounter; c < iWCounter + bDims[1]; ++c) {
                if(r < iDims[0] && c < iDims[1]){
                    arrayImg[r][c] = block[i];
                }
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
    free(arrayImg);
    return newImage;
}

double VQ::MSE(const fMatrix &oldI, const fMatrix &newI){
    double err = 0;
    unsigned rows = oldI.size();
    unsigned cols = oldI[0].size();
    unsigned its = 0;
//    float lasterr = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            err += pow(oldI[i][j] - newI[i][j], 2)/(cols*rows);
//                printf("\nerr %f/%f", oldI[i][j], newI[i][j]);
//            }
//            lasterr = err;
        }
    }

    return err;
}

double VQ::PSNR(const fMatrix &oldI, const fMatrix &newI){
    return 10*log10(255*255/MSE(oldI, newI));
}

//vector<unsigned> VQ::best_codebook(const fMatrix &image, const vector<fMatrix> &block_list, const vector<fMatrix> &codebook_list, const unsigned *dims, unsigned testIdx){
//    unsigned bcb = 0;
//    unsigned bestcbSize;
//    double maxPSNR = -1;
//    double maxR = 0;
//    unsigned cIdx = 0;
//    unsigned bcIdx = 0;
//    vector<vector<string>> performance;
//
//    printf("\nCalculo do melhor codebook:");
//
//    for (int i = 0; i < block_list.size(); ++i) {
//        for (unsigned int cb_size : csize_list()) {
//            unsigned bSize = vector_list[i][0] * vector_list[i][1];
//            double R = log2(cb_size+1)/bSize;
//            if(R <= 7) {
//
//                vector<int> newBlocks;
//                fMatrix newImage = VQ::replaceBlocks(block_list[i], codebook_list[cIdx], vector_list[i], dims, newBlocks);
//                double psnr = VQ::PSNR(image, newImage);
//
//
//                printf("\n[%i|%i] PSNR = %f R = %f", i,cIdx, psnr, R);
//
//                vector<string> p_row = {to_string(vector_list[i][0]), to_string(vector_list[i][1]), to_string(cb_size),
//                                        to_string(psnr), to_string(R)};
//                performance.push_back(p_row);
//
//                if (psnr < 0 || (psnr > maxPSNR && maxPSNR < minPSNR) || (psnr > minPSNR && R < maxR)) {
//                    maxPSNR = psnr;
//                    bestcbSize = cb_size;
//                    maxR = R;
//                    bcb = i;
//                    bcIdx = cIdx;
//                }
//                cIdx += 1;
//            }
//        }
//
//    }
//
//
////    ImageReader::save_csv(("./desempenhos/performance_" + to_string(testIdx) + "_" + to_string(minPSNR) + ".csv").c_str(), performance, false);
//
//    printf("\nMelhor codebook: [%i/%i] PSNR = %f R = %f", bcb, bcIdx, maxPSNR, maxR);
//
//    vector<unsigned > best = {bcb, bcIdx, bestcbSize, vector_list[bcb][0], vector_list[bcb][1]};
//
//    return best;
//}

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
                 bool mark = false;
                 for(char c : temp){
                     putc(c, fout);
                     if(mark) break;
                     if(c == '.') mark = true;

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
    int codebooks = 0;

    while(true){
//        if(codebooks > 9){
//            printf("");
//        }
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
//                printf("\n Size: %i/%i",bsizeRead, csizeRead);
                for (int i = 0; i < csizeRead; i++) {
                    vector<float> row;
                    for (int j = 0; j < bsizeRead; j++) {
                        string temp;
                        while(true){
                            char c = (char)getc(fin);
                            if(c == ',') break;
                            temp += c;

                        }
//                        if (codebooks > 11){
//                            printf(" %i/%i", i, csizeRead);
//                        }
                        row.push_back(stof(temp));
                    }
                    codebook.push_back(row);
//                    getc(fin);
                }
                codebook_list.push_back(codebook);
                codebooks += 1;
//                printf("\n Codebooks lidos: %i/%i", codebooks, vl_size*cb_size_size);
                state = States::bsize;
                break;
        }
    }

    fclose(fin);

    return codebook_list;
}

vector<vector<performance>> VQ::evaluate_codebooks(const vector<fMatrix> &subbands, vector<intMatrix>& bb_idx) {
    unsigned count = 0;
    vector<vector<performance>> allPerformances;

    printf("\nAvaliando codebooks:");
    for (int j = 0; j < NBANDS; ++j) {
        printf("\nCarregando codebooks: %i/%i", j+1, NBANDS);
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(j) + ".txt");

        vector<fMatrix> block_list;

        for(const auto& block_size : bsize_list(j)){
            fMatrix test_blocks = ImageReader::getBlocks(block_size, subbands[j]);
            block_list.push_back(test_blocks);
        }

        unsigned cIdx = 0;

        vector<performance> subbandPer;
        intMatrix subband_bb_idx;

        for (int i = 0; i < block_list.size(); ++i) {
            double lastPSNR = 0;
            bool skip = false;
            for (int cb_size : csize_list(j)) {
                unsigned bSize = bsize_list(j)[i][0] * bsize_list(j)[i][1];
                double R = log2(cb_size+1)/bSize;
//                if(bsize_list(j)[i][0] + bsize_list(j)[i][1] > MAXBLOCK) skip = true;
                if(R <= MAXR) {
                    performance per;
                    if(!skip){
                        unsigned dims[3] = {(unsigned)subbands[j].size(), (unsigned)subbands[j][0].size(), 255};
                        vector<int> bestblockList;
                        fMatrix newImage = VQ::replaceBlocks(block_list[i], codebook_list[cIdx], bsize_list(j)[i], dims, bestblockList);
                        subband_bb_idx.push_back(bestblockList);

//                        ImageReader::save_histogram

//                        if(cb_size == 16 && vector_list[i][0] == 2 && j == 0){
//                            ImageReader::save_csv("teste.csv", ImageReader::float2int(block_list[i]));
//                            ImageReader::save_csv("teste2.csv", ImageReader::float2int(codebook_list[cIdx]));
//                        }
//                        if(count > 39){
//                            printf("");
//                        }
                        double mse = VQ::MSE(subbands[j], newImage);
//                        if(std::isnan(mse)) mse = pow(10, 9);
                        if(std::isnan(mse)) exit(-1);
                        double psnr = 10*log10(255*255/mse);

                        printf("\n[%i/%i] SB: %i/10 MSE = %f PSNR = %f R = %f", count, NBANDS*csize_list(j).size()*bsize_list(j).size(), j, mse, psnr, R);

                        per.R = R;
                        per.MSE = mse;
//                        per.PSNR = psnr;
//                        per.blockList = bestblockList;
//                        if(lastPSNR != 0 && psnr <= lastPSNR + 0.01){
//                            skip = true;
//                        }
                        lastPSNR = psnr;

                    }else{
                        printf("\n[%i/%i] SB: %i/10 Skip", count, NBANDS*csize_list(j).size()*bsize_list(j).size(), j);
                        per.R = -1;
                        per.MSE = pow(10,9);
//                        per.PSNR = psnr;

                    }
                    per.block_size[0] = bsize_list(j)[i][0];
                    per.block_size[1] = bsize_list(j)[i][1];
                    per.codebook_size = cb_size+1;
                    per.codebook_idx = cIdx;
                    subbandPer.push_back(per);

                    count += 1;
                    cIdx += 1;
                }
            }

        }
        bb_idx.push_back(subband_bb_idx);
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
//per[imagem][banda][config]
void VQ::save_performances(const vector<vector<vector<performance>>> &performances, const vector<string>& img_names) {
    vector<vector<performance>> means;
//    printf("\nSizes: %i/%i/%i", performances.size(), performances[0].size(), performances[0][0].size());
    for (int i = 0; i < NBANDS; ++i) {
        vector<performance> temp;
        for (int j = 0; j < performances[0][i].size(); ++j) {
            performance mean_per;
            mean_per.codebook_size = performances[0][i][j].codebook_size;
            mean_per.codebook_idx = performances[0][i][j].codebook_idx;
            mean_per.block_size[0] = performances[0][i][j].block_size[0];
            mean_per.block_size[1] = performances[0][i][j].block_size[1];
            for (int k = 0; k < img_names.size(); k++){
                mean_per.MSE += performances[k][i][j].MSE/(double)img_names.size();
                mean_per.R += performances[k][i][j].R/(double)img_names.size();
            }
            temp.push_back(mean_per);
        }
        means.push_back(temp);
    }


    FILE *fout = fopen("./performances/performances.txt", "wb");

    if (fout == nullptr){
        printf("\nArquivo de avaliacoes nao encontrado (escrita)!");
        return;
    }


    for (int i = 0; i < NBANDS; ++i) {
        for (int j = 0; j < performances[0][i].size(); ++j) {
            for(const char &c : to_string(i)){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].codebook_size)){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].codebook_idx)){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].MSE)){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].R)){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].block_size[0])){
                putc(c, fout);
            }
            putc(',', fout);
            for(const char &c : to_string(means[i][j].block_size[1])){
                putc(c, fout);
            }
            putc(',', fout);
            putc('\n', fout);
        }
    }



    fclose(fout);
}

vector<vector<performance>> VQ::load_performances(const string& filename){
    FILE *fin = fopen(filename.c_str(), "rb");
    vector<vector<performance>> subbs_performances;

    if (fin == nullptr){
        printf("\nArquivo de avaliacoes nao encontrado (leitura)!");
        return subbs_performances;
    }
    while (true){
        char ch = (char)getc(fin);
        if(ch == ',') break;
    }
    for (int k = 0; k < NBANDS; ++k) {
        vector<performance> subb_performances;
        while(true){
            performance per;
            string buffer;
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.codebook_size = stoi(buffer);
            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.codebook_idx = stoi(buffer);
            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.MSE = stof(buffer);
            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.R = stof(buffer);
            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.block_size[0] = stoi(buffer);
            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == ',') break;
                buffer += ch;

            }
            per.block_size[1] = stoi(buffer);
            subb_performances.push_back(per);

            (char)getc(fin);

            buffer = "";
            while (true){
                char ch = (char)getc(fin);
                if(ch == EOF){
                    fclose(fin);
                    subbs_performances.push_back(subb_performances);
                    return subbs_performances;
                }
                if(ch == ',') break;
                buffer += ch;
            }
            int band = stoi(buffer);
            if(band > k) break;
        }
        subbs_performances.push_back(subb_performances);
    }
    fclose(fin);
    return subbs_performances;
}

void VQ::save_histograms(const vector<vector<vector<vector<int>>>> &idx_list, const vector<vector<vector<performance>>>& performances) {
    vector<vector<vector<int>>> histograms;
//    printf("\nSizes: %i/%i/%i", performances.size(), performances[0].size(), performances[0][0].size());
    for (int i = 0; i < NBANDS; ++i) {
        vector<performance> temp;
        vector<vector<int>> temp_hists;
        for (int j = 0; j < performances[0][i].size(); ++j) {
            unsigned cbsize = performances[0][i][j].codebook_size;
            vector<int> hist(cbsize, 0);

            for (int k = 0; k < idx_list.size(); k++){
//                printf("\nHisto: %i/%i/%i", k, i, j);
//                printf("\nIdx_list: %i/%i/%i/%i", idx_list.size(), idx_list[k].size(), idx_list[k][i].size(), idx_list[k][i][j].size());
                for (int l = 0; l < idx_list[k][i][j].size(); ++l) {
                    unsigned location = idx_list[k][i][j][l];
                    hist[location] += 1;
                }
            }
            for (int & l : hist) {
                l = (int)round((float)l/idx_list.size());
            }
            temp_hists.push_back(hist);
        }
        histograms.push_back(temp_hists);
    }

    printf("\nEscrita");


    FILE *fout = fopen("./performances/histograms.txt", "wb");

    if (fout == nullptr){
        printf("\nArquivo de histogramas nao encontrado (escrita)!");
        return;
    }

    for (int i = 0; i < NBANDS; ++i) {
        for (int j = 0; j < performances[0][i].size(); ++j) {
            for(const char &c : to_string(i)){
                putc(c, fout);
            }
            for (int k = 0; k < histograms[i][j].size(); ++k) {
                putc(',', fout);
                for(const char &c : to_string(histograms[i][j][k])){
                    putc(c, fout);
                }
            }
            putc(',', fout);
            putc('\n', fout);
        }
    }

    fclose(fout);
}

int *VQ::load_model(int sband, int cb_idx){

    ifstream input("./performances/histograms.txt");
    printf("\n CBidx: %i", cb_idx);

    vector<int> temp_cum;
    int counter = 0;
    for( string line; getline( input, line ); )
    {
//        printf("\nLine: %c", line[0]);
        if(line[0] - '0' == sband){
//            printf("\nCounter: %i", counter);
            if(counter == cb_idx){
                string buffer;
                for (int i = 0; i < line.size(); ++i) {
                    if(i > 1){
                        if (line[i] == ','){
                            temp_cum.push_back(stoi(buffer));
                            buffer = "";
                        }else{
                            buffer += line[i];
                        }
                    }
                }
            }
            counter += 1;
        }
    }
    unsigned freq_size = temp_cum.size()+2;
    int *cum_freq = (int *)calloc (freq_size,  sizeof (int));
    cum_freq[freq_size-1] = 0;
//    printf("\n\n 0");
    for (unsigned i = freq_size-2; i > 0; --i) {
        cum_freq[i] = cum_freq[i+1] + temp_cum[i-1] + 1;
//        cum_freq[i] = cum_freq[i+1] + temp_cum[freq_size-i] + 1;
//        printf(" %i", cum_freq[i]);
    }
    cum_freq[0] = cum_freq[1]+1;
//    printf(" %i", cum_freq[0]);
    return cum_freq;
}
