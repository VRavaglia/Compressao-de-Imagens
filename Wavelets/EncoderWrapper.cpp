//
// Created by Victor on 03/06/2022.
//

#include "EncoderWrapper.h"
#include "ImageReader.h"
#include "WaveletHelper.h"
#include "VQ.h"
extern "C"
{
#include "subdefs2.h"
#include "sub.h"
#include "bit_output.h"
#include "arithmetic_encode.h"
#include "histogram.h"
}


void EncoderWrapper::encode(const string& in, const string& out) {
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


    return;


//    fMatrix decomp = ImageReader::ipointer2fmatrix(Image, dims);

    vector<intMatrix> subbands = WaveletHelper::splitSubbands(pSIMG, (int)dims[1], (int)dims[0], NSTAGES);

    vector<vector<performance>> performances = VQ::evaluate_codebooks(subbands);

    unsigned bestCodebooks[NBANDS];
    intMatrix newBlocks = WaveletHelper::quantize(subbands, performances, LAMBDA, bestCodebooks);

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
    write_header(fout, performances, bestCodebooks, dims);
    printf("\nArquivo de saida: %s", (out).c_str());

    if(fout == nullptr)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");

        return;
    }

#define testandoDecoder true

    for (int subbanda = 0; subbanda < NBANDS; ++subbanda) {
        performance per = performances[subbanda][bestCodebooks[subbanda]];
        int freq_size = (int)per.codebook_size + 1;
        int *freq = (int *)calloc (freq_size,  sizeof (int));
        int *cum_freq = (int *)calloc (freq_size,  sizeof (int));

        int max_bits = ceil(log2(freq_size));

//        printf("\n%i Maxbits: %i", freq_size, max_bits);
        if (testandoDecoder){
            printf("\nBlocos: %i", newBlocks[subbanda].size());
            for (int idx : newBlocks[subbanda]) {
                int symbol = idx+1;
                putc(symbol, fout);		         /* Update the model 	 	 */
            }
        }else{

            start_model(freq, cum_freq, freq_size);
            start_outputing_bits();
            start_encoding(max_bits);

            for (int idx : newBlocks[subbanda]) {
                int symbol = idx+1;
                encode_symbol(symbol, cum_freq, fout);	 /* Encode that symbol.	 	 */
                update_model(freq, cum_freq, freq_size, symbol);		         /* Update the model 	 	 */
            }

            done_encoding(fout);
            done_outputing_bits(fout);
        }
    }

    fseek(fout, 0, SEEK_END);
    long file_size_out = ftell(fout);
    printf("\n\n Taxa de compressao: %.2fx", (float) file_size/ (float) file_size_out);
    fclose(fout);
}

void EncoderWrapper::write_header(FILE *fout, const vector<vector<performance>> &performances, unsigned *bestCodebooks, const unsigned *dims) {

    for (int i = 0; i < 2; ++i) {
        int buffer = dims[i];
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

    for (unsigned i = 0; i < 2; i++) {
        int buffer = 0;
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

    for (int subband = 0; subband < NBANDS; ++subband) {
        vector<fMatrix> codebook_list = VQ::load_codebooks("./codebooks/codebooks_" + to_string(subband) + ".txt");
        int cbIdx = header[subband];
        selected_codebooks.push_back(codebook_list[cbIdx]);
        codebookInfo cbInfo = get_cb_info(cbIdx, dims, subband);
        vector<int> subbandBlocksIdx;

        for (int i = 0; i < cbInfo.blocks; ++i) {
            int ch = getc(fin);
            subbandBlocksIdx.push_back(ch-1);
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

    printf("\nSubband synthesis ...");
//    sub4synt(pSIMG, NSTAGES, 1, (int)dims[1], (int)dims[0]);
    sub4synt(pSIMG, NSTAGES, 1, (int)dims[1], (int)dims[0]);



    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);

    printf("\nTranslating restored image...");
    for (int y = 0; y < dims[0]; y++)
    {
        for (int x = 0; x < dims[1]; x++) {
            Image_out[y][x] = mpel(round(pSIMG[y][x]));
        }
    }


    fMatrix FImage_out = ImageReader::ipointer2fmatrix(Image_out, dims);

    ImageReader::write((out+"wav.pgm").c_str(), dims, deecodedImage);
    ImageReader::write(out.c_str(), dims, FImage_out);
}

codebookInfo EncoderWrapper::get_cb_info(int codebook_index, const unsigned *dims, int subband) {
    codebookInfo cbInfo;
    int cbIdx = -1;

    for (auto v : vector_list) {
        for (unsigned int cb_size: cb_size_list) {
            unsigned bSize = v[0] * v[1];
            double R = log2(cb_size + 1) / bSize;
            if (R <= 7) {
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
