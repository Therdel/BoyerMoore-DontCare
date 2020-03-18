#include <iostream>
#include <string>

#include "BoyerMooreDontCare/BoyerMoore.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"
#include "BoyerMooreDontCare/Signature.hpp"
#include "BoyerMooreDontCare/BoyerMooreDontCare.hpp"

using namespace std;

auto printListTypeAlignAndSizes() {
	using IndexList = ForwardList<int>;
	using IndexTable = std::array<IndexList, 256>;
	using Node = IndexList::Node;
	cout << "IndexList size/align: " << sizeof(IndexList) << ", "
		<< alignof(IndexList) << endl;
	cout << "IndexTable size/align: " << sizeof(IndexTable) << ", "
		<< alignof(IndexTable) << endl;
	cout << "Node size/align: " << sizeof(Node) << ", "
		<< alignof(Node) << endl;
}

auto printMatchIndicatorString(const std::string_view& haystack, const std::vector<const uint8_t*>& matches) {
	auto* haystack_uint8_t = reinterpret_cast<const uint8_t*>(haystack.data());
	auto indexMatches = [&](int i) {
		for (auto* match : matches) {
			auto index = std::distance(haystack_uint8_t, match);
			if (index == i) {
				return true;
			}
		}
		return false;
	};

	for (int i = 0; i < haystack.size(); ++i) {
		if (indexMatches(i)) {
			std::cout << "^";
		}
		else {
			std::cout << " ";
		}
	}
	std::cout << std::endl;
}

#include <iomanip>
void test_badCharDontCare(Signature const&signature) {
	HeapArena arena{ BadCharTableDontCare::arenaSpaceNeeded(signature) };
	ArenaAllocator arenaAllocator{ arena };
	BadCharTableDontCare badCharTableDontCare{ arenaAllocator, signature };
	badCharTableDontCare.printTable();

	int mismatchIdx = 4;
	uint8_t mismatchSymbol = 0xba;
	std::cout << "Mismatch index=" << mismatchIdx << std::endl;
	std::cout << "Mismatch symbol=" << std::hex << "0x" << std::uppercase << static_cast<size_t>(mismatchSymbol) << std::resetiosflags(std::ios_base::basefield) << std::endl;
	std::cout << "Shift: " << badCharTableDontCare.computeShift(mismatchIdx, mismatchSymbol) << std::endl;

	int i = 3;
}

auto test_matchOrder(Signature const&signature) {
	auto& pattern = signature.pattern();
	auto& mask = signature.mask();
	cout << std::showbase;	// hex numbers with 0x prefix

	cout << "Pattern:\t";
	for (int i = 0; i < pattern.size(); ++i) {
		if (mask[i] == SignatureMask::DONT_CARE) {
			cout << "??\t";
		}
		else {
			cout << std::hex << std::showbase << static_cast<size_t>(pattern[i]) << '\t';
		}
	}
	cout << endl;
	cout << std::resetiosflags(std::ios_base::hex | std::ios_base::dec)
		<< std::noshowbase;

	// stdout order
	auto matchOrderDefault = MatchOrder::compute(signature, MatchOrder::DefaultBoyerMoore);
	auto matchOrderMaxDontCareDist = MatchOrder::compute(signature, MatchOrder::SortedMaxDontCareDistance);
	auto printOrder = [&pattern](MatchOrder::MatchOrder_t const& order) {
		for (int i = 0; i < pattern.size(); ++i) {
			auto it = std::find(order.begin(), order.end(), i);
			int orderIndex = static_cast<int>(std::distance(order.begin(), it));
			if (it != order.end()) {
				cout << orderIndex;
			}
			cout << '\t';
		}
		cout << endl;
	};
	cout << "Order: Default\t";
	printOrder(matchOrderDefault);
	cout << "Order: MaxDist\t";
	printOrder(matchOrderMaxDontCareDist);
}

auto test_GoodSuffixDontCare(Signature const& signature) {
	HeapArena arena{ GoodSuffixTableDontCare::arenaSpaceNeeded(signature) };
	ArenaAllocator arenaAllocator{ arena };
	auto matchOrder = MatchOrder::compute(signature, MatchOrder::SortedMaxDontCareDistance);
	GoodSuffixTableDontCare table(arenaAllocator, signature, matchOrder);

	auto& pattern = signature.pattern();
	auto& mask = signature.mask();
	cout << std::showbase;	// hex numbers with 0x prefix

	cout << "Pattern:\t";
	for (int i = 0; i < pattern.size(); ++i) {
		if (mask[i] == SignatureMask::DONT_CARE) {
			cout << "??\t";
		}
		else {
			cout << std::hex << std::showbase << static_cast<size_t>(pattern[i]) << '\t';
		}
	}
	cout << endl;
	cout << std::resetiosflags(std::ios_base::hex | std::ios_base::dec)
		<< std::noshowbase;

	// stdout shifts
	cout << "GoodTable:\t";
	for (int i = 0; i < pattern.size(); ++i) {
		int shift = table.computeShift(i);
		if (shift > 0) {
			std::cout << shift;
		}
		cout << '\t';
	}
	cout << endl;
	cout << "Aftermatchshift: " << table.afterMatchShift() << endl;
}

auto test_BoyerMooreDontCare(const Signature& signature) {
	BoyerMooreDontCare boyerMooreDontCare(signature);

}

int main() {
	/*

	// _bcde__ef_
	?? 62 63 64 65 ?? ?? 65 66 ??
	*/
	Signature signature("?? 41 4e ");
	test_matchOrder(signature);
	test_badCharDontCare(signature);
	test_GoodSuffixDontCare(signature);
	// usage example of BoyerMoore search class
	string_view haystack{ "ANPANMAN" };
	string pattern{ "o" };
	basic_string_view<uint8_t> haystack_uint8_t{ reinterpret_cast<uint8_t const*>(haystack.data()), haystack.size() };
	basic_string_view<uint8_t> pattern_uint8_t{ reinterpret_cast<uint8_t*>(pattern.data()), pattern.size() };

	//BoyerMoore boyerMoore(pattern_uint8_t);
	//auto matches = boyerMoore.search(haystack_uint8_t);
	BoyerMooreDontCare boyerMooreDontCare(signature);
	auto matches = boyerMooreDontCare.search(haystack_uint8_t);

	std::cout << "Pattern: '" << pattern << '\'' << std::endl;
	std::cout << "Haystack:" << std::endl;
	std::cout << haystack << std::endl;
	printMatchIndicatorString(haystack, matches);
}