#pragma once
#include <vector>

#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "BadCharTable.hpp"
#include "GoodSuffixTable.hpp"

class BoyerMoore {
public:
	using PatternRef = std::basic_string_view<uint8_t>;
	explicit BoyerMoore(PatternRef pattern);

	auto search(std::basic_string_view<uint8_t> haystack)->std::vector<uint8_t const*>;
private:
	PatternRef _pattern;
	HeapArena _arena;
	ArenaAllocator _arenaAllocator;
	BadCharTable _badCharTable;
	GoodSuffixTable _goodSuffixTable;
};