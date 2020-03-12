#include <stdexcept>

#include "GoodSuffixTableDontCare.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "Signature.hpp"

GoodSuffixTableDontCare::GoodSuffixTableDontCare(ArenaAllocator& allocator,
	const Signature& signature,
	const MatchOrder::MatchOrder_t& matchOrder)
	: _allocator(allocator)
	, _signature(signature)
	, _matchOrder(matchOrder)
	, _shiftTable() {
	_preprocessShifts();
}

auto GoodSuffixTableDontCare::computeShift(int mismatchPatternIndex) const -> int {
	return _shiftTable.at(mismatchPatternIndex);
}

auto GoodSuffixTableDontCare::afterMatchShift() const -> int {
	return _afterMatchShift;
}

auto GoodSuffixTableDontCare::arenaSpaceNeeded(const Signature& signature) -> size_t {
	// TODO: this is pessimistic padding
	using shiftDistanceType = decltype(_shiftTable)::value_type;
	size_t worstCasePadding = alignof(shiftDistanceType) - 1;
	size_t shiftTableBytes = signature.pattern().size() * sizeof(shiftDistanceType);
	return worstCasePadding + shiftTableBytes;
}

auto GoodSuffixTableDontCare::_preprocessShifts() -> void {
	// allocate table in arena
	void* storage = _allocator.allocate<int>(_signature.pattern().size());
	if (storage == nullptr) {
		throw std::bad_alloc();
	}
	auto *intStorage = reinterpret_cast<int*>(storage);
	std::fill(intStorage, intStorage + _signature.pattern().size(), 0);
	_shiftTable = { intStorage, _signature.pattern().size() };

	// build table
	// write recommended shifts to table
	int matchOrderLength = static_cast<int>(_matchOrder.size()), patternLength = static_cast<int>(_signature.pattern().size());
	for (int matchLength = 1; matchLength < matchOrderLength; ++matchLength) {
		for (int shift = 1; shift <= patternLength; ++shift) {
			if (_checkShiftStrong(matchLength, shift)) {
				int mismatchPatternIndex = _matchOrder[matchLength];
				intStorage[mismatchPatternIndex] = shift;
				break;
			}
		}
	}
	// recommend shift by 1 when mismatch on first character
	int mismatchPatternIndex = _matchOrder[0];
	intStorage[mismatchPatternIndex] = 1;

	// calculate shift amount after successful matches
	_afterMatchShift = _computeAfterMatchShift();
}

auto GoodSuffixTableDontCare::_checkShiftStrong(size_t matchLength, int shift) const -> bool {
	auto& pattern = _signature.pattern();

	bool previousPositionsMatch = _checkMatchPositionsAfterShift(matchLength, shift);

	bool mismatchPositionIsDifferent;
	int mismatchPosition = _matchOrder[matchLength];
	int afterShiftMismatchPosition = mismatchPosition - shift;
	if (afterShiftMismatchPosition < 0) {
		mismatchPositionIsDifferent = true;
	}
	else {
		if (_signature.mask()[afterShiftMismatchPosition] == SignatureMask::DONT_CARE) {
			// dont care field in pattern will always match
			mismatchPositionIsDifferent = true;
		}
		else {
			mismatchPositionIsDifferent = pattern[mismatchPosition] != pattern[afterShiftMismatchPosition];
		}
	}

	return previousPositionsMatch && mismatchPositionIsDifferent;
}

auto GoodSuffixTableDontCare::_checkMatchPositionsAfterShift(size_t amountMatchingPositions, int shift) const -> bool {
	bool positionsMatchAfterShift = true;
	auto& pattern = _signature.pattern();
	for (size_t i = 0; i < amountMatchingPositions; ++i) {
		int patternPos = _matchOrder[i];
		int afterShiftPos = patternPos - shift;
		if (afterShiftPos < 0) {
			continue;
		}
		if (_signature.mask()[afterShiftPos] == SignatureMask::DONT_CARE) {
			// dont care field in pattern will always match
			continue;
		}
		positionsMatchAfterShift &= pattern[patternPos] == pattern[afterShiftPos];
		if (!positionsMatchAfterShift) {
			break;
		}
	}
	return positionsMatchAfterShift;
}

auto GoodSuffixTableDontCare::_computeAfterMatchShift() const -> int {
	int patternLength = static_cast<int>(_signature.pattern().size());
	int afterMatchShift = -1;
	for (int shift = 1; shift < patternLength; ++shift) {
		if (_checkMatchPositionsAfterShift(_matchOrder.size(), shift)) {
			afterMatchShift = shift;
		}
	}
	if (afterMatchShift == -1) {
		// no overlapping shift possible, shift by whole pattern
		afterMatchShift = patternLength;
	}
	return afterMatchShift;
}