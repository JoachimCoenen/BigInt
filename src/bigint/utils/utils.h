#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <type_traits>
#include <vector>
#include <cstdint>

#define CONSTEVAL_AUTO [[nodiscard]] consteval auto

#define CONSTEXPR_AUTO [[nodiscard]] constexpr auto
#define CONSTEXPR_AUTO_DISCARD constexpr auto
#define CONSTEXPR_VOID constexpr void

#define NODISCARD_AUTO [[nodiscard]] inline auto


// assertions that are included in the release build:
#define assert_release_msg(condition, msg) \
do { if(!(condition)) { \
std::fprintf(stderr, "%s in %s(...) at line %i in file %s", (msg), __func__, __LINE__, __FILE__); \
abort(); \
} } while (false)
#define assert_release(condition) \
do { if(!(condition)) { \
std::fprintf(stderr, "%s in %s(...) at line %i in file %s", #condition, __func__, __LINE__, __FILE__); \
abort(); \
} } while (false)

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

inline std::string error_msg(std::string&& msg, const std::source_location& location = std::source_location::current()) {
	return concat(
		"{", location.file_name(),
		":", location.line(),
		":", location.function_name(),
		"} -> ", std::forward<std::string>(msg)
	);
}

/**
 * @brief removes any character in `chars_to_remove` from the given `str`.
 * @param str string to remove characters from
 * @param chars_to_remove the characters to remove.
 */
CONSTEXPR_VOID
remove_chars_from_string(std::string& str, std::string_view chars_to_remove) {
	for (const auto char_to_remove : chars_to_remove) {
		str.erase(std::ranges::remove(str, char_to_remove).begin(), str.end());
	}
}

/**
 * @brief A range pipe that results in a string.
 */
template<std::ranges::range R, class F>
CONSTEXPR_AUTO
join_transformed_strings(const R& r, F to_string, std::string_view separator) -> std::string {
	std::stringstream ss;
	auto iter = r.begin();
	if (iter != r.end()) {
		ss << to_string(*iter);
		while (++iter != r.end()) {
			ss << separator << to_string(*iter);
		}
	}
	return ss.str();
}

}

