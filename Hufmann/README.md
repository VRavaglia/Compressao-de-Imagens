# Image Compression Algorithms

This repository is a collection of four algorithms related to data compression. They were developed as assignments for the course CPE781 - Image Compression. The algorithms are:  
* A Huffman based text encoder/decoder 
* A text encoder/decoder based on Prediction by Partial Match
* An image compressor based on Vector Quantization
* An image compressor based on Vector Quantization, Wavelet decomposition and an Arithmetic encoder.

Each folder of the repository correspond to one of the mentioned algorithms and contains its own README. 

Here is a brief explanation of each algorithm:

## Huffman Encoder

This is a text compression algorithm that aims to allocate an optimum amount of bits to each character based of the frequency  it appears in the text to be compressed. The encoder reads the entire text file and counts the occurrences of each character. Then, this information is used to generate a [Huffman table](https://en.wikipedia.org/wiki/Huffman_coding). The ideia is to represent each character by a set of bits. The compression comes from the fact that the characters that have the highest occurrences are given less bits. If, on average, the number of bits per character is less than 8 (number of bits per character in standard ASCII code), than the compressed text has fewer bits than the original.

## Prediction by Partial Match

This is a text compression algorithm that uses [Arithmetic coding](https://en.wikipedia.org/wiki/Arithmetic_coding) to generate the bits that will represent each character. The arithmetic encoder/decoder uses a table of occurrences, similar to Huffman coding, aiming to assign fewer bits to the most frequent characters. In addition, the algorithm keeps track of the context each character appears, that is, the characters that preceded it. With the context information, the algorithm adjusts the probability of a character appearing in a given context. For example, if the context (last read characters) were "the", there is a high probability that the next character is " " (space). The probability information can be used to allocate fewer (more) bits to the most (least) probable outcomes.

## Vector Quantization

This algorithm uses a variation  of [LBG](https://www.geeksforgeeks.org/linde-buzo-gray-lbg-algorithm/) algorithm to generate codebooks by analyzing a set of training images. In theory, these codebooks can be used to represent the images instead of the value of each pixel. Each codebook is a set of blocks, each blocks a set of pixels. Because the codebooks are known, the encoder can send the index of the block instead of the value of each pixel. If the number of possible indexes of each codebook is less than the number of possible pixel values, compression is achieved. 

## Wavelets

This algorithm is uses all previous algorithms and more to perform image compression. First, the image is decomposed in sub-bands by a [Wavelet Transform](https://en.wikipedia.org/wiki/Wavelet_transform), then, VQ is used in each sub-band. The indexes of each sub-band is encoded with an arithmetic encoder that uses a table of frequencies that was generated during the training, for each sub-band. Splitting the image in sub-bands allows for codebooks and frequency tables better adjusted for each sub-band. 


