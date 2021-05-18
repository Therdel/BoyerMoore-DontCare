#pragma once
#include <vector>

#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "BadCharTable.hpp"
#include "GoodSuffixTable.hpp"

class BoyerMoore {
public:
	using PatternRef = std::basic_string_view<uint8_t>;
	explicit BoyerMoore(PatternRef pattern);

	// STL compatibility with std::search
	// If the pattern ([pat_first, pat_last)) is empty, returns make_pair(first, first).
	//
	// Otherwise, returns a pair of iterators to the first and one past last positions
	// in[first, last) where a subsequence that compares equal to[pat_first, pat_last)
	// as defined by pred is located, or make_pair(last, last) otherwise.
	template< class RandomIt2 >
	auto operator()(RandomIt2 first, RandomIt2 last) const -> std::pair<RandomIt2, RandomIt2>;
	auto operator()(std::basic_string_view<uint8_t> haystack) const -> std::pair<const uint8_t*, const uint8_t*>;

	auto search(std::basic_string_view<uint8_t> haystack) -> std::vector<uint8_t const*>;
private:
	PatternRef _pattern;
	HeapArena _arena;
	ArenaAllocator _arenaAllocator;
	BadCharTable _badCharTable;
	GoodSuffixTable _goodSuffixTable;
};

#include "BoyerMoore.ipp"