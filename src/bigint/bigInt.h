#pragma once

#include "utils.h"

// standard library
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#ifndef BIGINT_ENABLE_BOUNDS_CHECKS
#	define BIGINT_ENABLE_BOUNDS_CHECKS 1
#endif

#include "_bigint_tracy_defines.h"


namespace bigint::_private {
/**
 * When Multiplying two BigInts A and B, the Karatsuba multiplication algorithm gets chosen if the number of
 * digits in the bigger operand is equal to or exceeds this threshold and the lower operand either also exceeds this
 * threshold or has at least half as many digits as the bigger operand.
 *
 * The optimal value seems to lie somewhere around 32 based on some basic testing.
 */
inline constexpr size_t MIN_DIGITS_FOR_MULT_KARATSUBA = 32;
}

namespace bigint {

class IBigIntLike { };

template <typename T>
concept is_BigInt_like = std::is_base_of_v<IBigIntLike, T>;

using utils::one_of;

}


// forward declarations:
namespace bigint {

class BigInt;

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base2(const BigInt &v) -> std::string;

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base8(const BigInt &v) -> std::string;

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base10(const BigInt &v) -> std::string;

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base16(const BigInt &v) -> std::string;

BIGINT_TRACY_CONSTEXPR_AUTO
to_string(const BigInt &v) -> std::string;

template<is_BigInt_like T>
NODISCARD_AUTO
to_debug_string(const T& value) -> std::string;

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base2(std::string_view input) -> BigInt;

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base8(std::string_view input) -> BigInt;

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base10(std::string_view input) -> BigInt;

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base16(std::string_view input) -> BigInt;

BIGINT_TRACY_CONSTEXPR_AUTO
from_string(std::string_view input) -> BigInt;


template <is_BigInt_like T>
CONSTEXPR_AUTO
is_zero(const T &value) -> bool;

template <is_BigInt_like T>
CONSTEXPR_AUTO
is_neg(const T &value) -> bool;

template <is_BigInt_like T>
CONSTEXPR_AUTO
is_pos(const T &value) -> bool;

}


// class BigInt:
namespace bigint {

enum class Sign: bool {
	POS = false,
	NEG = true
};

}


// class BigInt:
namespace bigint::_private {

CONSTEXPR_AUTO
neg(Sign sign) noexcept  -> Sign {
	return (sign == Sign::POS) ? Sign::NEG : Sign::POS;
}

template<std::integral T>
CONSTEXPR_AUTO
get_sign(T v) noexcept -> Sign {
	if constexpr (std::is_signed_v<T>) {
		return v < 0 ? Sign::NEG : Sign::POS;
	} else {
		return Sign::POS;
	}
}

CONSTEXPR_VOID
cleanup(std::vector<uint64_t>& data) {
	if (data.empty()) {
		data.resize(1);
		data[0] = 0;
		return;
	}

	for (auto i = data.size(); 0 <--i;) {
		if (data[i] == 0) {
			data.pop_back();
		} else {
			break;
		}
	}
}

}


// class BigInt:
namespace bigint {

class BigInt : public IBigIntLike
{

 public:
	constexpr
	BigInt(): BigInt(0) {}

	explicit constexpr
	BigInt(uint64_t v, Sign sign=Sign::POS)
		: _data({v}), _sign(sign)
	{ }

	explicit constexpr
		BigInt(std::signed_integral auto v)
		: BigInt{utils::constexpr_abs(v), v < 0 ? Sign::NEG : Sign::POS}
	{ }

	explicit BIGINT_TRACY_CONSTEXPR
	BigInt(const std::string_view v)
		: BigInt(from_string(v))
	{ }

	explicit constexpr
	BigInt(const utils::Span<const uint64_t> &v, Sign sign=Sign::POS)
		: _data(v.size()), _sign(sign) {
		std::copy(v.begin(), v.end(), _data.begin());
		if (_data.empty()) {
			_data.push_back(0);
		}
	}

	explicit constexpr
	BigInt(std::vector<uint64_t> &&v, Sign sign=Sign::POS)
		: _data(std::move(v)), _sign(sign) {
		std::copy(v.begin(), v.end(), _data.begin());
		if (_data.empty()) {
			_data.push_back(0);
		}
	}

	template<is_BigInt_like T>
	explicit constexpr
	BigInt(const T &v)
		: _data(v.size()), _sign(v.sign()) {
		for (size_t i = 0; i < v.size(); ++i) {
			_data[i] = v[i];
		}
		if (_data.empty()) {
			_data.push_back(0);
		}
	}

	template<is_BigInt_like T>
	CONSTEXPR_AUTO_DISCARD
	operator=(const T &other) -> BigInt& {
		BigInt tmp(other);
		std::swap(*this, tmp);
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return _sign;
	}

