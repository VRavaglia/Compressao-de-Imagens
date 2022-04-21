#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "ArithmeticCoder.hpp"
#include "BitIoStream.hpp"
#include "Ppm.hpp"

using namespace std;


static void encodeSymbol(Ppm &model, const vector<uint32_t> &contexto, uint32_t symbol, ArithmeticEncoder &enc);


int main(int argc, char *argv[]) {
	if (argc != 4) {
		cerr << "Modo de Uso " << argv[0] << " Arquivo a Codificar, Arquivo Codificado, Ordem do Contexto" << endl;
		return 1;
	}
	const char *inputFile  = argv[1];
	const char *outputFile = argv[2];
	static int model_order = stoi(argv[3]);

	ifstream in(inputFile, ios::binary);
	ofstream out(outputFile, ios::binary);

	BitOutputStream bout(out);

	ArithmeticEncoder enc(32, bout);
	Ppm model(model_order, 257, 256);

	vector<uint32_t> contexto;

	while (true) {

		int symbol = in.get();

		if (symbol == EOF)
			break;

		uint32_t sym = static_cast<uint32_t>(symbol);

		encodeSymbol(model, contexto, sym, enc);
		model.incContext(contexto, sym);

		if (model.modelOrder >= 1) {
			/*Atualizar contexto guardando chars anteriores */
			if (contexto.size() >= static_cast<unsigned int>(model.modelOrder))
				contexto.erase(contexto.end() - 1);
			contexto.insert(contexto.begin(), sym);
		}
	}

	encodeSymbol(model, contexto, 256, enc);  /* EOF no caso do contexto -1 e Escape para os outros */
	enc.finish();
	bout.finish();

	return 0;
}

void encodeSymbol(Ppm &model, const vector<uint32_t> &contexto, uint32_t symbol, ArithmeticEncoder &enc) {

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

			if (symbol != 256 && ctx->frequencies.get(symbol) > 0) {

				enc.write(exclusionlist, symbol);
				return;

			}

		/*Escape e diminuir ordem do contexto */
			enc.write(exclusionlist, 256);

			for(size_t i=0; i<excluded.size()-1; i++){
				if(ctx->frequencies.get(i)>0){
					excluded.at(i) = -1;
				}
			}

		}
	}

	/*Senao ordem -1*/
	enc.write(model.ordemn1, symbol);
}

