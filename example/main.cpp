#include <iostream>
#include <string>

#include "BoyerMooreDontCare/BoyerMoore.hpp"
#include "HeteroArenaAllocator/ArenaAllocator.hpp"

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

int main() {
	// usage example of BoyerMoore search class
	string_view haystack{ "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed ." };
	string pattern{ "Lorem ipsum dolor sit ame" };
	basic_string_view<uint8_t> haystack_uint8_t{ reinterpret_cast<uint8_t const*>(haystack.data()), haystack.size() };
	basic_string_view<uint8_t> pattern_uint8_t{ reinterpret_cast<uint8_t*>(pattern.data()), pattern.size() };

	BoyerMoore boyerMoore(pattern_uint8_t);
	auto matches = boyerMoore.search(haystack_uint8_t);

	std::cout << "Pattern: '" << pattern << '\'' << std::endl;
	std::cout << "Haystack:" << std::endl;
	std::cout << haystack << std::endl;
	printMatchIndicatorString(haystack, matches);
}