//
// Created by Victor on 03/06/2022.
//

#include "EncoderWrapper.h"
#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
#include "cb_list.h"
extern "C"
{
#include "subdefs2.h"
#include "sub.h"
#include "bit_output.h"
#include "bit_input.h"
#include "arithmetic_encode.h"
#include "arithmetic_decode.h"
#include "histogram.h"
}


void EncoderWrapper::encode(const string& in, const string& out, float lb) {
    //******************************************************************************
    //*                                                                            *
    //*                         Quantizar Subbandas                                *
    //*                                                                            *
    //******************************************************************************

    char * imgPath = const_cast<char *>(in.c_str());
    FILE *fin = fopen(imgPath, "rb");
    fseek(fin, 0, SEEK_END);
    long file_size = ftell(fin);
    fclose(fin);

    unsigned dims[3];
    intMatrix image = ImageReader::read(imgPath, dims);
    int **Image_orig = ImageReader::imatrix2ipointer(image);
    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);


    double *pSIMG[YLUM];
    int avg = ImageReader::remove_avg(Image_orig, dims);
//    int avg = 0;
    only_anal(Image_orig, pSIMG, (int)dims[1], (int)dims[0]);

//    double avg = 0;
//    for (int k = 0; k < (int)dims[0]/8; ++k) {
//        for (int j = 0; j < (int)dims[1]/8; ++j) {
//            avg += pSIMG[k][j]/((float)dims[0]/8*(float)dims[1]/8);
//        }
//    }
//    for (int i = 0; i < (int)dims[0]/8; ++i) {
//        for (int j = 0; j < (int)dims[1]/8; ++j) {
//            pSIMG[i][j] -= round(avg);
//        }
//    }
//    double media = 0;
//    for (int i = 0; i < 64; ++i) {
//        for (int j = 0; j < 64; ++j) {
//            media += pSIMG[i][j]/4096;
//        }
//    }
//    for (int i = 0; i < 64; ++i) {
//        for (int j = 0; j < 64; ++j) {
//            pSIMG[i][j] -= media;
//        }
//    }
//
//    for (int i = 0; i < 64; ++i) {
//        for (int j = 0; j < 64; ++j) {
//            pSIMG[i][j] += media;
//        }
//    }

//    only_synt(Image_out, pSIMG, (int)dims[1], (int)dims[0]);
//    ImageReader::add_avg(Image_out, dims, avg);
//    fMatrix FImage_out = ImageReader::ipointer2fmatrix(Image_out, dims);

//    ImageReader::write((out+"wav.pgm").c_str(), dims, deecodedImage);
//    ImageReader::write("teste2.pgm", dims, FImage_out);

//    exit(0);


//    fMatrix decomp = ImageReader::ipointer2fmatrix(Image, dims);

    vector<fMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);

    vector<vector<performance>> performances = VQ::load_performances("./performances/performances.txt");
    printf("");


    unsigned bestCodebooks[NBANDS];
    unsigned imgSize = dims[1]*dims[0];
    intMatrix newBlocks = WaveletHelper::quantize_1(subbands, performances, lb, bestCodebooks, (int)imgSize);