	CONSTEXPR_AUTO
	sign() noexcept -> Sign& {
		return _sign;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return _data.size();
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const noexcept -> uint64_t {
		return index >= size() ? 0 : _data[index];
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
#if BIGINT_ENABLE_BOUNDS_CHECKS
		utils::check_bounds(index, size());
#endif
		_data[index] = digit;
	}

	CONSTEXPR_VOID
	append(uint64_t v) { _data.push_back(v); }

	CONSTEXPR_VOID
	insert_front(uint64_t v) { _data.insert(_data.cbegin(), v); }

	CONSTEXPR_VOID
	remove_last() {
		if (_data.size() > 1) {
			_data.pop_back();
		} else {
			_data.at(0) = 0;
		}
	}

	BIGINT_TRACY_CONSTEXPR_VOID
	cleanup() {
		BIGINT_TRACY_ZONE_SCOPED;
		_private::cleanup(_data);
	}

	BIGINT_TRACY_CONSTEXPR_VOID
	resize(std::size_t size) {
		BIGINT_TRACY_ZONE_SCOPED;
		// size 0 clears the BigInt and sets its value to 0.
		_data.resize(std::max<size_t>(1, size), 0);
		if (size == 0) {
			_data[0] = 0;
		}
	}

	CONSTEXPR_AUTO
	_span() noexcept -> utils::Span<uint64_t> {
		return utils::Span{_data};
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> utils::Span<const uint64_t> {
		return utils::Span{_data.data(), _data.size()};
	}

public:
	[[nodiscard]] auto
	__data_for_testing_only() const -> std::vector<uint64_t>{
		auto result = _data;
		result.push_back(is_neg(*this) ? 1 : 0);
		return result;
	}

private:
	std::vector<uint64_t> _data;
	Sign _sign; // adds another 8 bytes :(
};

}


// is_zero(), is_neg(), ...:
namespace bigint {

CONSTEXPR_AUTO
is_zero(const utils::Span<const uint64_t> &value) -> bool {
	if (value.empty() || value.size() == 1 && value[0] == 0)
		return true;

	for (size_t i = value.size(); i --> 0;) {
		if (value[i])
			return false;
	}
	return true;
}

template <is_BigInt_like T>
CONSTEXPR_AUTO
is_zero(const T &value) -> bool {
	return is_zero(value._span());
}


template <is_BigInt_like T>
CONSTEXPR_AUTO
is_neg(const T &value) -> bool {
	return !is_zero(value) && value.sign() == Sign::NEG;
}


template <is_BigInt_like T>
CONSTEXPR_AUTO
is_pos(const T &value) -> bool {
	return !is_zero(value) && value.sign() == Sign::POS;
}


/**
 * @brief check whether `value` would fit into the given integral type `I`.
 * @return `true` if `value` would fit into the given integral type `I`, `false` otherwise.
 */
template <std::integral I>
CONSTEXPR_AUTO
fits(const is_BigInt_like auto& value) -> bool {
	if constexpr (std::is_unsigned_v<I>) {
		return value.size() == 1 && value.sign() == Sign::POS && std::numeric_limits<I>::max() <= value[0];
	} else {
		return value.size() == 1 && (
			(value.sign() == Sign::POS && std::numeric_limits<I>::max() <= value[0]) ||
			(value.sign() == Sign::NEG && -std::numeric_limits<I>::min() <= value[0])
		);
	}
}


/**
 * @brief check whether `value` would fit into a uint64_t.
 * @return `true` if `value` would fit into a uint64_t, `false` otherwise.
 */
CONSTEXPR_AUTO
fits_u64(const is_BigInt_like auto& value) -> bool { return fits<uint64_t>(value); }


/**
 * @brief check whether `value` would fit into a int64_t.
 * @return `true` if `value` would fit into a int64_t, `false` otherwise.
 */
CONSTEXPR_AUTO
fits_i64(const is_BigInt_like auto& value) -> bool { return fits<int64_t>(value); }


/**
 * @brief check whether `value` would fit into a uint32_t.
 * @return `true` if `value` would fit into a uint32_t, `false` otherwise.
 */
CONSTEXPR_AUTO
fits_u32(const is_BigInt_like auto& value) -> bool { return fits<uint32_t>(value); }


/**
 * @brief check whether `value` would fit into a int32_t.
 * @return `true` if `value` would fit into a int32_t, `false` otherwise.
 */
CONSTEXPR_AUTO
fits_i32(const is_BigInt_like auto& value) -> bool { return fits<int32_t>(value); }


/**
 * @brief converts the given `value` to the given integral type `I`., assuming it fits. If value is too big or small for `value` the result will be garbage.
 * @return the given `value` converted to the given integral type `I`.
 */
template <std::integral I>
CONSTEXPR_AUTO
as_integral(const is_BigInt_like auto& value) -> bool {
	if (value.sign() == Sign::POS) {
		return static_cast<I>(value[0]);
	} else {
		return -static_cast<I>(value[0]);
	}
}


/**
 * @brief converts the given `value` to uint64_t, assuming it fits. If value is too big or small for `value` the result will be garbage.
 * @return the given `value` as a uint64_t.
 */
CONSTEXPR_AUTO
as_u64(const is_BigInt_like auto& value) -> uint64_t { return as_integral<uint64_t>(value); }


/**
 * @brief converts the given `value` to int64_t, assuming it fits. If value is too big or small for `value` the result will be garbage.
 * @return the given `value` as a int64_t.
 */
CONSTEXPR_AUTO
as_i64(const is_BigInt_like auto& value) -> int64_t { return as_integral<int64_t>(value); }


/**
 * @brief converts the given `value` to uint32_t, assuming it fits. If value is too big or small for `value` the result will be garbage.
 * @return the given `value` as a uint32_t.
 */
CONSTEXPR_AUTO
as_u32(const is_BigInt_like auto& value) -> uint32_t { return as_integral<uint32_t>(value); }


/**
 * @brief converts the given `value` to int32_t, assuming it fits. If value is too big or small for `value` the result will be garbage.
 * @return the given `value` as a int32_t.
 */
CONSTEXPR_AUTO
as_i32(const is_BigInt_like auto& value) -> int32_t { return as_integral<int32_t>(value); }



}


// rshifted & lshifted:
namespace bigint::_private {

CONSTEXPR_AUTO
rshifted(const utils::Span<const uint64_t>& a, uint64_t shifted) {
	return a.subspan_trunc(shifted);
}

CONSTEXPR_AUTO
rshifted(const utils::Span<uint64_t>& a, uint64_t shifted) {
	return a.subspan_trunc(shifted);
}


CONSTEXPR_AUTO
rmasked(const utils::Span<const uint64_t>& a, uint64_t shifted, uint64_t mask_size) {
	return a.subspan_trunc(shifted, mask_size);
}

CONSTEXPR_AUTO
rmasked(const utils::Span<uint64_t>& a, uint64_t shifted, uint64_t mask_size) {
	return a.subspan_trunc(shifted, mask_size);
}

}


// abs(), neg:
namespace bigint::_private {

class BigIntNeg;
class BigIntAbsNeg;

class BigIntAbs: IBigIntLike {
public:
	explicit constexpr
	BigIntAbs(const BigInt& lhs) :
		_lhs(lhs) {}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return Sign::POS;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return lhs().size();
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return lhs()[index];
	}

	CONSTEXPR_AUTO
	_span() noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	lhs() const -> const BigInt& { return _lhs; }

private:
	const BigInt& _lhs;
};

class BigIntNeg: IBigIntLike {
public:

	explicit constexpr
	BigIntNeg(const BigInt& lhs) :
		_lhs(lhs) {}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return _private::neg(lhs().sign());
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return lhs().size();
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return lhs()[index];
	}

	CONSTEXPR_AUTO
	_span() noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	lhs() const -> const BigInt& { return _lhs; }

private:
	const BigInt& _lhs;
};

class BigIntAbsNeg: IBigIntLike {
public:
	explicit constexpr
	BigIntAbsNeg(const BigInt& lhs) :
		_lhs(lhs) {}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return Sign::NEG;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return lhs().size();
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return lhs()[index];
	}

	CONSTEXPR_AUTO
	_span() noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> utils::Span<const uint64_t> {
		return _lhs._span();
	}

	CONSTEXPR_AUTO
	lhs() const -> const BigInt& { return _lhs; }

private:
	const BigInt& _lhs;
};

}


// abs(), neg:
namespace bigint {

CONSTEXPR_AUTO
abs(const _private::BigIntAbs& a) -> _private::BigIntAbs {
	return a;
}

CONSTEXPR_AUTO
abs(const _private::BigIntNeg& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a.lhs());
}

CONSTEXPR_AUTO
abs(const _private::BigIntAbsNeg& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a.lhs());
}

CONSTEXPR_AUTO
abs(const BigInt& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a);
}

CONSTEXPR_AUTO
abs(BigInt&& a) -> BigInt {
	a.sign() = Sign::POS;
	return std::move(a);
}

CONSTEXPR_AUTO
operator-(const _private::BigIntNeg& a) -> BigInt {
	return a.lhs();
}

CONSTEXPR_AUTO
operator-(const _private::BigIntAbs& a) -> _private::BigIntAbsNeg {
	return _private::BigIntAbsNeg(a.lhs());
}

CONSTEXPR_AUTO
operator-(const _private::BigIntAbsNeg& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a.lhs());
}

CONSTEXPR_AUTO
operator-(const BigInt& a) -> _private::BigIntNeg {
	return _private::BigIntNeg(a);
}

CONSTEXPR_AUTO
operator-(BigInt&& a) -> BigInt {
	a.sign() = _private::neg(a.sign());
	return std::move(a);
}

}


