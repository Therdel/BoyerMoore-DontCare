#include "BoyerMoore.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"

BoyerMoore::BoyerMoore(PatternRef pattern)
: _pattern(pattern)
, _arena(BadCharTable::arenaSpaceNeeded(pattern) + GoodSuffixTable::arenaSpaceNeeded(pattern))
, _arenaAllocator(_arena)
, _badCharTable(_arenaAllocator, pattern)
, _goodSuffixTable(_arenaAllocator, pattern) {
}

auto BoyerMoore::operator()(std::basic_string_view<uint8_t> haystack) const -> std::pair<const uint8_t*, const uint8_t*> {
	/*
	If the pattern ([pat_first, pat_last)) is empty, returns make_pair(first, first).
	Otherwise, returns a pair of iterators to the first and one past last positions
	in [first, last) where a subsequence that compares equal to [pat_first, pat_last)
	as defined by pred is located, or make_pair(last, last) otherwise.
	*/

	if (_pattern.empty()) {
		return { haystack.data(), haystack.data() };
	}
	size_t lastMatchCandidatePos = haystack.size() - _pattern.size();
	int lastPatternOffset = static_cast<int>(_pattern.size() - 1);

	size_t pos = 0;
	while (pos <= lastMatchCandidatePos) {
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
			const uint8_t* match_first = haystack.data() + pos;
			const uint8_t* match_last = match_first + _pattern.size();
			return { match_first, match_last };
		}

		// shift pattern
		pos += nextShift;
	}

	// no match found
    const uint8_t* haystack_last = haystack.data() + haystack.size();
    return { haystack_last, haystack_last };
}

auto BoyerMoore::search(std::basic_string_view<uint8_t> haystack)->std::vector<uint8_t const*> {
	size_t lastMatchCandidatePos = haystack.size() - _pattern.size();
	int lastPatternOffset = static_cast<int>(_pattern.size() - 1);

	std::vector<uint8_t const*> matches;

	size_t pos = 0;
	while(pos <= lastMatchCandidatePos) {
		auto remaining = haystack.substr(pos);
		auto [match_first, match_last] = operator()(remaining);
		
		bool match_found = match_first != match_last;
		if (match_found) {
			matches.push_back(match_first);

			// shift pattern
			pos = match_first - haystack.data();
			pos += _goodSuffixTable.afterMatchShift();
		}
		else {
			break;
		}
	}

	return matches;
}
