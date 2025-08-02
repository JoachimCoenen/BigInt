#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <type_traits>
#include <vector>
#include <cstdint>

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


// assertions that are included in the release build:
#define assert_release_msg(condition, msg) \
if(!(condition)) { \
std::fprintf(stderr, "%s in %s(...) at line %i in file %s", (msg), __func__, __LINE__, __FILE__); \
abort(); \
}
#define assert_release(condition) \
if(!(condition)) { \
std::fprintf(stderr, "%s in %s(...) at line %i in file %s", #condition, __func__, __LINE__, __FILE__); \
abort(); \
}

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

}

// string_view to int
namespace bigint::utils {

template <class T>
void _from_chars_throws(const std::string_view input, T &result, int base) {
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
	_from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline int32_t
stol(const std::string_view input, int base = 10) {
	int32_t result;
	_from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline uint64_t
stoull(const std::string_view input, int base = 10) {
	uint64_t result;
	_from_chars_throws(input, result, base);
	return result;
}

[[nodiscard]] inline int64_t
stoll(const std::string_view input, int base = 10) {
	int64_t result;
	_from_chars_throws(input, result, base);
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
 */
CONSTEXPR_AUTO
constexpr_abs(std::integral auto x) -> std::make_unsigned_t<decltype(x)> {
	if constexpr (std::is_unsigned_v<decltype(x)>) {
		return x;
	} else {
		return std::make_unsigned_t<decltype(x)>(x < 0 ? -x : x);
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
		const auto a2 = (static_cast<_private::uint128_t_>(high_dividend) << 64) | static_cast<_private::uint128_t_>(low_dividend);
		const auto q = a2 / divisor;
		return static_cast<uint64_t>(q);
	} else {
		// overflow is clamped to 2^64 - 1
		return static_cast<uint64_t>(0) - static_cast<uint64_t>(1);
	}
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

namespace bigint::utils {
/**
 *
 * very simple unique_ptr that supports constexpr.
 */
template<typename T>
struct UniquePtr {
	explicit constexpr UniquePtr(T* ptr) noexcept : ptr(ptr) { }
	explicit constexpr UniquePtr() noexcept : ptr(nullptr) { }
	constexpr ~UniquePtr() { delete ptr; }

	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;
	UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	UniquePtr& operator=(UniquePtr&& other) noexcept {
		std::swap(ptr, other.ptr);
		return *this;
	}

	CONSTEXPR_AUTO
	operator *() const noexcept -> T& { return *ptr; }

	CONSTEXPR_AUTO
	operator ->() const noexcept -> T* { return ptr; }

	[[nodiscard]] constexpr
	explicit operator bool() const noexcept { return ptr != nullptr; }
private:
	T* ptr;
};

}

namespace bigint::utils {

template <typename T>
class Span {
public:
	using size_type = std::size_t;

	// constructors, copy and assignment

	constexpr
	Span() noexcept
		: _data(nullptr), _size(0)
	{ }

	constexpr
	Span(T* data, size_type size) noexcept
		: _data(data), _size(size)
	{ }

	template <size_type N>
	explicit constexpr
	Span (std::type_identity_t<T>(&arr)[N])
	noexcept
	: Span (static_cast<T*>(arr), N)
	{ }

	template <size_type N>
	requires std::is_const_v<T>
	explicit constexpr
	Span(const std::array<std::remove_const_t<T>, N>& arr) noexcept
		: Span(arr.data(), N)
	{ }

	template <size_type N>
	requires (!std::is_const_v<T>)
	explicit constexpr
	Span(std::array<T, N>& arr) noexcept
		: Span(arr.data(), N)
	{ }

	template <class T2>
	requires (std::is_const_v<T> && !std::is_const_v<T2> && std::is_same_v<std::remove_const_t<T>, T2>)
	explicit constexpr
	Span(const std::vector<T2>& vec) noexcept
		: Span(vec.data(), vec.size())
	{ }

	explicit constexpr
	Span(std::vector<T>& vec) noexcept
		: Span(vec.data(), vec.size())
	{ }

	template <class T2>
	requires (std::is_const_v<T> && !std::is_const_v<T2> && std::is_same_v<std::remove_const_t<T>, T2>)
	constexpr
	Span(const Span<T2>& other) noexcept
		: _data(other.data()), _size(other.size())
	{ }

	constexpr
	Span(const Span&) noexcept = default;

	~Span() noexcept = default;

	constexpr Span&
	operator=(const Span&) noexcept = default;

	// observers

	CONSTEXPR_AUTO
	size() const noexcept -> size_type { return _size; }

	CONSTEXPR_AUTO
	size_bytes() const noexcept -> size_type { return _size * sizeof(T); }

	CONSTEXPR_AUTO
	empty() const noexcept -> bool { return size() == 0; }

	// element access

	CONSTEXPR_AUTO
	front() const noexcept -> T& {
		assert(!empty());
		return *_data;
	}

	CONSTEXPR_AUTO
	back() const noexcept -> T& {
		assert(!empty());
		return *(_data + (_size - 1));
	}

	CONSTEXPR_AUTO
	operator[](size_type idx) const noexcept -> T& {
		assert(idx < _size);
		return *(_data + idx);
	}

	CONSTEXPR_AUTO
	data() const noexcept -> T* { return _data; }

	// iterator support

	CONSTEXPR_AUTO
	begin() const noexcept -> T* { return _data; }

	CONSTEXPR_AUTO
	end() const noexcept -> T* { return _data + _size; }

	// subviews

	CONSTEXPR_AUTO
	first(size_type count) const noexcept -> Span {
		assert(count <= _size);
		return {_data, count};
	}

	CONSTEXPR_AUTO
	last(size_type count) const noexcept -> Span {
		assert(count <= _size);
		return {_data + (_size - count), count};
	}

private:
	static constexpr size_type full_extent = static_cast<size_t>(-1);

public:

	CONSTEXPR_AUTO
	subspan(size_type offset, size_type count = full_extent) const noexcept -> Span {
		assert(offset <= _size);
		if (count == full_extent)
			count = _size - offset;
		else {
			assert(count <= _size);
			assert(offset + count <= _size);
		}
		return {_data + offset, count};
	}

	/**
	 * like @link subspan, but `offset` and `count` are truncated automatically.
	 * @param offset
	 * @param count
	 * @return
	 */
	CONSTEXPR_AUTO
	subspan_trunc(size_type offset, size_type count = full_extent) const noexcept -> Span {
		offset = _size > offset ? offset : _size;
		count = std::min<size_type>(_size - offset, count);
		return subspan(offset, count);
		// return {_data + offset, count};
	}

private:
	T* _data;
	size_type _size;
};

}
