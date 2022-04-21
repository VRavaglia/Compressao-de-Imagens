#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include "ArithmeticCoder.hpp"
#include "BitIoStream.hpp"
#include "Ppm.hpp"

using namespace std;

static uint32_t decodeSymbol(ArithmeticDecoder &dec, Ppm &model, const vector<uint32_t> &contexto);


int main(int argc, char *argv[]) {

	if (argc != 4) {
		cerr << "Modo de Uso " << argv[0] << " Arquivo a Decodificar, Arquivo Decodificado, Ordem do Contexto" << endl;
		return 1;
	}
	const char *inputFile  = argv[1];
	const char *outputFile = argv[2];
	static int model_order = stoi(argv[3]);

	ifstream in(inputFile, ios::binary);
	ofstream out(outputFile, ios::binary);
	BitInputStream bin(in);

	ArithmeticDecoder dec(32, bin);
	Ppm model(model_order, 257, 256);

	vector<uint32_t> contexto;

	while (true) {

		uint32_t symbol = decodeSymbol(dec, model, contexto);
		if (symbol == 256)
			break;

		int b = static_cast<int>(symbol);

		if (numeric_limits<char>::is_signed)
			b -= (b >> 7) << 8;

		out.put(static_cast<char>(b));
		model.incContext(contexto, symbol);

		if (model.modelOrder >= 1) {
			if (contexto.size() >= static_cast<unsigned int>(model.modelOrder))
				contexto.erase(contexto.end() - 1);
			contexto.insert(contexto.begin(), symbol);
		}
	}

	return 0;
}


uint32_t decodeSymbol(ArithmeticDecoder &dec, Ppm &model, const vector<uint32_t> &contexto) {

	vector<int64_t> excluded(257,1);
	vector<uint32_t> excluded_u(257,0);

	for (int order = static_cast<int>(contexto.size()); order >= 0; order--) {
		Context *ctx = model.rootContext.get();
		int skip = 0;

		for (int i = 0; i < order; i++) {
			ctx = ctx->subcontexts.at(contexto.at(i)).get();
			if (ctx == nullptr){
				skip = 1;
				break;
			}
		}

		if (skip == 0){

			for(size_t i=0; i<excluded.size(); i++){
					if(excluded.at(i)==-1){
						excluded_u.at(i) = 0;
					}
					else{
						excluded_u.at(i) = ctx->frequencies.get(i);
					}
				}

			SimpleFrequencyTable exclusionlist(excluded_u);

			uint32_t symbol = dec.read(exclusionlist);

			if (symbol < 256){

				return symbol;

			}
			else{

				for(size_t i=0; i<excluded.size()-1; i++){
					if(ctx->frequencies.get(i)>0){
						excluded.at(i) = -1;
					}
				}
			}
		}
	}

	/*Contexto ordem -1 */
	return dec.read(model.ordemn1);
}