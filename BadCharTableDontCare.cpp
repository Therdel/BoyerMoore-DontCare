#include <iostream>
#include <iomanip>	// resetiosflags

#include "BadCharTableDontCare.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "Signature.hpp"

BadCharTableDontCare::BadCharTableDontCare(ArenaAllocator& allocator, const Signature& signature)
	: _signature(signature)
	, _indexTable(_buildTableInArena(allocator, signature))
	, _dontCareRightIndexList(_buildDontCareRightIndexList(allocator, signature)) {
}

auto BadCharTableDontCare::printTable() const -> void {
	using namespace std;
	auto &pattern = _signature.pattern();
	auto &mask = _signature.mask();

	cout << std::showbase	// hex numbers with 0x prefix
		 << std::uppercase;	// hex numbers in capital letters

	cout << "Pattern: ";
	for (int i = 0; i < pattern.size(); ++i) {
		if (mask[i] == SignatureMask::DONT_CARE) {
			cout << "?? ";
		}
		else {
			cout << std::hex << std::showbase << static_cast<size_t>(pattern[i]) << ' ';
		}
	}
	cout << endl;

	cout << "Index table: (total size=" << arenaSpaceNeeded(_signature) << " bytes)" << endl;
	// print symbol indices
	for(uint8_t c = 0; c < std::numeric_limits<uint8_t>::max(); ++c) {
		IndexList& list = _indexTable[c];
		if (!list.empty()) {
			cout << std::hex << static_cast<size_t>(c) << ":\t";
			for (auto* cur = list.head(); cur; cur = cur->next()) {
				cout << std::dec << cur->value() << ", ";
			}
			cout << endl;
		};
	}
	// print dontcare indices
	if (!_dontCareRightIndexList.empty()) {
		cout << "??:\t";
		for (auto* cur = _dontCareRightIndexList.head(); cur; cur = cur->next()) {
			cout << std::dec << cur->value() << ", ";
		}
		cout << endl;
	};
	cout << std::resetiosflags(std::ios_base::hex | std::ios_base::dec)
	     << std::noshowbase
		 << std::nouppercase;
}

auto BadCharTableDontCare::computeShift(int mismatchIdx, uint8_t mismatchSymbol) const -> int {
	// find nearest index at which found symbol occurs again in pattern
	auto &indexList = _indexTable[mismatchSymbol];
	int nextOccuringIndex = -1;
	for (IndexList::Node* curNode = indexList.head(); curNode; curNode = curNode->next()) {
		int occurance = curNode->value();
		if (occurance < mismatchIdx) {
			nextOccuringIndex = occurance;
			break;
		} else if (occurance == mismatchIdx) {
			throw std::runtime_error("BadCharTableDontCare: Should not happen - "
									 "mismatch and match at the same time");
		}
	}

	// find nearest dontCare area index
	int nextDontCareIndex = -1;
	for (IndexList::Node* curNode = _dontCareRightIndexList.head(); curNode; curNode = curNode->next()) {
		int dontCare = curNode->value();
		if (dontCare < mismatchIdx) {
			nextDontCareIndex = dontCare;
			break;
		}
		else if (dontCare == mismatchIdx) {
			throw std::runtime_error("BadCharTableDontCare: Should not happen - "
                                     "mismatch on a dontCare position");
		}
	}

	int shiftAmount;
	// choose candidate based on smallest shift, so no possible re-match is missed
	int nextMatchingIndex = std::max(nextOccuringIndex, nextDontCareIndex);
	if (nextMatchingIndex != -1) {
		// recommend shift to next match
		shiftAmount = mismatchIdx - nextMatchingIndex;
	}
	else {
		// recommend maximum shift past the pattern beginning
		shiftAmount = mismatchIdx + 1;
	}
	return shiftAmount;
}

