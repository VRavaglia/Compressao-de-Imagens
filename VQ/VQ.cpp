//
// Created by Victor on 16/05/2022.
//https://github.com/internaut/py-lbg/blob/53c1c77f67192745c27f1dfbee9ef976e9156f35/lbg.py#L145
//

#include <cmath>
#include "VQ.h"
#include "ImageReader.h"
#include <map>

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
        d += pow(a[i]-b[i], 2);
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
//                        closest_c_list.insert(pair<unsigned , vector<float>>(i, codebook[j]));
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

vector<float> VQ::new_codevector(const vector<float> &c, float e){
    vector<float> nc;

    for(float x : c){
        nc.push_back(x * (1 + e));
    }

    return nc;
}

fMatrix VQ::replaceBlocks(const fMatrix &blocks, const fMatrix &codebook, const unsigned *bDims, const unsigned *iDims){

    float arrayImg[iDims[0]][iDims[1]];
    fMatrix newBlocks;


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

unsigned VQ::best_codebook(const intMatrix &image, const vector<fMatrix> &block_list, const vector<fMatrix> &codebook_list, const unsigned *dims){
    unsigned bcb = 0;
    unsigned bcsize = 0;
    unsigned bbSize = 0;
    double maxPSNR = -1;
    unsigned vIdx = 0;
    unsigned cIdx = 0;

    printf("\nCalculo do melhor codebook:");

    for (int i = 0; i < block_list.size(); ++i) {
        fMatrix newImage  = VQ::replaceBlocks(block_list[i], codebook_list[i], vector_list[vIdx], dims);
        double psnr = VQ::PSNR(image, newImage);
        unsigned bSize = vector_list[vIdx][0] * vector_list[vIdx][1];

        printf("\nPSNR [%i] v = %i c = %i: %f", i, bSize, cb_size_list[cIdx], psnr);

        if (psnr < 0 || (psnr > maxPSNR && maxPSNR < 40) || (psnr > 40 && bSize < bbSize)){
            maxPSNR = psnr;
            bcsize = cIdx;
            bbSize = bSize;
            bcb = i;
        }

        cIdx += 1;
        if(cIdx > 3){
            cIdx = 0;
            vIdx += 1;
        }

    }

    printf("\nMelhor: PSNR [%i] v = %i c = %i: %f", bcb, bbSize, cb_size_list[bcsize], maxPSNR);

    return bcb;
}