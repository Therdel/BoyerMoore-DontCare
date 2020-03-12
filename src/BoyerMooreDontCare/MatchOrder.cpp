#include <stdexcept>	// std::invalid_argument
#include <algorithm>	// std::max

#include "MatchOrder.hpp"
#include "Signature.hpp"

namespace MatchOrder {
	auto computeDefaultBoyerMoore(const Signature& signature) -> MatchOrder_t {
		auto patternSize = signature.pattern().size();
		MatchOrder_t matchOrder;
		matchOrder.reserve(patternSize);

		int lastPatternIndex = static_cast<int>(patternSize) - 1;
		for (int i = lastPatternIndex; i >= 0; --i) {
			if (signature.mask()[i] == SignatureMask::CARE) {
				matchOrder.push_back(i);
			}
		}
			return matchOrder;
	}

	auto computeMaxDontCareDistance(const Signature& signature) -> MatchOrder_t {
		int patternLength = static_cast<int>(signature.pattern().size());
		MatchOrder_t matchOrder(patternLength);

		// Patt: _aba__asdf
		// Dist: _123__1234
		// Ordr:  632  5410
		// Indx: 0123456789
		// Rslt: 9832761

		struct Info {
			int patternIndex = -1;
			int dontCareDistance = -1;
			int matchOrder = -1;
		};
		std::vector<Info> positions(patternLength);

		// 1. compute dontcareDistance
		int lastDontCarePos = -1;
		for (int i = 0; i < patternLength; ++i) {
			// init patternIndices
			positions[i].patternIndex = i;
			// init dontCareDistances
			if (signature.mask()[i] == SignatureMask::CARE) {
				int dontCareDistance = i - lastDontCarePos;
				positions[i].dontCareDistance = dontCareDistance;
			}
			else {
				lastDontCarePos = i;
			}
		}

/*
// TODO: optimize matching order for contiguousness
		// derive matchOrders
			// using maxDistance... Never go more than one below maxDistance
		for (int currentDistance = maxDist; currentDistance > 0; --currentDistance)
		{
		}
*/
		// 2. derive order
		// sort pattern positions by maximum distance to next DONTCARE pos to the left
		std::sort(positions.begin(), positions.end(), [](Info& lhs, Info& rhs) {
			if (lhs.dontCareDistance != rhs.dontCareDistance)
				return lhs.dontCareDistance > rhs.dontCareDistance;
			else
				return lhs.patternIndex > rhs.patternIndex;
		});

		// 3. write indices in order to matchOrder field
		int lastCaringMatchOrderIndex = -1;
		int amountPositions = static_cast<int>(positions.size());
		for (int i = 0; i < amountPositions; ++i) {
			auto& position = positions[i];
			matchOrder[i] = position.patternIndex;

			if (position.dontCareDistance >= 0) {
				lastCaringMatchOrderIndex = i;
			}
			else {
				break;
			}
		}

		// truncate length to only contain indices to CARE-fields
		matchOrder.resize(lastCaringMatchOrderIndex + 1);

		return matchOrder;
	}

	auto compute(const Signature& signature, Strategy strategy) -> MatchOrder_t {

		switch (strategy) {
		case DefaultBoyerMoore:
			return computeDefaultBoyerMoore(signature);
			// case SortedChunkLinear:
			//	break;
		case SortedMaxDontCareDistance:
			return computeMaxDontCareDistance(signature);
			break;
		default:
			throw std::invalid_argument("Strategy unimplemented");
		}
	}
};