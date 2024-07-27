#ifndef UTILS_H
#define UTILS_H

#include <charconv>
#include <stdexcept>
#include <string_view>
#include <stdint.h>

//      [[nodiscard]] constexpr auto
#define CONSTEXPR_AUTO [[nodiscard]] constexpr auto
//      constexpr auto
#define CONSTEXPR_AUTO_DISCARD constexpr auto
//      constexpr void
#define CONSTEXPR_VOID constexpr void

namespace utils {

template <class T>
void __from_chars_throws(const std::string_view &input, T &result, int base) {
	const char* first = input.data();
	const char* last = input.data() + input.size();

	auto res = std::from_chars(first, last, result, base);

	// These two exceptions reflect the behavior of std::stoi.
	if (res.ec == std::errc::invalid_argument || res.ptr != last) {
		throw std::invalid_argument{"invalid_argument"};
	}
	else if (res.ec == std::errc::result_out_of_range) {
		throw std::out_of_range{"out_of_range"};
	}
}


inline unsigned long
stoul(const std::string_view& input, int base = 10) {
	unsigned long result;
	__from_chars_throws(input, result, base);
	return result;
}

inline unsigned long long
stoull(const std::string_view& input, int base = 10) {
	unsigned long long result;
	__from_chars_throws(input, result, base);
	return result;
}


constexpr uint64_t
consteval_pow(uint32_t base, uint8_t exponent)  {
	uint64_t base2 = base;
	uint64_t result = 1;
	for (uint8_t i = 0; i < 8; ++i) {
		if (exponent & (0b00000001 << i)) {
			result *= base2;
		};
		base2 *= base2;
	}
	return result;
}


#ifdef _MSC_VER
}
#include <intrin.h>
namespace utils {

uint32_t __inline ctzll(uint64_t value) {
	// adapted from https://stackoverflow.com/a/20468180/8091657
	unsigned long trailing_zero = 0;
	if (_BitScanForward64(&trailing_zero, value)) {
		return trailing_zero;
	} else {
		// This is undefined, I better choose 64 than 0
		return 64;
	}
}

uint32_t __inline clzll(uint64_t value) {
	// adapted from https://stackoverflow.com/a/20468180/8091657
	unsigned long leading_zero = 0;
	if (_BitScanReverse64(&leading_zero, value)) {
		return 63 - leading_zero;
	} else {
		// Same remarks as above
		return 64;
	}
}
#else

uint32_t __inline ctzll(uint64_t value) {
	if (value != 0) {
		return __builtin_ctzll(value);
	} else {
		return 64;
	}
}

uint32_t __inline clzll(uint64_t value) {
	if (value != 0) {
		return __builtin_clzll(value);
	} else {
		return 64;
	}
}

#endif

}

#endif // UTILS_H
