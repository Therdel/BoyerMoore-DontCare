#include "GoodSuffixTable.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"

GoodSuffixTable::GoodSuffixTable(ArenaAllocator& allocator, PatternRef pattern)
	: _allocator(allocator)
	, _pattern(pattern)
	, _shiftTable() {
	_preprocessShifts();
}

auto GoodSuffixTable::computeShift(int mismatchPatternIndex) const -> int {
	return _shiftTable.at(mismatchPatternIndex);
}

auto GoodSuffixTable::afterMatchShift() const -> int {
	return _afterMatchShift;
}

auto GoodSuffixTable::arenaSpaceNeeded(PatternRef pattern) -> size_t {
	// TODO: this is pessimistic padding
	using shiftDistanceType = decltype(_shiftTable)::value_type;
	size_t worstCasePadding = alignof(shiftDistanceType) - 1;
	size_t shiftTableBytes = pattern.size() * sizeof(shiftDistanceType);
	return worstCasePadding + shiftTableBytes;
}

auto GoodSuffixTable::_preprocessShifts() -> void {
	// allocate table in arena
	void* storage = _allocator.allocate<int>(_pattern.size());
	if (storage == nullptr) {
		throw std::bad_alloc();
	}
	auto *intStorage = reinterpret_cast<int*>(storage);
	_shiftTable = { intStorage, _pattern.size() };

	// build table
	// 1. write L'(i) values into table
	_writeLhyphenValues(intStorage);

	// 2. write recommended shifts to table
	auto patternLen = _pattern.size();
	auto lastIndex = patternLen - 1;
	for (int mismatchIdx = 0; mismatchIdx < lastIndex; ++mismatchIdx) {
		int lHyphen = intStorage[mismatchIdx + 1];

		int shiftAmount;
		if (lHyphen > 0) {
			// matching suffix exists in pattern
			shiftAmount = lastIndex - lHyphen;
		} else {
			// no matching suffix, check for some prefix matching suffix of already matched suffix
			int matchingPrefixLength = _iHyphen(mismatchIdx + 1);
			if (matchingPrefixLength > 0) {
				// matching prefix exists, shift to align with matching suffix
				int matchingPrefixRightEndIndex = matchingPrefixLength - 1;
				shiftAmount = lastIndex - matchingPrefixRightEndIndex;
			} else {
				// no matching prefix, no alignment possible, shift by whole pattern
				shiftAmount = patternLen;
			}
		}
		intStorage[mismatchIdx] = shiftAmount;
	}
	// 3. recommend shift by 1 when mismatch on first character
	intStorage[_pattern.size() - 1] = 1;

	// 4. calculate shift amount after successful matches
	_afterMatchShift = _computeAfterMatchShift();
}

auto GoodSuffixTable::_N(int j) const -> int {
	int suffixLength{ 0 };
	auto patternLength = _pattern.size();
	for (int i = 0; i < patternLength && j >= i; ++i) {
		int prefixIdx = j - i;
		int patternSuffixIdx = (patternLength - 1) - i;

		if (_pattern[prefixIdx] == _pattern[patternSuffixIdx]) {
			++suffixLength;
		} else {
			break;
		}
	}

	return suffixLength;
}

auto GoodSuffixTable::_writeLhyphenValues(int* buffer) const -> void {
	auto patternLen = _pattern.size();
	std::fill(buffer, buffer + patternLen, 0);

	int lastPatternIndex = patternLen - 1;
	for (int j = 0; j <= lastPatternIndex - 1; ++j) {
		int matchingSuffixLen = _N(j);

		// devise starting index of suffix at the end of the pattern
		int index = lastPatternIndex - matchingSuffixLen + 1;
		// store L'(index)
		// which is the right-end index of the matching substring
		// which matches the suffix [i..n] of the pattern (n is the last elem inclusive]
		buffer[index] = j;
	}
}

auto GoodSuffixTable::_iHyphen(int i) const -> int {
	int suffixPrefixMatchLength{ 0 };

	int substringLength = (_pattern.size() - 1) - i;
	for (int suffixLength = 1; suffixLength <= substringLength; ++suffixLength) {
		// compare 
		bool prefixSuffixEqual = std::equal(_pattern.begin(), _pattern.begin() + suffixLength,
											_pattern.end() - suffixLength);
		if (prefixSuffixEqual) {
			suffixPrefixMatchLength = suffixLength;
		}
	}
	return suffixPrefixMatchLength;
}

auto GoodSuffixTable::_computeAfterMatchShift() const -> int {
	// _iHyphen(0) would just match the whole pattern as its own 
	// prefix/suffix pair and thus always return the patterns length
	return _pattern.size() - _iHyphen(1);
}