// string_view to int
namespace bigint::utils {

template <class T, class BaseOrFmt>
CONSTEXPR_VOID
_from_chars_throws(const std::string_view input, T &result, BaseOrFmt baseOrFmt) {
	const char* first = input.data();
	const char* last = input.data() + input.size();

	auto res = std::from_chars(first, last, result, baseOrFmt);

	// These two exceptions reflect the behavior of std::stoi.
	if (res.ec == std::errc::invalid_argument || res.ptr != last) {
		throw std::invalid_argument{"invalid_argument"};
	}
	else if (res.ec == std::errc::result_out_of_range) {
		throw std::out_of_range{"out_of_range"};
	}
}


CONSTEXPR_AUTO
stoul(const std::string_view input, int base = 10) -> uint32_t {
	uint32_t result;
	_from_chars_throws(input, result, base);
	return result;
}

CONSTEXPR_AUTO
stol(const std::string_view input, int base = 10) -> int32_t {
	int32_t result;
	_from_chars_throws(input, result, base);
	return result;
}

CONSTEXPR_AUTO
stoull(const std::string_view input, int base = 10) -> uint64_t {
	uint64_t result;
	_from_chars_throws(input, result, base);
	return result;
}

CONSTEXPR_AUTO
stoll(const std::string_view input, int base = 10) -> int64_t {
	int64_t result;
	_from_chars_throws(input, result, base);
	return result;
}

CONSTEXPR_AUTO
stod(const std::string_view input) -> double {
	double result;
	_from_chars_throws(input, result, std::chars_format::general);
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
 * @brief like std::abs(), but returns the unsigned version of the argument. (std::abs() returns the same type as the argument).
 * And handles unsigned types correctly (no casting to signed type):
 * `std::numeric_limits<uint64_t>::max() == utils::constexpr_abs(std::numeric_limits<uint64_t>::max());`
 */
CONSTEXPR_AUTO
constexpr_abs(std::integral auto x) -> std::make_unsigned_t<decltype(x)> {
	if constexpr (std::is_unsigned_v<decltype(x)>) {
		return x;
	} else {
		return static_cast<std::make_unsigned_t<decltype(x)>>(x < 0 ? -x : x);
	}
}


}


// clzll, clzll
#ifdef _MSC_VER
#include <intrin.h>
namespace bigint::utils {

CONSTEXPR_AUTO
ctzll(uint64_t value) -> uint32_t {
	// adapted from https://stackoverflow.com/a/20468180/8091657
	unsigned long trailing_zero = 0;
	if (_BitScanForward64(&trailing_zero, value)) {
		return trailing_zero;
	} else {
		// This is undefined, better choose 64 than 0
		return 64;
	}
}

CONSTEXPR_AUTO
clzll(uint64_t value) -> uint32_t {
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

CONSTEXPR_AUTO
ctzll(uint64_t value) -> uint32_t {
	if (value != 0) {
		return __builtin_ctzll(value);
	} else {
		return 64;
	}
}

CONSTEXPR_AUTO
clzll(uint64_t value) -> uint32_t {
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

CONSTEXPR_AUTO
div_u128_saturate(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor) -> uint64_t {
	if (high_dividend == 0) {
		return low_dividend / divisor;
	}
	if (high_dividend < divisor) {
		const auto a2 = (static_cast<_private::uint128_t_>(high_dividend) << 64) | static_cast<_private::uint128_t_>(low_dividend);
		const auto q = a2 / divisor;
		return static_cast<uint64_t>(q);
	} else {
		// overflow is clamped to 2^64 - 1
		return std::numeric_limits<uint64_t>::max();
	}
}

CONSTEXPR_VOID
mul_u128(uint64_t lhs, uint64_t rhs, uint64_t *hi, uint64_t *lo) {
	auto result = static_cast<_private::uint128_t_>(lhs) * rhs;
	*lo = static_cast<uint64_t>(result);
	*hi = static_cast<uint64_t>(result >> 64);
}

}

// check_bounds
namespace bigint::utils {

/**
 * Checks whether `index` is smaller than `size`.
 * @param index the value to check
 * @param size size of the array/vector/span/etc. to be indexed
 * @throw std::invalid_argument if `index` is out of bounds.
 */
CONSTEXPR_VOID
check_bounds(std::integral auto index, decltype(index) size) {
	if (index >= size) {
		auto msg = concat(
			"index out of bound.",
			" size(): ", size, " index: ", index, ".");
		throw std::invalid_argument(error_msg(std::move(msg)));
	}
}

}


// build_table
namespace bigint::utils::_private {
	CONSTEXPR_AUTO
	_build_table_row(const uint64_t column_count, const std::vector<size_t>& colum_widths, const std::vector<std::string>& row) -> std::string {
		constexpr std::string_view column_divider = " | ";
		constexpr auto row_divider = '-';
		constexpr std::string_view row_column_divider = "-+-";

		std::string result_row;

		if (row.empty()) {
			result_row.append(row_column_divider);
			for (uint64_t i = 0; i < column_count; ++i) {
				const auto width = colum_widths[i];
				result_row.append(width, row_divider).append(row_column_divider);
			}
		} else {
			result_row.append(column_divider);
			uint64_t i = 0;
			for (; i < row.size(); ++i) {
				const auto& content = row[i];
				const auto indent = colum_widths[i] - content.length();
				result_row.append(indent, ' ').append(content).append(column_divider);
			}
			for (; i < column_count; ++i) {
				const auto width = colum_widths[i];
				result_row.append(width, ' ').append(column_divider);
			}
		}

		return result_row;
	}

	/// builds an ascii table. The values are right-aligned.
	/// @param table: A vector of rows. Column count can vary from row to row. Empty rows are interpreted as a horizontal delimiter (a horizontal line is drawn).
	CONSTEXPR_AUTO
	build_table(const std::vector<std::vector<std::string> > &table) -> std::string {
		if (table.empty()) {
			return "";
		}

		const uint64_t column_count = std::ranges::max_element(table, [](const auto &a, const auto &b) { return a.size() < b.size(); })->size();

		std::vector<size_t> colum_widths(column_count);
		for (const auto &row : table) {
			for (uint64_t i = 0; i < row.size(); ++i) {
				colum_widths[i] = std::max(colum_widths[i], row[i].size());
			}
		}

		// build the actual table string
		auto iter = table.begin();
		if (iter == table.end()) {
			return std::string{};
		}
		std::string str = _build_table_row(column_count, colum_widths, *iter);
		++iter;
		for (; iter != table.end(); ++iter) {
			str += "\n";
			str += _build_table_row(column_count, colum_widths, *iter);
		}

		return str;
	}
}