// bitwise shift operations:
namespace bigint {

namespace _private {
CONSTEXPR_AUTO
lshift_safe(uint64_t a, uint64_t b) {
	return (b >= 64) ? 0 : a << b; // shifting by 64 bits for 64 bit int is undefined.
}

CONSTEXPR_AUTO
rshift_safe(uint64_t a, uint64_t b) {
	return (b >= 64) ? 0 : a >> b; // shifting by 64 bits for 64 bit int is undefined.
}

}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<<(const TLHS &a, uint64_t digits) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a)) {
		return BigInt{0, a.sign()};
	}

	const uint64_t start = digits / 64;
	BigInt result{0, a.sign()};
	result.resize(a.size() + start);
	digits %= 64;
	result.set(start, a[0] << digits);
	for (uint64_t i = 1; i < a.size(); ++i) {
		const uint64_t lo = _private::rshift_safe(a[i-1], 64-digits);
		const uint64_t hi = a[i] << digits;
		result.set(i + start, lo | hi);
	}
	const auto last = _private::rshift_safe(a[a.size()-1], 64-digits);
	if (digits > 0 && last != 0) {
		result.append(last);
	}
	return result;
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator>>(const TLHS &a, uint64_t digits) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	const uint64_t start = digits / 64;
	if (is_zero(a) || a.size() <= start) {
		return BigInt{0, a.sign()};
	}
	BigInt result{0, a.sign()};
	result.resize(a.size() - start);

	digits %= 64;
	for (uint64_t i = start; i < a.size()-1; ++i) {
		const uint64_t lo = a[i] >> digits;
		const uint64_t hi = _private::lshift_safe(a[i+1], 64-digits);
		result.set(i - start, lo | hi);
	}
	const auto last = a[a.size()-1] >> digits;
	if (last != 0) {
		result.set(result.size()-1, last);
	} else {
		result.remove_last();
	}
	return result;
}


template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<<=(TLHS &a, uint64_t digits) -> TLHS& {
	if (is_zero(a) || digits == 0) {
		return a;
	}

	const uint64_t start = digits / 64;
	digits %= 64;

	const uint64_t asize = a.size();

	a.resize(a.size() + start);

	const auto last = _private::rshift_safe(a[asize-1], 64-digits);
	if (digits > 0 && last != 0) {
		a.append(last);
	}

	for (uint64_t i = asize; i --> 1;) {
		const uint64_t lo = _private::rshift_safe(a[i-1], 64-digits);
		const uint64_t hi = a[i] << digits;
		a.set(i + start, lo | hi);
	}
	a.set(start, a[0] << digits);

	for (uint64_t i = start; i --> 0;) {
		a.set(i, 0);
	}

	return a;
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator>>=(TLHS &a, uint64_t digits) -> TLHS& {
	const uint64_t start = digits / 64;
	if (a.size() <= start) {
		a.resize(1);
		a.set(0, 0);
		return a;
	}
	if (is_zero(a) || digits == 0) {
		return a;
	}

	digits %= 64;
	for (uint64_t i = start; i < a.size()-1; ++i) {
		const uint64_t lo = a[i] >> digits;
		const uint64_t hi = _private::lshift_safe(a[i+1], 64-digits);
		a.set(i - start, lo | hi);
	}
	const auto last = a[a.size()-1] >> digits;
	a.resize(a.size() - start);
	if (last != 0) {
		a.set(a.size()-1, last);
	} else {
		a.remove_last();
	}
	return a;
}

}


// comparison operators:
namespace bigint {

BIGINT_TRACY_CONSTEXPR_AUTO
operator<=>(const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b) -> std::strong_ordering {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b)) {

		if (a.size() > b.size()) {
			for (size_t i = a.size(); i --> b.size();) {
				if (a[i]) { return std::strong_ordering::greater; }
			}
		} else if (b.size() > a.size()) {
			for (size_t i = b.size(); i --> a.size();) {
				if (b[i]) { return std::strong_ordering::less; }
			}
		}

		size_t i;
		for (i = std::min(a.size(), b.size()); i --> 1 && a[i] == b[i];) {
			// do nothing
		}
		return a[i] <=> b[i];
	}

	return is_zero(a) ? std::strong_ordering::less : std::strong_ordering::greater;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<=>(const TLHS &a, const TRHS &b) -> std::strong_ordering {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b) && a.sign() == b.sign()) {
		return a._span() <=> b._span();
	}

	return (is_neg(a) || is_pos(b)) ? std::strong_ordering::less : std::strong_ordering::greater;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) < 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator>(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) > 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) <= 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator>=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) >= 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator==(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) == 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator!=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) != 0;
}


template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator<=>(const TLHS &a, TRHS b) -> std::strong_ordering {
	if (a.size() > 1) {
		return is_neg(a) ? std::strong_ordering::less : std::strong_ordering::greater;
	} else if (is_neg(a)) {
		return (b < 0) ? a[0] <=> static_cast<uint64_t>(-b) : std::strong_ordering::less;
	} else {
		return (b < 0) ? std::strong_ordering::greater : a[0] <=> static_cast<uint64_t>(b);
	}
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator<(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) < 0;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator>(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) > 0;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator<=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) <= 0;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator>=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) >= 0;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator==(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) == 0;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator!=(const TLHS &a, const TRHS &b) -> bool {
	return (a <=> b) != 0;
}

}


