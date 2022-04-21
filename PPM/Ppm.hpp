#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "FrequencyTable.hpp"



class Context {

		public:

		SimpleFrequencyTable frequencies;

		std::vector<std::unique_ptr<Context> > subcontexts;


		Context(std::uint32_t symbols, int order);

	};

class Ppm {

	public:


	Ppm(int order, std::uint32_t symLimit, std::uint32_t escapeSym);

	int modelOrder;

	std::uint32_t symbolLimit;
	std::uint32_t escapeSymbol;

	SimpleFrequencyTable ordemn1;

	std::unique_ptr<Context> rootContext;


	void incContext(const std::vector<std::uint32_t> &contexto, std::uint32_t symbol);

};
