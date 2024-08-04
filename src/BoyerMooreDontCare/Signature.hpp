#pragma once
#include <string>		// std::stoi
#include <cstdint>
#include <stdexcept>	// std::invalid_argument

#include "Utility.hpp"

namespace SignatureMask {
	enum Field : bool {
		CARE = true,
		DONT_CARE = false
	};
}

class Signature {
public:
	using Pattern = std::basic_string<uint8_t>;
	using Mask = std::basic_string<SignatureMask::Field>;

	Signature(std::string_view signatureText) {
		_parseMaskAndPattern(signatureText);
	}

	auto pattern() const -> const Pattern& { return _pattern; }
	auto mask() const -> const Mask& { return _mask; }

private:
	Pattern _pattern;
	Mask _mask;

	auto _parseMaskAndPattern(std::string_view signatureText) -> void {
		using namespace SignatureMask;
		Utility::split_for(signatureText, " ", [this](std::string_view byteStr) {
			if (byteStr == "??" || byteStr == "?") {
				_pattern.push_back(0);
				_mask.push_back(DONT_CARE);
			}
			else {
				if (byteStr.size() != 2) {
					throw std::invalid_argument("Wrong signature byte description size");
				}
				uint8_t byteValue = std::stoi(std::string(byteStr), nullptr, 16);
				_pattern.push_back(byteValue);
				_mask.push_back(CARE);
			}
		});
	}
};
