#ifndef UTILS_H
#define UTILS_H

#include <charconv>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <type_traits>
#include <stdint.h>
#include <vector>

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

// //      auto
// #define AUTO_DISCARD auto

namespace bigint::utils {


/**
 * @brief A range adaptor that converts the range to a vector.
 */
struct to_vector_adapter { /* just a marker */ };

/**
 * @brief creates a range adaptor that converts the range to a vector.
 * @return A range adaptor that converts the range to a vector.
 */
CONSTEXPR_AUTO
to_vector() {
	return to_vector_adapter{};
}

/**
 * @brief A range pipe that results in a vector.
 * @param r range that gets converted to a vector.
 * @param a used to create the vector.
 * @return a vector from the given range.
 */
template<std::ranges::range R>
CONSTEXPR_AUTO
operator|(R&& r, [[maybe_unused]] const to_vector_adapter a) {
	std::vector<std::ranges::range_value_t<R>> v;

	// if we can get a size, reserve that much
	if constexpr (requires { std::ranges::size(r); }) {
		v.reserve(std::ranges::size(r));
	}

	v.insert(v.begin(), r.begin(), r.end());

	return v;
}

/**
 * @brief A range adaptor that converts the range to a vector.
 */
struct join_str_adapter {
	std::string separator;
};

/**
 * @brief creates a range adaptor that converts the range to a vector.
 * @return A range adaptor that converts the range to a vector.
 */
CONSTEXPR_AUTO
join_str(const std::string& separator) {
	return join_str_adapter{separator};
}

/**
 * @brief A range pipe that results in a string.
 * @param r range that gets converted to a string.
 * @param a used to create the string.
 * @return a string from the given range.
 */
template<std::ranges::range R>
	requires std::same_as<std::ranges::range_value_t<R>, std::string>
CONSTEXPR_AUTO
operator|(R&& r, [[maybe_unused]] const join_str_adapter& a) {
	auto iter = r.begin();
	auto end = r.end();

	if (iter == end) {
		return std::string{};
	}

	std::string str = *iter;
	++iter;
	for (; iter != end; ++iter) {
		str += a.separator + *iter;
	}

	return str;
}


namespace _private {

template <class T, T... Vs, class Op>
CONSTEXPR_VOID
consteval_for (std::integer_sequence<T, Vs...> const &, const Op& op) {
	using unused = int[]; // just a trick to provide an environment where to unpack the Vs... values
	(void)unused { 0, (op.template operator()<Vs>(), 0)... };
}

}

template <size_t N, class Op>
CONSTEXPR_VOID
consteval_for (const Op& op) {
	_private::consteval_for(std::make_index_sequence<N>{}, op);
}


/**
 * Returns the Nth type in the given template parameter pack.
 */
template<int N, typename... Ts>
using nth_type_of = std::tuple_element_t<N, std::tuple<Ts...>>;

/**
 * returns the first type in the given template parameter pack.
 */
template<typename... Ts>
using first_type_of = std::tuple_element_t<0, std::tuple<Ts...>>;


/**
 * @brief Extracts the Ith element from the tuple. I must be an integer value in [​0​, sizeof...(Ts)).
 * @param ts the parameter pack.
 */
template <int I, class... Ts>
const auto& get(const Ts&... ts) {
	return std::get<I>(std::tie(ts...));
}


template <typename T, typename T0, typename... Tn>
concept one_of = std::same_as<std::remove_cvref_t<T>, T0> || (std::same_as<std::remove_cvref_t<T>, Tn> || ...);

template <typename T, bool T0, bool... Tn>
concept or_c = T0 || (Tn || ...);


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
concept _to_string_able = requires(T a)
{
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
	}

	// overflow is clammped to 2^64 - 1
	return (uint64_t)0 - (uint64_t)1;
}

}

#endif // UTILS_H
