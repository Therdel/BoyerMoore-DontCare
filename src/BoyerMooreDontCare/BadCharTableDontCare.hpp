#pragma once
#include <string>
#include <array>
#include <functional>

#include "ForwardList.hpp"

class ArenaAllocator;
class Signature;

class BadCharTableDontCare {
public:
	BadCharTableDontCare(ArenaAllocator &allocator, const Signature &signature);

	static auto arenaSpaceNeeded(const Signature &signature) -> size_t;
	auto printTable() const -> void;
	auto computeShift(int mismatchIdx, uint8_t mismatchSymbol) const -> int;

private:
	using IndexList = ForwardList<int>;
	using IndexTable = std::array<IndexList, 256>;

	const Signature &_signature;
	IndexTable &_indexTable;
	// holds right-most indices of dontCare areas
	// sorted from right to left
	IndexList &_dontCareRightIndexList;

	// calls handler with right-most indices of all dontCare areas of the signature
	// - which can be shifted to with pattern right-shift
	// from right to left
	static auto _iterateArrivableDontCareAreaRightIndices(const Signature& signature, std::function<void(int)> handler) -> void;
	static auto _buildTableInArena(ArenaAllocator& allocator, const Signature &signature) -> IndexTable&;
	static auto _buildDontCareRightIndexList(ArenaAllocator& allocator, const Signature& signature) -> IndexList&;
};