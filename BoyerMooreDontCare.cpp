#include "BoyerMooreDontCare.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "Signature.hpp"

BoyerMooreDontCare::BoyerMooreDontCare(const Signature& signature)
: _signature(signature)
, _arena(BadCharTableDontCare::arenaSpaceNeeded(_signature) + GoodSuffixTableDontCare::arenaSpaceNeeded(_signature))
, _arenaAllocator(_arena)
// TODO: arena-allocate
, _matchOrder(MatchOrder::compute(signature))
, _badCharTableDontCare(_arenaAllocator, _signature)
, _goodSuffixTableDontCare(_arenaAllocator, _signature, _matchOrder)
{
}

auto BoyerMooreDontCare::search(std::basic_string_view<uint8_t> haystack) const -> std::vector<uint8_t const*> {
	auto& pattern = _signature.pattern();
	auto& mask = _signature.mask();
	size_t lastMatchCandidatePos = haystack.size() - pattern.size();

	std::vector<uint8_t const*> matches;

	size_t pos = 0;
	while(pos <= lastMatchCandidatePos) {
		int nextShift;

		bool match = true;
		for (int matchOrderPos = 0; matchOrderPos < _matchOrder.size(); ++matchOrderPos) {
			int pattern_idx = _matchOrder[matchOrderPos];
			uint8_t hay_letter = haystack[pos + pattern_idx];
			bool dont_care = mask[pattern_idx] == SignatureMask::DONT_CARE;
			if (dont_care || hay_letter == pattern[pattern_idx]) {
				// match, check next letter in pattern
				continue;
			}
			else {
				match = false;

				int badCharShift = _badCharTableDontCare.computeShift(pattern_idx, hay_letter);
				int goodSuffixShift = _goodSuffixTableDontCare.computeShift(pattern_idx);

				nextShift = std::max(badCharShift, goodSuffixShift);
				break;
			}
		}

		if (match) {
			matches.push_back(haystack.data() + pos);
			nextShift = _goodSuffixTableDontCare.afterMatchShift();
		}

		// shift pattern
		pos += nextShift;
	}

	return matches;
}
