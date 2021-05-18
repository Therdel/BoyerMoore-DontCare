#pragma once
#include <string_view>
#include <functional>

namespace Util {
    // simple string split
    // source: https://www.bfilipek.com/2018/07/string-view-perf-followup.html
    // code: https://github.com/fenbf/StringViewTests/blob/master/StringViewTest.cpp
	auto split_for(std::string_view str, std::string_view delims, std::function<void(std::string_view)> handler) -> void;
}