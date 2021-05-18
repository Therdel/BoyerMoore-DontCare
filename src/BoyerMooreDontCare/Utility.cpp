#include <algorithm>	// std::find_first_of

#include "Utility.hpp"

namespace Util {
	auto split_for(std::string_view str, std::string_view delims, std::function<void(std::string_view)> handler) -> void {
		for (auto first = str.data(), second = str.data(), last = first + str.size();
			second != last && first != last; first = second + 1) {
			second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

			if (first != second) {
				std::string_view split_item(first, second - first);
				handler(split_item);
			}
		}
	}
}