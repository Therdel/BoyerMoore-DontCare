#include <optional>
#include <numeric_limits>

#include "BadCharTable.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"

BadCharTable::BadCharTable(ArenaAllocator& allocator, PatternRef pattern)
	: _pattern(pattern)
	, _indexTable(buildTableInArena(allocator, pattern)) {
}

#include <iostream>	// printTable
auto BadCharTable::printTable() const -> void {
	using namespace std;
	cout << "Pattern: ";
	for (uint8_t c : _pattern) {
		cout << static_cast<char>(c);
	}
	cout << endl;

	cout << "Index table: (total size=" << arenaSpaceNeeded(_pattern) << " bytes)" << endl;
	for(uint8_t c = 0; c < std::numeric_limits<uint8_t>::max(); ++c) {
		IndexList& list = _indexTable[c];
		if (!list.empty()) {
			cout << static_cast<char>(c) << ": ";
			for (auto* cur = list.head(); cur; cur = cur->next()) {
				cout << cur->value() << ", ";
			}
			cout << endl;
		};
	}
}

auto BadCharTable::computeShift(int matchLength, uint8_t mismatchSymbol) const -> int {
	auto &indexList = _indexTable[mismatchSymbol];

	// find index at which found symbol occurs again in pattern
	int nextIndex = -1;
	for (IndexList::Node* curNode = indexList.head(); curNode; curNode = curNode->next()) {
		int curIdx = curNode->value();
		if (curIdx > matchLength) {
			nextIndex = curIdx;
			break;
		} else if (curIdx == matchLength) {
			throw std::runtime_error("BadCharTable: Should not happen - "
									 "mismatch and match at the same time");
		}
	}

	int shiftAmount;
	if (nextIndex != -1) {
		// recommend shift to found occurance
		shiftAmount = nextIndex - matchLength;
	}
	else {
		// recommend maximum shift past the pattern beginning
		shiftAmount = static_cast<int>(_pattern.size()) - matchLength;
	}
	return shiftAmount;
}

auto BadCharTable::arenaSpaceNeeded(PatternRef pattern) -> size_t {
	// TODO: pessimistic padding
	size_t worstCaseIndexTablePadding = alignof(IndexTable) - 1;
	size_t indexTableSize = sizeof(IndexTable);
	size_t worstCaseListNodePadding = alignof(IndexList::Node) - 1;
	size_t patternListNodeSize = sizeof(IndexList::Node) * pattern.size();

	return worstCaseIndexTablePadding +
		indexTableSize +
		worstCaseListNodePadding +
		patternListNodeSize;
}

auto BadCharTable::buildTableInArena(ArenaAllocator& allocator, PatternRef pattern) -> IndexTable& {
	IndexTable* indexTable = allocator.construct<IndexTable>();
	if (indexTable == nullptr) {
		// TODO: bad alloc error msg
		throw std::bad_alloc();
	}
	// initialize array
	*indexTable = {};

	int lastPatternIndex = static_cast<int>(pattern.size() - 1);
	for (int pattern_idx = lastPatternIndex; pattern_idx >= 0; --pattern_idx) {
		if (pattern_idx == lastPatternIndex) {
			// don't put the patterns last symbol into the table,
			// as it won't be shifted to
			continue;
		}
		int shift = lastPatternIndex - pattern_idx;
		uint8_t curSymbol = pattern[pattern_idx];

		auto& shiftList = (*indexTable)[curSymbol];
		auto* nextShift = allocator.construct<IndexList::Node>(shift);
		if (nextShift == nullptr) {
			// TODO: bad alloc error msg
			throw std::bad_alloc();
		}
		shiftList.append(nextShift);
	}

	return *indexTable;
}