// add / sub ignoring sign:
namespace bigint::_private {

/**
 * @brief adds two integers ignoring their sign. `a.size()` *must* be equal or greater than `b.size()`.
 * @param result the result will be put in here.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 * @return carry
 */
BIGINT_TRACY_CONSTEXPR_VOID
_add_ignore_sign(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(a.size() >= b.size());
	assert(result.size() >= a.size());

	bool c = false; // carry
	size_t i = 0;
	for (; i < b.size(); ++i) {
		const auto ai = a[i];
		auto result_i = ai + b[i];
		if (c) { ++result_i; }
		c = result_i < ai || (c && result_i == ai);
		result[i] = result_i;
	}

	for (; i < a.size(); ++i) {
		const auto ai = a[i];
		result[i] = ai + (c ? 1 : 0);
		c = ai == std::numeric_limits<decltype(ai)>::max() && c;
	}

	if (c) {
		result[i] = 1;
		++i;
	}

	for (; i < result.size(); ++i) {
		result[i] = 0;
	}
}

/**
 * @brief subtracts `b` from `a` ignoring their sign. `abs(a)` *must* be equal or greater than `abs(b)`.
 * @param result_lo the result will be put in here.
 * @param result_hi the result will be put in here.
 * @param a_lo the first operand.
 * @param a_hi the first operand.
 * @param b_lo the second operand.
 * @param b_hi the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_sub_ignore_sign_no_negative_result(
	uint64_t& result_hi,
	uint64_t& result_lo,
	const uint64_t a_hi,
	const uint64_t a_lo,
	const uint64_t b_hi,
	const uint64_t b_lo
) {
	result_lo = a_lo - b_lo;
	bool c = result_lo > a_lo; // carry

	result_hi = a_hi - b_hi;
	if (c) { --result_hi; }
	c = result_hi > a_hi || (c && result_hi == a_hi);

	if (c) { // should NEVER happen.
		auto msg = utils::concat(
			"leftover carry! abs(b) was greater than abs(a). This is not supported.",
			" c: ", c, ".");
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}
}


/**
 * @brief subtracts `b` from `a` ignoring their sign. `abs(a)` *must* be equal or greater than `abs(b)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_sub_ignore_sign_no_negative_result(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(result.size() >= std::max(a.size(), b.size()));

	bool c = false; // carry
	size_t i = 0;
	const auto min_size = std::min(a.size(), b.size());
	for (; i < min_size; ++i) {
		const auto ai = a[i];
		auto result_i = ai - b[i];
		if (c) { --result_i; }
		c = result_i > ai || (c && result_i == ai);
		result[i] = result_i;
	}

	if (a.size() >= b.size()) {
		for (; i < a.size(); ++i) {
			const auto ai = a[i];
			result[i] = ai - (c ? 1 : 0);
			c = ai == 0 && c;
		}
	} else {
		if (!is_zero(b.subspan_trunc(a.size()))) {
			std::string msg = "abs(b) was greater than abs(a). This is not supported.";
			throw std::invalid_argument(utils::error_msg(std::move(msg)));
		}
	}

	if (c) { // should NEVER happen.
		auto msg = utils::concat(
			"leftover carry! abs(b) was greater than abs(a). This is not supported.",
			" c: ", c, ".");
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}

	for (; i < result.size(); ++i) {
		result[i] = 0;
	}
}

/**
 * @brief adds two integers ignoring their sign.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 * @return carry
 */
BIGINT_TRACY_CONSTEXPR_VOID
add_ignore_sign(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b) {
	if (b.size() > a.size()) { // put the number with more digits first.
		_add_ignore_sign(result, b, a);
	} else {
		_add_ignore_sign(result, a, b);
	}
}

/**
 * @brief subtracts `b` from `a` ignoring their sign.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
sub_ignore_sign(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b) -> Sign {
	BIGINT_TRACY_ZONE_SCOPED;
	const bool isNegative = (a <=> b) == std::strong_ordering::less;
	if (isNegative) {
		_sub_ignore_sign_no_negative_result(result, b, a);
		return Sign::NEG;
	} else {
		_sub_ignore_sign_no_negative_result(result, a, b);
		return Sign::POS;
	}
}

}


// addition:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
add(TRES &result, TLHS &a, const TRHS &b) {
	result.resize(std::max(a.size(), b.size()) + 1);
	if (is_pos(a)) {
		if (is_pos(b)) {
			_private::add_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
		} else {
			result.sign() = _private::sub_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
		}
	} else {
		if (is_pos(b)) {
			result.sign() = _private::sub_ignore_sign(result._span(), const_cast<TRHS&>(b)._span(), a._span());
		} else {
			_private::add_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
			result.sign() = Sign::NEG;
		}
	}
	result.cleanup();
}


template <is_BigInt_like TRES, is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
add(TRES &result, TLHS &a, std::integral auto b) {
	result.resize(a.size() + 1);

	uint64_t abs_b = utils::constexpr_abs(b);
	if (is_pos(a)) {
		if (b > 0) {
			_private::add_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
		} else {
			result.sign() = _private::sub_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
		}
	} else {
		if (b > 0) {
			result.sign() = _private::sub_ignore_sign(result._span(), utils::Span{&abs_b, 1}, a._span());
		} else {
			_private::add_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
			result.sign() = Sign::NEG;
		}
	}
	result.cleanup();
}


template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const TLHS &a, std::integral auto b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(std::integral auto a, const TRHS &b) -> BigInt {
	return b + a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, std::integral auto b) -> TLHS& {
	add(a, a, b);
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, const TRHS &b) -> TLHS& {
	add(a, a, b);
	return a;
}

}


// subtraction:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
sub(TRES &result, TLHS &a, const TRHS &b) {
	result.resize(std::max(a.size(), b.size()) + 1);
	if (is_pos(a)) {
		if (is_pos(b)) {
			result.sign() = _private::sub_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
		} else {
			_private::add_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
		}
	} else {
		if (is_pos(b)) {
			_private::add_ignore_sign(result._span(), a._span(), const_cast<TRHS&>(b)._span());
			result.sign() = Sign::NEG;
		} else {
			result.sign() = _private::sub_ignore_sign(result._span(), const_cast<TRHS&>(b)._span(), a._span());
		}
	}
	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
sub(TRES &result, TLHS &a, std::integral auto b) {
	result.resize(a.size() + 1);

	uint64_t abs_b = utils::constexpr_abs(b);
	if (is_pos(a)) {
		if (b > 0) {
			result.sign() = _private::sub_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
		} else {
			_private::add_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
		}
	} else {
		if (b > 0) {
			_private::add_ignore_sign(result._span(), a._span(), utils::Span{&abs_b, 1});
			result.sign() = Sign::NEG;
		} else {
			result.sign() = _private::sub_ignore_sign(result._span(), utils::Span{&abs_b, 1}, a._span());
		}
	}
	result.cleanup();
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const TLHS &a, std::integral auto b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(std::integral auto a, const TRHS &b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TRHS&>(b), a);
	result.sign() = _private::neg(result.sign());
	return result;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, std::integral auto b) -> TLHS& {
	sub(a, a, b);
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, const TRHS &b) -> TLHS& {
	sub(a, a, b);
	return a;
}


}


// sign handing for multiplication & division:
namespace bigint::_private {

CONSTEXPR_AUTO
mult_sign(Sign a, Sign b) -> Sign {
	return a != b ? Sign::NEG : Sign::POS;
}

}

// multiplication:
namespace bigint {

struct MultResult {
	uint64_t lo, hi;

	[[nodiscard]] explicit constexpr
	operator BigInt() const {
		BigInt result{{lo, hi}};
		result.cleanup();
		return result;
	}
};

BIGINT_TRACY_CONSTEXPR_AUTO
mult(uint64_t a, uint64_t b) -> MultResult {
	BIGINT_TRACY_ZONE_SCOPED;
	const auto a_0 = a & 0xFFFFFFFFull;
	const auto a_1 = a >> 32;
	const auto b_0 = b & 0xFFFFFFFFull;
	const auto b_1 = b >> 32;
	const uint64_t r_00 = a_0 * b_0;
	const uint64_t r_01 = a_0 * b_1;
	const uint64_t r_10 = a_1 * b_0;
	const uint64_t r_11 = a_1 * b_1;

	auto c = r_11 >> 0;
	c += r_01 >> 32;
	c += r_10 >> 32;

	auto r = r_00 << 0;
	r += r_01 << 32;
	c += (r < (r_01 << 32)) ? 1 : 0;
	r += r_10 << 32;
	c += (r < (r_10 << 32)) ? 1 : 0;

	return MultResult{r, c};
}

namespace _private {

	BIGINT_TRACY_CONSTEXPR_VOID
	_mult_naive_ignore_sign(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, uint64_t b) {
		BIGINT_TRACY_ZONE_SCOPED;
		uint64_t c = 0; // carry
		size_t i = 0;
		for (; i < a.size(); i++) {
			const auto rc = mult(a[i], b);
			auto result_i = rc.lo + c;
			result[i] = result_i;
			c = rc.hi + (result_i < c ? 1 : 0); // account for addition overflow
		}

		if (c) {
			result[i] = c;
			++i;
		}

		for (; i < result.size(); ++i) {
			result[i] = 0;
		}
	}

}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult(TRES &result, TLHS &a, uint64_t b) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0 || is_zero(a)) {
		result = BigInt{};
		return;
	}

	result.resize(a.size() + 1);
	_private::_mult_naive_ignore_sign(result._span(), a._span(), b);
	result.sign() = a.sign();
	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult(TRES &result, TLHS &a, int64_t b) {
	mult(result, a, static_cast<uint64_t>(llabs(b)));
	if (b < 0) {
		result.sign() = _private::neg(result.sign());
	}
}


namespace _private {
	BIGINT_TRACY_CONSTEXPR_VOID
	_mult_naive_ignore_sign(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b, std::vector<uint64_t> &temp_vec) {
		BIGINT_TRACY_ZONE_SCOPED;
		if (is_zero(a) || is_zero(b)) {
			std::ranges::fill(result, 0);
			return;
		}

		// fill first digits with zeros, so we do not add to what ever garbage was in there.
		std::fill_n(result.begin(), std::min(a.size() + 1, result.size()), 0);
		// we don't need to fill all digits, because all subsequent digits are replaced by the carry of the previous addition.

		temp_vec.resize(a.size() + 1, 0);
		utils::Span temp(temp_vec);
		size_t i = 0;
		for (i = 0; i < b.size(); i++) {
			_mult_naive_ignore_sign(temp, a, b[i]);
			_add_ignore_sign(rmasked(result, i, temp.size() + 1), rmasked(result, i, temp.size()), temp);
		}

		i += temp.size() + 1;

		for (; i < result.size(); ++i) {
			result[i] = 0;
		}
	}

	struct KaratsubaStepTemps {
		std::vector<uint64_t> ac;
		std::vector<uint64_t> bd;
		std::vector<uint64_t> ab_cd;
		std::vector<uint64_t> a_b;
		std::vector<uint64_t> c_d;

		utils::UniquePtr<KaratsubaStepTemps> local_temps;

		CONSTEXPR_AUTO
		ac_span() -> utils::Span<uint64_t> { return utils::Span{ac}; };
		CONSTEXPR_AUTO
		bd_span() -> utils::Span<uint64_t> { return utils::Span{bd}; };
		CONSTEXPR_AUTO
		ab_cd_span() -> utils::Span<uint64_t> { return utils::Span{ab_cd}; };
		CONSTEXPR_AUTO
		a_b_span() -> utils::Span<uint64_t> { return utils::Span{a_b}; };
		CONSTEXPR_AUTO
		c_d_span() -> utils::Span<uint64_t> { return utils::Span{c_d}; };
	};

	CONSTEXPR_AUTO
	should_use_karatsuba(size_t a_size, size_t b_size) -> bool {
		const auto min_size = std::min(a_size, b_size);
		const auto max_size = std::max(a_size, b_size);
		return max_size >= MIN_DIGITS_FOR_MULT_KARATSUBA && (min_size >= MIN_DIGITS_FOR_MULT_KARATSUBA || min_size > max_size >> 1);
	}

	BIGINT_TRACY_CONSTEXPR_VOID
	_mult_karatsuba_step(const utils::Span<uint64_t> &result, const utils::Span<const uint64_t> &lhs_, const utils::Span<const uint64_t> &rhs_, KaratsubaStepTemps& temps) {
		BIGINT_TRACY_ZONE_SCOPED;
		// xx = mm(ac) + m((a+b) * (c+d) - ac - bd) + (bd)
		auto [lhs, rhs] = lhs_.size() >= rhs_.size() ? std::tie(lhs_, rhs_) : std::tie(rhs_, lhs_);

		assert(lhs.size() >= rhs.size());

		if (is_zero(rhs)) {
			std::ranges::fill(result, 0);
			return;
		}
		if (!should_use_karatsuba(lhs.size(), rhs.size())) {
			_mult_naive_ignore_sign(result, rhs, lhs, temps.ab_cd);
			return;
		}

		if (!temps.local_temps) {
			temps.local_temps = utils::UniquePtr(new KaratsubaStepTemps());
		}

		auto n = std::max(lhs.size(), rhs.size());
		const auto mid = n >> 1;

		const auto a = rmasked(lhs, mid, lhs.size());
		const auto b = rmasked(lhs, 0, mid);
		const auto c = rmasked(rhs, mid, rhs.size());
		const auto d = rmasked(rhs, 0, mid);

		KaratsubaStepTemps &local_temps = *temps.local_temps;

		temps.ac.resize(a.size() + c.size());
		_mult_karatsuba_step(temps.ac_span(), a, c, local_temps);
		cleanup(temps.ac);

		temps.bd.resize(b.size() + d.size());
		_mult_karatsuba_step(temps.bd_span(), b, d, local_temps);
		cleanup(temps.bd);


		temps.a_b.resize(std::max(a.size(), b.size()) + 1);
		add_ignore_sign(temps.a_b_span(), a, b);
		cleanup(temps.a_b);

		temps.c_d.resize(std::max(c.size(), d.size()) + 1);
		add_ignore_sign(temps.c_d_span(), c, d);
		cleanup(temps.c_d);

		temps.ab_cd.resize(temps.a_b.size() + temps.c_d.size());
		_mult_karatsuba_step(temps.ab_cd_span(), temps.a_b_span(), temps.c_d_span(), local_temps);
		cleanup(temps.ab_cd);

		[[maybe_unused]] auto sign = sub_ignore_sign(temps.ab_cd_span(), temps.ab_cd_span(), temps.ac_span());
		cleanup(temps.ab_cd);
		[[maybe_unused]] auto sign2 = sub_ignore_sign(temps.ab_cd_span(), temps.ab_cd_span(), temps.bd_span());
		cleanup(temps.ab_cd);

		std::ranges::copy(temps.bd, result.begin());
		std::fill(result.begin() + temps.bd.size(), result.end(), 0);

		const auto result_shifted1 = rshifted(result, mid);
		_add_ignore_sign(result_shifted1, result_shifted1, temps.ab_cd_span().subspan_trunc(0, result_shifted1.size()));

		const auto result_shifted2 = rshifted(result, mid << 1);
		_add_ignore_sign(result_shifted2, result_shifted2, temps.ac_span().subspan_trunc(0, result_shifted2.size()));
	}

}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult_naive(TRES &result, const TLHS &lhs, const TRHS &rhs) {
	result.resize(lhs.size() + rhs.size());
	std::vector<uint64_t> temp_vec;
	if (rhs.size() > lhs.size()) { // put the number with more digits first.
		_private::_mult_naive_ignore_sign(result._span(), rhs._span(), lhs._span(), temp_vec);
	} else {
		_private::_mult_naive_ignore_sign(result._span(), lhs._span(), rhs._span(), temp_vec);
	}
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult_karatsuba(TRES &result, const TLHS &lhs, const TRHS &rhs) {
	result.resize(lhs.size() + rhs.size());
	_private::KaratsubaStepTemps local_temps;
	_private::_mult_karatsuba_step(result._span(), lhs._span(), rhs._span(), local_temps);
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult(TRES &result, const TLHS &a, const TRHS &b) {
	if (_private::should_use_karatsuba(a.size(), b.size())) {
		mult_karatsuba(result, a, b);
	} else {
		mult_naive(result, a, b);
	}
}

template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator*(const TLHS &a, TRHS b) -> BigInt {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<TRHS>, uint64_t, int64_t>;
	BigInt result;
	mult(result, const_cast<TLHS&>(a), static_cast<TRHS2>(b));
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator*(TLHS a, const TRHS &b) -> BigInt {
	return b * a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator*(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	mult(result, a, b);
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator*=(TLHS &a, TRHS b) -> TLHS& {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<TRHS>, uint64_t, int64_t>;
	mult(a, a, static_cast<TRHS2>(b));
	return a;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator*=(BigInt &a, const TRHS &b) -> BigInt& {
	BigInt result;
	mult(result, a, b);
	a = std::move(result);
	return a;
}

}


// DivModResult:
namespace bigint {

template<class D, class R=D>
struct DivModResult {
	typedef D DD;
	typedef R RR;
	D d; R r;
};

}


// divmod ignoring sign:
namespace bigint::_private {

BIGINT_TRACY_CONSTEXPR_AUTO
_correct_d_and_subtract(const utils::Span<uint64_t> &x, const utils::Span<const uint64_t> &b, uint64_t d, const utils::Span<uint64_t> &temp) -> uint64_t {
	// all values are guaranteed to be positive.
	_mult_naive_ignore_sign(temp, b, d);

	if (sub_ignore_sign(x, x, temp) == Sign::NEG) {
		d -= 1;
		if (sub_ignore_sign(x, x, b) == Sign::POS) { // once was not enough to chang the sign.
			d -= 1;
			[[maybe_unused]]
			auto sign_of_x = sub_ignore_sign(x, x, b);
		}
	}
	// x is now positive again and (t = bd) holds.
	return d;
}

/**
 * @brief division algorithm adapted from "Nitin Verma, 2021, Implementing Basic Arithmetic for Large Integers: Division" and adapted.
 * @param result the result will be put in here. Size requirement: `result.d.size() == a.size() - b.size() + 1 && `result.r.size() == a.size() + 1`.
 * @param a the dividend
 * @param b the divisor
 * @param e estimator (?) for the divisor
 * @return
 */
template <bool ignore_quotient>
BIGINT_TRACY_CONSTEXPR_VOID
_divide_loop(const DivModResult<utils::Span<uint64_t>>& result, const utils::Span<const uint64_t> &a, const utils::Span<const uint64_t> &b, uint64_t e, std::vector<uint64_t>& temp) {
	const auto na = a.size();
	const auto nb = b.size();
	/* na >= nb holds. */
	assert(nb > 1); // single digit division must be handled by dedicated divmod_ignore_sign(DivModResult, Span uint64_t).
	if constexpr (!ignore_quotient) {
		/* quotient can have maximum (na-nb+1) digits */
		assert(result.d.size() == na - nb + 1);
	}
	assert(result.r.size() == na + 1);

	std::ranges::copy(a, result.r.begin());
	result.r.back() = 0;
	auto x_span = result.r.subspan(na - nb + 1);

	temp.resize(nb + 1);
	auto temp_span = utils::Span{temp};

	/* loop-invariant P: first m digits of ’a’ have been brought-down and processed. */
	for (auto i = na - nb + 1; i --> 0;) {
		x_span = utils::Span{x_span.data() - 1, x_span.size() + 1};
		uint64_t d = utils::div_u128_saturate(x_span[nb], x_span[nb-1], e); // yz/e;
		d = _correct_d_and_subtract(x_span, b, d, temp_span);
		if constexpr (!ignore_quotient) {
			result.d[i] = d;
		}
	}
	/* (loop-invariant P) AND (m=na) holds. */
	/* Now x contains the remainder. */
}


/**
 * @brief division & modulo ignoring any sign.
 * @param result the result will be put in here. Size requirement: `result.size() == a.size()`.
 * @param a the dividend. Can be the same span as the quotient of the result (`result.d.data() == a.data()`).
 * @param b the divisor. Either `uint32_t` or `uint64_t`.
 * @return the result
 */
template <bool ignore_quotient = false, one_of<uint32_t, uint64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
divmod_ignore_sign(DivModResult<utils::Span<uint64_t>, TRHS>& result, const utils::Span<const uint64_t>& a, TRHS b) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(a)) {
		std::ranges::fill(result.d, 0);
		result.r = 0;
		return;
	}

	TRHS x_lo = 0;
	for (auto i = a.size(); i --> 0;) {
		uint64_t qi;
		if constexpr (std::is_same_v<TRHS, uint32_t>) {
			const auto ai = a[i];
			const auto ai_hi = (ai >> 32) | (static_cast<uint64_t>(x_lo) << 32);
			const auto qi_hi = ai_hi / b;
			const auto c_hi = ai_hi % b;
			const auto ai_lo = (ai & 0xFFFFFFFF) | (static_cast<uint64_t>(c_hi) << 32);
			const auto qi_lo = ai_lo / b;
			x_lo = ai_lo % b; // ???
			qi = (qi_hi << 32) | qi_lo;
		} else {
			uint64_t x_hi;
			x_hi = x_lo;
			x_lo = a[i];
			qi = utils::div_u128_saturate(x_hi, x_lo, b); // yz/e;
			auto bqi = mult(b, qi);
			_sub_ignore_sign_no_negative_result(x_hi, x_lo, x_hi, x_lo, bqi.hi, bqi.lo);
		}

		if constexpr (!ignore_quotient) {
			result.d[i] = qi;
		}
	}

	result.r = x_lo;
}