auto BadCharTableDontCare::arenaSpaceNeeded(const Signature &signature) -> size_t {
	size_t amountDontCareIndices = 0;
	_iterateArrivableDontCareAreaRightIndices(signature, [&](int) { ++amountDontCareIndices; });

	// TODO: pessimistic padding
	// TODO: build arena allocation planner
	size_t worstCaseIndexTablePadding = alignof(IndexTable) - 1;
	size_t indexTableSize = sizeof(IndexTable);
	size_t worstCaseListNodePadding = alignof(IndexList::Node) - 1;
	size_t patternListNodesSize = sizeof(IndexList::Node) * signature.pattern().size();

	size_t worstCaseIndexListPadding = alignof(IndexList) - 1;
	size_t indexListSize = sizeof(IndexList);
	size_t dontCareListNodesSize = sizeof(IndexList::Node) * amountDontCareIndices;

	return worstCaseIndexTablePadding +
		indexTableSize +
		worstCaseListNodePadding +
		patternListNodesSize +

		worstCaseIndexListPadding +
		indexListSize +
		worstCaseListNodePadding +
		dontCareListNodesSize;
}

auto BadCharTableDontCare::_iterateArrivableDontCareAreaRightIndices(const Signature &signature, std::function<void(int)> handler) -> void {
	auto& mask = signature.mask();
	int lastMaskIndex = static_cast<int>(mask.size()) - 1;

	// skip out index of possible dontcare-suffix, as it can't be shifted towards
	bool lastPosWasDontCare = true;
	for (int i = lastMaskIndex; i >= 0; --i) {
		bool isDontCare = mask[i] == SignatureMask::DONT_CARE;
		bool dontCareAreaBegun = isDontCare && !lastPosWasDontCare;
		if (dontCareAreaBegun) {
			handler(i);
		}
		lastPosWasDontCare = isDontCare;
	}
}

auto BadCharTableDontCare::_buildTableInArena(ArenaAllocator& allocator, const Signature &signature) -> IndexTable& {
	IndexTable* indexTable = allocator.construct<IndexTable>();
	if (indexTable == nullptr) {
		// TODO: bad alloc error msg
		throw std::bad_alloc();
	}
	// initialize array
	*indexTable = {};

	auto& pattern = signature.pattern();
	auto& mask = signature.mask();
	int lastPatternIndex = static_cast<int>(pattern.size() - 1);
	for (int pattern_idx = lastPatternIndex; pattern_idx >= 0; --pattern_idx) {
		if (pattern_idx == lastPatternIndex) {
			// don't put the patterns last symbol into the table,
			// as it won't be shifted to
			continue;
		}
		else if (mask[pattern_idx] == SignatureMask::DONT_CARE) {
			// indices of dontCare areas aren't recorded in this table
			continue;
		}
		uint8_t curSymbol = pattern[pattern_idx];

		auto& indexList = (*indexTable)[curSymbol];
		auto* nextIndex = allocator.construct<IndexList::Node>(pattern_idx);
		if (nextIndex == nullptr) {
			// TODO: bad alloc error msg
			throw std::bad_alloc();
		}
		indexList.append(nextIndex);
	}

	return *indexTable;
}

auto BadCharTableDontCare::_buildDontCareRightIndexList(ArenaAllocator& allocator, const Signature& signature) -> IndexList& {
	IndexList* dontCareRightIndexList = allocator.construct<IndexList>();
	if (dontCareRightIndexList == nullptr) {
		// TODO: bad alloc error msg
		throw std::bad_alloc();
	}

	_iterateArrivableDontCareAreaRightIndices(signature, [&](int dontCareIndex) {
		auto* dontCareIndexNode = allocator.construct<IndexList::Node>(dontCareIndex);
		if (dontCareIndexNode == nullptr) {
			// TODO: bad alloc error msg
			throw std::bad_alloc();
		}
		dontCareRightIndexList->append(dontCareIndexNode);
	});

	return *dontCareRightIndexList;
}