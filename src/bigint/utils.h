#ifndef UTILS_H
#define UTILS_H

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <type_traits>
#include <stdint.h>

//      [[nodiscard]] conseval auto
#define CONSTEVAL_AUTO [[nodiscard]] consteval auto

//      [[nodiscard]] constexpr auto
#define CONSTEXPR_AUTO [[nodiscard]] constexpr auto
//      constexpr auto
#define CONSTEXPR_AUTO_DISCARD constexpr auto
//      constexpr void
#define CONSTEXPR_VOID constexpr void

//      [[nodiscard]] auto
#define NODISCARD_AUTO [[nodiscard]] inline auto

namespace bigint::utils {

/**
 * The concept one_of<T, T0, Tn...> is satisfied if and only if std::remove_cvref_t<T> and Ti denote the same type for at least one Ti in the given Types T0, Tn, ...;.
 */
template <typename T, typename T0, typename... Tn>
concept one_of = std::same_as<std::remove_cvref_t<T>, T0> || (std::same_as<std::remove_cvref_t<T>, Tn> || ...);


template <typename T>
	requires std::convertible_to<T, std::string> || std::convertible_to<T, std::string_view> || std::integral<std::remove_cvref_t<T>> || std::floating_point<std::remove_cvref_t<T>>
std::string _to_string(T&& arg) {
	if constexpr (std::integral<std::remove_cvref_t<T>> || std::floating_point<std::remove_cvref_t<T>>) {
		return std::to_string(arg); // no need to forward integral and floating point types
	} else {
		return std::string{std::forward<T>(arg)};
	}
}


/** Concept which is satisfied by any type “T” such that for values “a” of type “T”,
 *  the expression utils::_to_string<T>(a) compiles and its result is convertible
 *  to std::string.
 */
template<typename T>
concept _to_string_able = requires(T a) {
	{ _to_string<T>(std::forward<T>(a)) } -> std::convertible_to<std::string>;
};


/**
 * @brief concatenates a number of strings
 * @param args the strings to
 * @return
 */
template<_to_string_able... Args>
std::string concat(Args&&... args) {
	return (_to_string(std::forward<Args>(args)) + ...);
}

std::string error_msg(std::string&& msg, const std::source_location& location = std::source_location::current()) {
	return concat(
		"{", location.file_name(),
		":", location.line(),
		":", location.function_name(),
		"} -> ", std::forward<std::string>(msg)
	);
}

}

// string_view to int
namespace bigint::utils {

template <class T>
void __from_chars_throws(const std::string_view input, T &result, int base) {
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


[[nodiscard]] inline uint32_t
stoul(const std::string_view input, int base = 10) {
	uint32_t result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline int32_t
stol(const std::string_view input, int base = 10) {
	int32_t result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline uint64_t
stoull(const std::string_view input, int base = 10) {
	uint64_t result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline int64_t
stoll(const std::string_view input, int base = 10) {
	int64_t result;
	__from_chars_throws(input, result, base);
	return result;
}

}


namespace bigint::utils {

/**
 * @brief std::pow() but for integers and constexpr.
 * @param base
 * @param exponent
 * @return
 */
CONSTEXPR_AUTO
ipow(uint32_t base, uint8_t exponent) -> uint64_t {
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


/**
 * @brief like std::abs(), but constexpr. (std::abs() is only constexpr since c++23.)
 * @param base
 * @param exponent
 * @return
 */
template <std::integral T>
CONSTEXPR_AUTO
constexpr_abs(T x) -> std::make_unsigned_t<T> {
	if constexpr (std::is_unsigned_v<T>) {
		return x;
	} else {
		return std::make_unsigned_t<T>(x < 0 ? -x : x);
	}
}


}


// clzll, clzll
#ifdef _MSC_VER
#include <intrin.h>
namespace bigint::utils {

uint32_t __inline ctzll(uint64_t value) {
	// adapted from https://stackoverflow.com/a/20468180/8091657
	unsigned long trailing_zero = 0;
	if (_BitScanForward64(&trailing_zero, value)) {
		return trailing_zero;
	} else {
		// This is undefined, better choose 64 than 0
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

}

#else
namespace bigint::utils {

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

}

#endif


#ifdef _MSC_VER
#include <__msvc_int128.hpp>
namespace bigint::utils::_private {
using uint128_t_ = std::_Unsigned128;
}
#else
namespace bigint::utils::_private {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-pedantic"
using uint128_t_ = unsigned __int128;
#pragma GCC diagnostic pop
}
#endif

namespace bigint::utils {

uint64_t __inline div_u128_saturate(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor) {
	if (high_dividend == 0) {
		return low_dividend / divisor;
	}
	if (high_dividend < divisor) {
		const auto a2 = (_private::uint128_t_(high_dividend) << 64) | _private::uint128_t_(low_dividend);
		const auto q = a2 / divisor;
		return (uint64_t)q;
	} else {
		// overflow is clammped to 2^64 - 1
		return (uint64_t)0 - (uint64_t)1;
	}
}

}

#endif // UTILS_H