template <bool ignore_quotient = false>
BIGINT_TRACY_CONSTEXPR_VOID
_resize_result_for_divide_loop(BigInt& quotient, BigInt& remainder, const size_t na, const size_t nb) {
	if constexpr (!ignore_quotient) {
		// quotient can have maximum (na-nb+1) digits
		quotient.resize(na - nb + 1);
	}
	remainder.resize(na + 1);
}


/**
 * @brief division algorithm adapted from Nitin Verma, 2021, Implementing Basic Arithmetic for Large Integers: Division
 *
 * @param aa the dividend
 * @param bb the divisor
 * @return the result
 */
template <bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_VOID
divmod_ignore_sign(BigInt& quotient, BigInt& remainder, const utils::Span<const uint64_t>& aa, const utils::Span<const uint64_t>& bb, std::vector<uint64_t>& temp) {
	BIGINT_TRACY_ZONE_SCOPED;

	if (is_zero(bb)) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(aa) || bb.size() > aa.size()) {
		quotient.resize(0);

		if constexpr (!ignore_remainder) {
			remainder.resize(aa.size());
			std::ranges::copy(aa, remainder._span().begin());
		} else {
			remainder.resize(1);
			// remainder could be any positive number. It's only used to signify that the remainder is non-zero.
			// Used for correcting a negative quotient in divmod(...)
			remainder.set(0, is_zero(aa) ? 0 : 1);
		}
		return;
	}
	if (bb.size() == 1) {
		if constexpr (!ignore_quotient) {
			quotient.resize(aa.size());
		}
		remainder.resize(1);

		DivModResult<utils::Span<uint64_t>, uint64_t> result2{quotient._span(), 0};
		divmod_ignore_sign<ignore_quotient>(result2, aa, bb[0]);
		remainder.set(0, result2.r);
		return;
	}

	uint64_t e = bb.back();
	if ((bb.size() > 1) && (e < 1ull<<63)) {
		/* normalization */
		const uint64_t f = utils::div_u128_saturate(1ull, 0ull, e + 1); // 1^64/(e + 1);

		std::vector<uint64_t> af(aa.size() + 1);
		_mult_naive_ignore_sign(utils::Span{af}, aa, f);
		cleanup(af);

		std::vector<uint64_t> bf(bb.size() + 1);
		_mult_naive_ignore_sign(utils::Span{bf}, bb, f);
		cleanup(bf);

		e = bf.back();

		_resize_result_for_divide_loop(quotient, remainder, af.size(), bf.size());
		_divide_loop<ignore_quotient>(DivModResult{quotient._span(), remainder._span()}, utils::Span{af}, utils::Span{bf}, e, temp);

		if constexpr (!ignore_remainder) { // fix remainder:
			DivModResult<utils::Span<uint64_t>, uint64_t> result2{remainder._span(), 0};
			divmod_ignore_sign(result2, remainder._span(), f);
		}
	} else {
		_resize_result_for_divide_loop(quotient, remainder, aa.size(), bb.size());
		_divide_loop<ignore_quotient>(DivModResult{quotient._span(), remainder._span()}, aa, bb, e, temp);
	}

	if constexpr (!ignore_quotient) {
		quotient.cleanup();
	}
	remainder.cleanup(); // remainder is also used by quotient.
}


