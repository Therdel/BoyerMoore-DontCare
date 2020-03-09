#include "BoyerMoore.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"

BoyerMoore::BoyerMoore(PatternRef pattern)
: _pattern(pattern)
, _arena(BadCharTable::arenaSpaceNeeded(pattern) + GoodSuffixTable::arenaSpaceNeeded(pattern))
, _arenaAllocator(_arena)
, _badCharTable(_arenaAllocator, pattern)
, _goodSuffixTable(_arenaAllocator, pattern) {
}

auto BoyerMoore::search(std::basic_string_view<uint8_t> haystack)->std::vector<uint8_t const*> {
	size_t lastMatchCandidatePos = haystack.size() - _pattern.size();
	int lastPatternOffset = static_cast<int>(_pattern.size() - 1);

	std::vector<uint8_t const*> matches;

	size_t pos = 0;
	while(pos <= lastMatchCandidatePos) {
		int nextShift;

		bool match = true;
		for (int pattern_idx = lastPatternOffset;
			pattern_idx >= 0;
			--pattern_idx) {
			uint8_t hay_letter = haystack[pos + pattern_idx];
			if (hay_letter == _pattern[pattern_idx]) {
				// match, check next letter in pattern
				continue;
			}
			else {
				match = false;

				int matchLength = lastPatternOffset - pattern_idx;
				int badCharShift = _badCharTable.computeShift(matchLength, hay_letter);
				int goodSuffixShift = _goodSuffixTable.computeShift(pattern_idx);

				nextShift = std::max(badCharShift, goodSuffixShift);
				break;
			}
		}

		if (match) {
			matches.push_back(haystack.data() + pos);
			nextShift = _goodSuffixTable.afterMatchShift();
		}

		// shift pattern
		pos += nextShift;
	}

	return matches;
}
