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

class STLCompatibleBoyerMoore {
public:
	STLCompatibleBoyerMoore(std::string_view needle)
	: _pattern({reinterpret_cast<const uint8_t*>(needle.data()), needle.size()})
	, _searcher(_pattern)
	{}

	template< class RandomIt2 >
	std::pair<RandomIt2, RandomIt2> operator()(RandomIt2 first, RandomIt2 last) const {
		return _searcher(first, last);
	}

private:
	std::basic_string_view<uint8_t> _pattern;
	BoyerMoore _searcher;
};

#include <algorithm>	// std::search
#include <functional>	// std::boyer_moore_searcher
#include <string>
#include <chrono>
#include <fstream>

template<typename F>
decltype(auto) exec_print_duration(F&& f, std::string description) {
	struct time_printer {
		static auto now() { return std::chrono::system_clock::now(); }
		time_printer(std::string description) : start(now()), d(std::move(description)) {}
		~time_printer() {
			auto duration = now() - start;
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			std::cout << d << " took " << ms << "ms" << std::endl;
		}
		std::chrono::time_point<std::chrono::system_clock> start;
		std::string d;
	};
	time_printer printer(std::move(description));
	return f();
}

auto test_compareWithSTL() {
	// read humongous haystack from file
	std::ifstream file("C:\\Users\\theod\\dev\\csshack\\lib\\BoyerMoore-DontCare\\examples\\haystack.txt", std::ios::binary | std::ios::ate);
	if (!file.good()) {
		cout << "Couldn't open file!" << endl;
		return;
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> haystack(size);
	if (!file.read(haystack.data(), size))
	{
		cout << "Couldn't read file!" << endl;
		return;
	}

	string needle = "Theodor\nLorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.\nUt wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.\nNam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis.\nAt vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.";
	boyer_moore_searcher stl_searcher(needle.begin(), needle.end());
	STLCompatibleBoyerMoore own_searcher(needle);
	auto stl_it = exec_print_duration([&] {return std::search(haystack.begin(), haystack.end(), stl_searcher); }, "stl_searcher");
	auto own_it = exec_print_duration([&] { return std::search(haystack.begin(), haystack.end(), own_searcher); }, "own_searcher");
	if (stl_it != haystack.end()) {
		cout << "stl_searcher found at " << stl_it - haystack.begin() << std::endl;
	}
	if (own_it != haystack.end()) {
		cout << "own_searcher found at " << own_it - haystack.begin() << std::endl;
	}
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
	test_compareWithSTL();
}