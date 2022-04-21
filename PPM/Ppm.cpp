#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include "Ppm.hpp"

using std::uint32_t;
using std::vector;


Context::Context(uint32_t symbols, int order) :
	frequencies(vector<uint32_t>(symbols, 0)) {

	if (order >= 1) {
		for (uint32_t i = 0; i < symbols; i++)
			subcontexts.push_back(std::unique_ptr<Context>(nullptr));
	}
}


Ppm::Ppm(int order, uint32_t symLimit, uint32_t escapeSym) :
		modelOrder(order),
		symbolLimit(symLimit),
		escapeSymbol(escapeSym),
		ordemn1(FlatFrequencyTable(symbolLimit)),
		rootContext(std::unique_ptr<Context>(nullptr))
		{


	if (order >= 0) {
		rootContext.reset(new Context(symbolLimit, order));
		rootContext->frequencies.increment(escapeSymbol);
	}

}


void Ppm::incContext(const vector<uint32_t> &contexto, uint32_t symbol) {

	if (modelOrder == -1)
		return;

	Context *ctx = rootContext.get();
	ctx->frequencies.increment(symbol);

	std::size_t i = 0;
	for (uint32_t sym : contexto) {

		vector<std::unique_ptr<Context> > &subctxs = ctx->subcontexts;

		std::unique_ptr<Context> &subctx = subctxs.at(sym);

		if (subctx.get() == nullptr) {
			subctx.reset(new Context(symbolLimit, i + 1 < static_cast<unsigned int>(modelOrder)));
			subctx->frequencies.increment(escapeSymbol);
		}

		ctx = subctx.get();
		ctx->frequencies.increment(symbol);
		i++;
	}
}

