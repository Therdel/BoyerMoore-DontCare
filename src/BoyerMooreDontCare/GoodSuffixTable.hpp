#pragma once
#include <string>
#include <cstdint>

class ArenaAllocator;

class GoodSuffixTable {
public:
	using PatternRef = std::basic_string_view<uint8_t>;
	GoodSuffixTable(ArenaAllocator& allocator, PatternRef pattern);

	auto computeShift(int mismatchPatternIndex) const -> int;

	// amount to safely shift pattern to the right after a match occured
	auto afterMatchShift() const -> int;

	static auto arenaSpaceNeeded(PatternRef pattern)->size_t;

private:
	ArenaAllocator& _allocator;
	PatternRef _pattern;
	std::basic_string_view<int> _shiftTable;
	int _afterMatchShift;

	auto _preprocessShifts() -> void;

	// length of longest common suffix of prefix P[0..j] and full P
	// where P is the Pattern
	auto _N(int j) const -> int;

	// fills buffer intStorage with respective L'(i) values.
	// stores L'(i) values at respective index i.
	// 
	// L'(i) gives the right end-position of the right-most copy of P[i..n]
	// that is not a suffix of P, with the stronger, added condition that
	// its preceding character is unequal to P(i-1).
	auto _writeLhyphenValues(int *buffer) const -> void;

	// length of largest suffix of P[i..n] that's also a prefix of P
	auto _iHyphen(int i) const -> int;


	// This calculates the longest suffix/prefix pair that's not the pattern itself
	// and calculates the shift distance needed to align it.
	//
	// when a match occurs, the biggest pattern overlap with itself (prefix/suffix pair)
	// is found and the distance shift hereby returned
	auto _computeAfterMatchShift() const -> int;
};