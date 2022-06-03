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
    int **Image_out = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);
    int **Image = ImageReader::allocIntMatrix((int)dims[0], (int)dims[1]);

    sub(Image_orig, Image_out, Image, (int)dims[1], (int)dims[0]);

    vector<intMatrix> subbands = WaveletHelper::splitSubbands(Image, (int)dims[1], (int)dims[0], NSTAGES);

    free(Image_orig);
    free(Image_out);
    free(Image);

    vector<vector<performance>> performances = VQ::evaluate_codebooks(subbands);

    unsigned bestCodebooks[NBANDS];
    intMatrix newBlocks = WaveletHelper::quantize(subbands, performances, LAMBDA, bestCodebooks);

    //******************************************************************************
    //*                                                                            *
    //*                         Codificador Aritimetico                            *
    //*                                                                            *
    //******************************************************************************

    FILE *fout = fopen((out).c_str(), "wb");
    write_header(fout, performances, bestCodebooks);
    printf("\nArquivo de saida: %s", (out).c_str());

    if(fout == nullptr)
    {
        printf("Nao foi possivel abrir o arquivo de saida!");

        return;
    }

    for (int subbanda = 0; subbanda < NBANDS; ++subbanda) {
        performance per = performances[subbanda][bestCodebooks[subbanda]];
        int freq_size = (int)per.codebook_size + 1;
        int *freq = (int *)calloc (freq_size,  sizeof (int));
        int *cum_freq = (int *)calloc (freq_size,  sizeof (int));

        int max_bits = ceil(log2(freq_size));

//        printf("\n%i Maxbits: %i", freq_size, max_bits);

        start_model(freq, cum_freq, freq_size);
        start_outputing_bits();
        start_encoding(max_bits);

        for (int idx : newBlocks[subbanda]) {
            int symbol = idx+1;

//            if (symbol == 9){
//                printf("\n");
//                for (int i = 0; i < freq_size; ++i) {
//                    printf("0%i|", freq[i]);
//                }
//                printf("\n");
//                for (int i = 0; i < freq_size; ++i) {
//                    printf("%i|", cum_freq[i]);
//                }
//            }
            encode_symbol(symbol, cum_freq, fout);	 /* Encode that symbol.	 	 */
            update_model(freq, cum_freq, freq_size, symbol);		         /* Update the model 	 	 */
        }

        done_encoding(fout);
        done_outputing_bits(fout);
    }

    fseek(fout, 0, SEEK_END);
    long file_size_out = ftell(fout);
    printf("\n\n Taxa de compressao: %.2fx", (float) file_size/ (float) file_size_out);
    fclose(fout);
}

void EncoderWrapper::write_header(FILE *fout, const vector<vector<performance>> &performances, unsigned *bestCodebooks) {
    for (int i = 0; i < NBANDS; ++i) {
        putc((int)performances[i][bestCodebooks[i]].codebook_idx, fout);
    }
}
