#pragma once
#include <string>

#include "MatchOrder.hpp"

class ArenaAllocator;
class Signature;

class GoodSuffixTableDontCare {
public:
	GoodSuffixTableDontCare(ArenaAllocator& allocator,
							const Signature &signature,
							const MatchOrder::MatchOrder_t &matchOrder);

	auto computeShift(int mismatchPatternIndex) const -> int;

	// amount to safely shift pattern to the right after a match occured
	auto afterMatchShift() const -> int;

	static auto arenaSpaceNeeded(const Signature &signature)->size_t;

private:
	ArenaAllocator& _allocator;
	const Signature& _signature;
	const MatchOrder::MatchOrder_t& _matchOrder;
	std::basic_string_view<int> _shiftTable;
	int _afterMatchShift;

	auto _preprocessShifts() -> void;

	// checks if a shift will result in a match
	// this considers all matched characters and the mismatch position
	// latter is ensured to be different to the pattern pos before the shift
	// to rule out instant mismatches after a shift (~strong good suffix rule)
	auto _checkShiftStrong(size_t matchLength, int shift) const -> bool;

	auto _checkMatchPositionsAfterShift(size_t matchingPositions, int shift) const -> bool;

	// This calculates the longest suffix/prefix pair that's not the pattern itself
	// and calculates the shift distance needed to align it.
	//
	// when a match occurs, the biggest pattern overlap with itself (prefix/suffix pair)
	// is found and the distance shift hereby returned
	auto _computeAfterMatchShift() const -> int;
};