template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_VOID
divmod(BigInt& quotient, BigInt& remainder, const TLHS &a, const TRHS &b, std::vector<uint64_t>& temp) {
	_private::divmod_ignore_sign<ignore_quotient, ignore_remainder>(quotient, remainder, a._span(), b._span(), temp);

	if constexpr (!ignore_remainder) {
		if (!is_zero(remainder)) {
			remainder.sign() = a.sign();
			if (a.sign() != b.sign()) {
				remainder += b;
			}
		}
	}
	if constexpr (!ignore_quotient) {
		quotient.sign() = _private::mult_sign(a.sign(), b.sign());
		if (!is_zero(remainder) and quotient.sign() == Sign::NEG) {
			quotient -= 1;
		}
		quotient.cleanup();
	}
}

template <is_BigInt_like TLHS, one_of<int32_t, uint32_t, int64_t, uint64_t> TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(BigInt& quotient, const TLHS &a, TRHS b) -> TRHS {
	if constexpr (!ignore_quotient) {
		quotient.resize(a.size());
	}
	TRHS remainder;
	{
		DivModResult<utils::Span<uint64_t>, std::make_unsigned_t<TRHS>> result2{quotient._span(), 0};
		_private::divmod_ignore_sign<ignore_quotient>(result2, a._span(), utils::constexpr_abs(b));
		remainder = result2.r;
	}

	if constexpr (!ignore_remainder) {
		if (remainder != 0) {
			if (is_neg(a)) {
				remainder = -remainder;
			}
			if (a.sign() != _private::get_sign(b)) {
				remainder += b;
			}
		}
	}
	if constexpr (!ignore_quotient) {
		quotient.sign() = _private::mult_sign(a.sign(), _private::get_sign(b));
		if (remainder != 0 and quotient.sign() == Sign::NEG) {
			quotient -= 1;
		}
		quotient.cleanup();
	}
	return remainder;
}

}