//    for (int i = 0; i < newBlocks.size(); ++i) {
//        printf("\n\n");
//        for (int j = 0; j < newBlocks[i].size(); ++j) {
//            printf("%i|", newBlocks[i][j]);
//        }
//    }

    //******************************************************************************
    //*                                                                            *
    //*                         Codificador Aritimetico                            *
    //*                                                                            *
    //******************************************************************************

    FILE *fout = fopen((out).c_str(), "wb");
    write_header(fout, performances, bestCodebooks, dims, round(avg));
    printf("\nArquivo de saida: %s", (out).c_str());

    if(fout == nullptr)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");

        return;
    }

    start_outputing_bits();
    start_encoding(32);
    double sent_idx = 0;


    for (int subband = 0; subband < NBANDS; ++subband) {
        performance per = performances[subband][bestCodebooks[subband]];
        int freq_size = (int)per.codebook_size + 1 + 1;
        int *freq = (int *)calloc (freq_size,  sizeof (int));
        int *cum_freq = (int *)calloc (freq_size,  sizeof (int));
//        int *cum_freq = VQ::load_model(subband, (int)per.codebook_idx);


//        int max_bits = ceil(log2(freq_size));

//        printf("\n%i Maxbits: %i", freq_size, max_bits);
        if (testandoDecoder){
            printf("\nBlocos: %i", newBlocks[subband].size());
            for (int idx : newBlocks[subband]) {
                int symbol = idx+1;
                putc(symbol, fout);
            }
        }else{
            printf("\nCodificando subbanda: %i, fSize: %i, cbIdx: %i\n", subband, freq_size, bestCodebooks[subband]);
            start_model(freq, cum_freq, freq_size, subband);


            int counter = 0;

            for (int idx : newBlocks[subband]) {
                sent_idx += log2(idx+1);
                int symbol = idx+1;
//                if(subband < 20){
//                    printf(" %i", symbol);
//                }
                encode_symbol(symbol, cum_freq, fout);
                update_model(freq, cum_freq, freq_size, symbol);
//                if(subband == 0){
//                    printf("\n");
//                    for (int j = 0; j < freq_size; ++j) {
//                        printf(" %i", cum_freq[j]);
//                    }
//                    printf("\n");
//                }
                counter += 1;
//                if (counter > 10) break;
            }



        }
    }
    done_encoding(fout);
    done_outputing_bits(fout);

    fseek(fout, 0, SEEK_END);
    long file_size_out = ftell(fout);
    printf("\n\n Taxa de compressao: %.2fx", (float) file_size/ (float) file_size_out);
//    printf("\n\n Taxa de compressao sem cod: %.2fx", (float) sent_idx/ (float) dims[0]/(float) dims[1]);
    fclose(fout);
}

void EncoderWrapper::write_header(FILE *fout, const vector<vector<performance>> &performances, unsigned *bestCodebooks, const unsigned *dims, int avg) {


//    putc(avg, fout);

    int buffer = avg;
    while(buffer > 0){
        if (buffer >= 255){
            buffer -= 255;
            putc(255, fout);
            if (buffer == 0){
                putc(0, fout);
            }
        }
        else{
            putc(buffer, fout);
            buffer = 0;
        }
    }


    for (int i = 0; i < 2; ++i) {
        buffer = dims[i];
        while(buffer > 0){
            if (buffer >= 255){
                buffer -= 255;
                putc(255, fout);
                if (buffer == 0){
                    putc(0, fout);
                }
            }
            else{
                putc(buffer, fout);
                buffer = 0;
            }
        }
    }

    for (int i = 0; i < NBANDS; ++i) {
        putc((int)performances[i][bestCodebooks[i]].codebook_idx, fout);
//        printf("\nCBHEADER: %i", performances[i][bestCodebooks[i]].codebook_idx);
    }
}

