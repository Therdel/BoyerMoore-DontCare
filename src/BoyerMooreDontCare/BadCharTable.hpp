#pragma once
#include <string>
#include <array>

#include "ForwardList.hpp"

class ArenaAllocator;

class BadCharTable {
public:
	using PatternRef = std::basic_string_view<uint8_t>;
	explicit BadCharTable(ArenaAllocator &allocator, PatternRef pattern);

	static auto arenaSpaceNeeded(PatternRef pattern)->size_t;
	auto printTable() const -> void;
	auto computeShift(int matchLength, uint8_t mismatchSymbol) const -> int;

private:
	using IndexList = ForwardList<int>;
	using IndexTable = std::array<IndexList, 256>;

	PatternRef _pattern;
	IndexTable &_indexTable;

	static auto buildTableInArena(ArenaAllocator& allocator, PatternRef pattern) -> IndexTable&;
};