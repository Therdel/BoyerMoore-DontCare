#pragma once
#include <vector>

#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "BadCharTableDontCare.hpp"
#include "GoodSuffixTableDontCare.hpp"
#include "MatchOrder.hpp"

class Signature;

class BoyerMooreDontCare {
public:
	explicit BoyerMooreDontCare(const Signature &signature);

	auto search(std::basic_string_view<uint8_t> haystack)->std::vector<uint8_t const*>;
private:
	const Signature &_signature;
	HeapArena _arena;
	ArenaAllocator _arenaAllocator;
	// TODO: Arena-allocate
	MatchOrder::MatchOrder_t _matchOrder;
	BadCharTableDontCare _badCharTableDontCare;
	GoodSuffixTableDontCare _goodSuffixTableDontCare;
};