void EncoderWrapper::decode(const string &filename, const string& out) {
    FILE *fin = fopen((filename).c_str(), "rb");
    printf("\nArquivo de saida: %s", (filename).c_str());

    if(fin == nullptr)
    {
        printf("Nao foi possivel abrir o arquivo de entrada!");

        return;
    }

    vector<int> header;
    unsigned dims[3] = {0, 0, 255};

    int buffer = 0;
    while(true){
        int ch = getc(fin);
        buffer += ch;
        if (ch < 255){
            break;
        }
    }
    int avg = buffer;

    for (unsigned i = 0; i < 2; i++) {
        buffer = 0;
        while(true){
            int ch = getc(fin);
            buffer += ch;
            if (ch < 255){
                break;
            }
        }
        dims[i] = buffer;
    }

    for (int i = 0; i < NBANDS; ++i) {
        int ch = getc(fin);
        header.push_back(ch);
    }

    vector<fMatrix> selected_codebooks;
    vector<codebookInfo> selected_infos;
    intMatrix allBlocksIdx;

    //******************************************************************************
    //*                                                                            *
    //*                         Decodificador Aritimetico                          *
    //*                                                                            *
    //******************************************************************************
    start_inputing_bits();
    start_decoding(fin, 16);

    for (int subband = 0; subband < NBANDS; ++subband) {
        printf("\nCarregando codebooks: %i/%i", subband+1, NBANDS);
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(subband) + ".txt");
        int cbIdx = header[subband];
        selected_codebooks.push_back(codebook_list[cbIdx]);
        codebookInfo cbInfo = get_cb_info(cbIdx, dims, subband);
        vector<int> subbandBlocksIdx;

        if(testandoDecoder) {
            for (int i = 0; i < cbInfo.blocks; ++i) {
                int ch = getc(fin);
                subbandBlocksIdx.push_back(ch - 1);
            }
        }
        else{
            int freq_size = (int)cbInfo.cbSize+1+1;
            int *freq = (int *)calloc (freq_size,  sizeof (int));
            int *cum_freq = (int *)calloc (freq_size,  sizeof (int));

//            int max_bits = ceil(log2(freq_size));

            start_model(freq, cum_freq, freq_size, subband);
//            int *cum_freq = VQ::load_model(subband, cbIdx);
//            printf("\n");
//            for (int j = 0; j < freq_size; ++j) {
//                printf(" %i", cum_freq[j]);
//            }
//            printf("\n");

            printf("\nDecodificando subbanda: %i, fSize: %i, cbIdx: %i\n", subband, freq_size, cbIdx);

            for (int i = 0; i < cbInfo.blocks; i++) {
//            for (int i = 0; i < 11; i++) {
                int symbol = decode_symbol(cum_freq, fin);
//                if(subband < 20){
//                    printf(" %i", symbol);
//                }
                subbandBlocksIdx.push_back(symbol - 1);
                update_model(freq, cum_freq, freq_size, symbol);
//                printf("\n");
//                for (int j = 0; j < freq_size; ++j) {
//                    printf(" %i", cum_freq[j]);
//                }
//                printf("\n");
            }

        }
        selected_infos.push_back(cbInfo);
        allBlocksIdx.push_back(subbandBlocksIdx);
    }


    //******************************************************************************
    //*                                                                            *
    //*                  Blocos -> subbandas -> sintese                            *
    //*                                                                            *
    //******************************************************************************

    fMatrix deecodedImage = VQ::fill_image(allBlocksIdx, selected_codebooks, selected_infos, dims);

    double *pSIMG[YIMG];


    for (int y = 0; y < dims[0]; y++) /* luminance */
    {
        pSIMG[y] = (double *) malloc(dims[1] * (sizeof(double)));
        for (int x = 0; x < dims[1]; x++) {
            *(pSIMG[y] + x) = (double) deecodedImage[y][x];
        }
    }

//    for (int i = 0; i < (int)dims[0]/8; ++i) {
//        for (int j = 0; j < (int)dims[1]/8; ++j) {
//            pSIMG[i][j] += round(avg);
//        }
//    }

    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
    printf("\nSubband synthesis ...");
    only_synt(Image_out, pSIMG, (int)dims[1], (int)dims[0]);
    ImageReader::add_avg(Image_out, dims, avg);



    fMatrix FImage_out = ImageReader::ipointer2fmatrix(Image_out, dims);

//    ImageReader::write((out+"wav.pgm").c_str(), dims, deecodedImage);
    ImageReader::write(out.c_str(), dims, FImage_out);
}

codebookInfo EncoderWrapper::get_cb_info(int codebook_index, const unsigned *dims, int subband) {
    codebookInfo cbInfo;
    int cbIdx = -1;

    for (auto v : bsize_list(subband)) {
        for (unsigned int cb_size: csize_list(subband)) {
            unsigned bSize = v[0] * v[1];
            double R = log2(cb_size + 1) / bSize;
            if (R <= MAXR) {
                cbIdx += 1;
                if (cbIdx == codebook_index){
                    cbInfo.cbSize = cb_size+1;
                    cbInfo.blockH = v[0];
                    cbInfo.blockW = v[1];

                    int level = (int)floor((subband-1)/3) + 1;
                    if (subband == 0){
                        level = (NBANDS-1)/3;
                    }
                    unsigned subbandSize = dims[0]*dims[1]/(unsigned)pow(2, 2*level);
                    cbInfo.blocks = subbandSize/bSize;
//                    printf("\nBlocks: %i/%i/%i", subbandSize, level, cbInfo.blocks);

                    return cbInfo;
                }
            }
        }
    }

    return cbInfo;
}
