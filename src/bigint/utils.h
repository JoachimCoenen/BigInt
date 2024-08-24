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
 * \brief Creates a to_vector_closure for operator()
 */
struct to_vector_adapter
{
	/**
	 * \brief Gets a vector of a given range.
	 * \tparam R type of range that gets converted to a vector.
	 * \param r range that gets converted to a vector.
	 * \return vector from the given range.
	 */
	template<std::ranges::range R>
	constexpr auto operator()(R&& r) const
	{
		std::vector<std::ranges::range_value_t<R>> v;

		// if we can get a size, reserve that much
		if constexpr (requires { std::ranges::size(r); }) {
			v.reserve(std::ranges::size(r));
		}

		v.insert(v.begin(), r.begin(), r.end());

		return v;
	}
};

/**
 * \brief Gets a closure to convert the range to a vector.
 * \return A to_vector_closure that will convert the range to a vector.
 */
constexpr auto to_vector() -> to_vector_adapter
{
	return to_vector_adapter{};
}

/**
 * \brief Gets a closure to convert the range to a vector.
 * \return A to_vector_closure that will convert the range to a vector.
 */
template<std::ranges::range R>
constexpr auto to_vector(R&& r)
{
	return to_vector_adapter{}(r);
}


/**
 * \brief A range pipe that results in a vector.
 * \tparam R type of range that gets converted to a vector.
 * \param r range that gets converted to a vector.
 * \param a used to create the vector.
 * \return a vector from the given range.
 */
template<std::ranges::range R>
constexpr auto operator|(R&& r, const to_vector_adapter& a)
{
	return a(std::forward<R>(r));
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


[[nodiscard]] inline unsigned long
stoul(const std::string_view input, int base = 10) {
	unsigned long result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline long
stol(const std::string_view input, int base = 10) {
	long result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline unsigned long long
stoull(const std::string_view input, int base = 10) {
	unsigned long long result;
	__from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline long long
stoll(const std::string_view input, int base = 10) {
	long long result;
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


// udiv128
#ifdef _MSC_VER
// #include <immintrin.h>
#include <intrin0.inl.h>

namespace bigint::utils {

uint64_t __inline udiv128(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor, uint64_t *remainder) {
	return _udiv128(high_dividend, low_dividend, divisor, remainder);
}

uint64_t __inline udiv128(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor) {
	uint64_t ignored_rm = 0;
	return _udiv128(high_dividend, low_dividend, divisor, &ignored_rm);
}

uint64_t __inline umul128(uint64_t multiplier, uint64_t multiplicand, uint64_t *high_product) {
	return _umul128(multiplier, multiplicand, high_product);
}

}

#else
namespace bigint::utils::_private {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-pedantic"
using uint128_t_ = unsigned __int128;
#pragma GCC diagnostic pop

uint128_t_ __inline make_uint128(uint64_t high, uint64_t low) {
	return (uint128_t_(high) << 64) | uint128_t_(low);
}

}
namespace bigint::utils {

uint64_t __inline udiv128(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor, uint64_t *remainder) {
	const auto dividend = _private::make_uint128(high_dividend, low_dividend);
	*remainder = uint64_t(dividend % divisor);
	return uint64_t(dividend / divisor); // maybe add overflow check?
}

uint64_t __inline udiv128(uint64_t high_dividend, uint64_t low_dividend, uint64_t divisor) {
	const auto dividend = _private::make_uint128(high_dividend, low_dividend);
	return uint64_t(dividend / divisor); // maybe add overflow check?
}

uint64_t __inline umul128(uint64_t multiplier, uint64_t multiplicand, uint64_t *high_product) {
	const auto result = _private::uint128_t_(multiplier) * _private::uint128_t_(multiplicand);
	*high_product = uint64_t(result >> 64);
	return uint64_t(result);
}

}

#endif

#endif // UTILS_H
