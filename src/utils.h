#ifndef UTILS_H
#define UTILS_H

#include <charconv>
#include <stdexcept>
#include <string_view>

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

}

#endif // UTILS_H