// divmod:
namespace bigint {

template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const TLHS &a, const TRHS &b) -> DivModResult<BigInt> {
	DivModResult<BigInt> result;
	std::vector<uint64_t> temp;
	_private::divmod<TLHS, TRHS, ignore_quotient, ignore_remainder>(result.d, result.r, a, b, temp);
	return result;
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const TLHS &a, one_of<int32_t, uint32_t, int64_t, uint64_t> auto b) -> DivModResult<BigInt, decltype(b)> {
	DivModResult<BigInt, decltype(b)> result;
	result.r = _private::divmod<TLHS, decltype(b), ignore_quotient, ignore_remainder>(result.d, a, b);
	return result;
}

}


// division:
namespace bigint {

template <is_BigInt_like TLHS, one_of<uint32_t, int32_t, uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const TLHS &a, TRHS b) -> BigInt {
	return divmod<TLHS, false, true>(a, b).d;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, false, true>(a, b).d;
}


template <one_of<uint32_t, int32_t, uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, TRHS b) -> BigInt& {
	[[maybe_unused]]auto remainder = _private::divmod<BigInt, TRHS, false, true>(a, a, b);
	return a;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, const TRHS &b) -> BigInt& {
	if (b.size() == 1) { // we can perform division inplace:
		auto remainder = _private::divmod<BigInt, uint64_t, false, true>(a, a, b[0]);
		a.sign() = _private::mult_sign(a.sign(), b.sign());
		// correct wrong corrections, caused by wrong sign of `b` supplied above:
		if (remainder != 0 && b.sign() == Sign::NEG) {
			a -= 1;
		}
	} else { // we cannot do it inplace:
		const auto result = divmod<BigInt, TRHS, false, true>(a, b).d;
		a = std::move(result);
	}
	return a;
}

}


// Modulo:
namespace bigint {

template <is_BigInt_like TLHS, one_of<uint32_t, int32_t, uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const TLHS &a, TRHS b) -> TRHS {
	return divmod<TLHS, true, false>(a, b).r;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, true, false>(a, b).r;
}

template <one_of<uint32_t, int32_t, uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%=(BigInt &a, const TRHS &b) -> BigInt& {
	TRHS result = divmod<BigInt, true, false>(a, b).r;
	a.resize(1);
	a.set(0, utils::constexpr_abs(result));
	a.sign() = _private::get_sign(result);
	return a;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%=(BigInt &a, const TRHS &b) -> BigInt& {
	a = divmod<BigInt, TRHS, true, false>(a, b).r;
	return a;
}

}


