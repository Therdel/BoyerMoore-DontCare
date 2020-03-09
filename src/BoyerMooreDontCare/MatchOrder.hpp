#pragma once
#include <vector>

class Signature;

namespace MatchOrder {
	using MatchOrder_t = std::vector<int>;
	enum Strategy {
		DefaultBoyerMoore,
		//SortedChunkLinear,
		SortedMaxDontCareDistance
	};

	auto compute(const Signature &signature, Strategy strategy = SortedMaxDontCareDistance) -> MatchOrder_t;
};