namespace bigint::_private {

consteval uint8_t calculate_base_power_64(uint32_t base) {
	// formula: result =floor(64 / log2(base))
	switch (base) {
	case 2:
		return 64;
	case 3:
		return 40;
	case 4:
		return 32;
	case 5:
		return 27;
	case 6:
		return 24;
	case 7:
		return 22;
	case 8:
		return 21;
	case 9:
		return 20;
	case 10:
		return 19;
	case 11:
		return 18;
	case 12: case 13:
		return 17;
	case 14: case 15: case 16:
		return 16;
	case 17: case 18: case 19:
		return 15;
	case 20: case 21: case 22: case 23:
		return 14;
	case 24: case 25: case 26: case 27: case 28: case 29: case 30:
		return 13;
	case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40:
		return 12;
	case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56:
		return 11;
	case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64:
		return 10;
	default:
		throw std::invalid_argument("illegal base! it must be between 2 and 64 (inclusive). Also, please use constexpr!");
	}
}

struct base_conversion_64 {
	explicit consteval base_conversion_64(uint32_t base) noexcept
		: base_power(calculate_base_power_64(base)), //  = 19 for base 10;
		division_base(utils::ipow(base, base_power)) {}
	uint8_t base_power;
	uint64_t division_base;
};

consteval uint8_t calculate_base_power_32(uint32_t base) {
	// formula: result =floor(64 / log2(base))
	switch (base) {
	case 2:
		return 32;
	case 3:
		return 20;
	case 4:
		return 16;
	case 5:
		return 13;
	case 6:
		return 12;
	case 7:
		return 11;
	case 8: case 9:
		return 10;
	case 10: case 11:
		return 9;
	case 12: case 13: case 14: case 15: case 16:
		return 8;
	case 17: case 18: case 19: case 20: case 21: case 22: case 23:
		return 7;
	case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40:
		return 6;
	case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62: case 63: case 64:
		return 5;
	default:
		throw std::invalid_argument("illegal base! it must be between 2 and 64 (inclusive). Also, please use constexpr!");
	}
}

struct base_conversion_32 {
	explicit consteval base_conversion_32(uint32_t base) noexcept
		: base_power(calculate_base_power_32(base)), //  = 9 for base 10;
		division_base(static_cast<uint32_t>(utils::ipow(base, base_power))) {}
	uint8_t base_power;
	uint32_t division_base;
};

CONSTEXPR_AUTO
to_char(uint8_t d) -> char {
	return (d > 9 ? 'a' - 10 : '0') + d;
}

template <int base, uint64_t len>
BIGINT_TRACY_CONSTEXPR_AUTO
to_string_padded_generic(uint64_t val) -> std::string {
	std::string result(len, '0');
	for (auto i = len; i --> 0;) {
		auto d = static_cast<uint8_t>(val % base);
		val /= base;
		result[i] = to_char(d);
	}
	return result;
}

}


// to_string, from_string, & digit_sum:
namespace bigint {

// todo convert argument to BigIntLike
template <int base = 10>
BIGINT_TRACY_CONSTEXPR_AUTO
to_string(const BigInt &v) -> std::string {
	constexpr auto conv = _private::base_conversion_32(base);

	std::string result;
	if constexpr (conv.division_base != 0) {
		DivModResult temp{v, (uint32_t)0};
		temp.d.sign() = Sign::POS;

		while (temp.d > 0) {
			temp = divmod(temp.d, conv.division_base);
			auto& digs = temp.r;
			result.insert(0, _private::to_string_padded_generic<base, conv.base_power>(digs));
			temp.d.cleanup();
		}
	} else { // special case for when base is a divider of 32.
		constexpr auto base_power = _private::base_conversion_64{base}.base_power;
		result.append(base_power * v.size(), '0');
		for (size_t i = v.size(); i --> 0;) {
			uint64_t digs = v[v.size() - i - 1];
			for (uint8_t j = base_power; j --> 0;) {
				auto d = static_cast<uint8_t>(digs % base);
				digs /= base;
				result.at((i) * base_power + j) = _private::to_char(d);
			}
		}
	}

	const auto index = result.find_first_of("123456789abcdefghijklmnopqrstuvwxyz");
	result.erase(0, index);
	if (result.empty()) {
		result = "0";
	}
	if (is_neg(v)) {
		result.insert(0, "-");
	}
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base2(const BigInt &v) -> std::string {
	return to_string<2>(v);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base8(const BigInt &v) -> std::string {
	return to_string<8>(v);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base10(const BigInt &v) -> std::string {
	return to_string<10>(v);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base16(const BigInt &v) -> std::string {
	return to_string<16>(v);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string(const BigInt &v) -> std::string {
	return to_string_base10(v);
}


template<is_BigInt_like T>
NODISCARD_AUTO
_to_debug_string_data(const T& value) -> std::string {
	if (value.size() == 0) {
		return "{}";
	} else {
		std::ostringstream oss;
		oss << "{";

		size_t i = 0;
		// add the first element with no delimiter
		oss << value[i];
		++i;
		for (; i < value.size(); ++i) {
			oss << ", ";
			oss << value[i];
		}

		oss << "}";
		return oss.str();
	}
}

template<is_BigInt_like T>
NODISCARD_AUTO
to_debug_string(const T& value) -> std::string {
	const std::string sign = value.sign() == Sign::POS ? "POS" : "NEG";
	return std::string{"IBigIntLike {_data: "} + _to_debug_string_data(value) + ", _sign: " + sign + "}";
}


template <int base = 10>
BIGINT_TRACY_CONSTEXPR_AUTO
from_string(const std::string_view input) -> BigInt {
	constexpr auto conv = _private::base_conversion_64(base);

	BigInt result{0};

	if (input.empty()) {
		return result;
	}

	const size_t i0 = input[0] == '-' or input[0] == '+' ? 1 : 0;

	if constexpr (conv.division_base != 0) {
		for (size_t i = i0; i < input.size(); i += conv.base_power) {
			auto substr = std::string_view(input).substr(i, conv.base_power);
			auto mul = substr.size() == conv.base_power ? conv.division_base : utils::ipow(base, static_cast<uint8_t>(substr.size()));
			auto add = utils::stoull(substr, base);
			result *= mul;
			result += add;
		}
		result.sign() = input[0] == '-' ? Sign::NEG : Sign::POS;
	} else { // special case for when base is a divider of 32.

		const auto digit_count = input.size() - i0;
		const auto big_int_digit_count = std::max<size_t>(0, digit_count / conv.base_power + (digit_count % conv.base_power > 0 ? 1 : 0));
		result.resize(big_int_digit_count);

		size_t i = input.size();
		for (size_t k = 0; k < result.size(); k += 1, i -= conv.base_power) {
			const auto window_size = std::min(i - i0, static_cast<size_t>(conv.base_power));
			auto substr = std::string_view(input).substr(i - window_size, window_size);
			auto add = utils::stoull(substr, base);
			result.set(k, add);
		}
	}
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base2(const std::string_view input) -> BigInt {
	return from_string<2>(input);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base8(const std::string_view input) -> BigInt {
	return from_string<8>(input);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base10(const std::string_view input) -> BigInt {
	return from_string<10>(input);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base16(const std::string_view input) -> BigInt {
	return from_string<16>(input);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string(const std::string_view input) -> BigInt {
	return from_string_base10(input);
}


 // todo convert argument to BigIntLike
inline std::ostream & operator<<(std::ostream &ostream, const BigInt &val) {
	return ostream << to_string(val);
}

//template<char...>
BIGINT_TRACY_CONSTEXPR_AUTO
operator ""_big(const char* literal) -> BigInt {
	std::string literal_string{literal};
	const bool is_hex = literal_string.starts_with("0x") || literal_string.starts_with("0X");
	const bool is_bin = literal_string.starts_with("0b") || literal_string.starts_with("0B");
	const bool is_oct = literal_string.starts_with("0");
	utils::remove_chars_from_string(literal_string, "'");
	if (is_hex) {
		return from_string_base16(std::string_view{literal_string}.substr(2));
	} else if (is_bin) {
		return from_string_base2(std::string_view{literal_string}.substr(2));
	} else if (is_oct) {
		return from_string_base8(literal_string);
	} else {
		return from_string_base10(literal_string);
	}
}

}

#include "_bigint_tracy_undefines.h"
