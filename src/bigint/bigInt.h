#pragma once

#include "utils/utils.h"
#include "utils/pool.h"

// standard library
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#ifndef BIGINT_ENABLE_BOUNDS_CHECKS
#	define BIGINT_ENABLE_BOUNDS_CHECKS 1
#endif

#include "utils/_bigint_tracy_defines.h"


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

using DigitsVec = std::vector<uint64_t>;

template <>
struct pool::pooled_reset<DigitsVec> {
	static CONSTEXPR_VOID
	reset(DigitsVec& v) { v.clear(); }
};

using DigitsVecPtr = pool::pooled_ptr<DigitsVec>;

}


// forward declarations:
namespace bigint {

class BigInt;
BIGINT_TRACY_CONSTEXPR_AUTO
from_string(std::string_view input, uint_fast8_t base = 10) -> BigInt;

CONSTEXPR_AUTO
is_neg(const is_BigInt_like auto &value) -> bool;

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

CONSTEXPR_AUTO
get_sign(std::integral auto v) noexcept -> Sign {
	return v < 0 ? Sign::NEG : Sign::POS;
}

CONSTEXPR_VOID
cleanup(DigitsVec& data) {
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
	using size_type = DigitsVec::size_type;

 public:
	constexpr
	BigInt(): BigInt(0) {}

	explicit constexpr
	BigInt(uint64_t v, Sign sign=Sign::POS)
		: _sign(sign)
	{
		_data().push_back(v);
	}

	explicit constexpr
		BigInt(std::signed_integral auto v)
		: BigInt{utils::constexpr_abs(v), v < 0 ? Sign::NEG : Sign::POS}
	{ }

	explicit BIGINT_TRACY_CONSTEXPR
	BigInt(const std::string_view v)
		: BigInt(from_string(v))
	{ }

	explicit constexpr // todo: maybe remove
	BigInt(const std::span<const uint64_t> &v, Sign sign=Sign::POS)
	: _sign(sign) {
		_data().resize(v.size());
		std::copy(v.begin(), v.end(), _data().begin());
		if (_data().empty()) {
			_data().push_back(0);
		}
	}

	explicit constexpr
	BigInt(DigitsVecPtr &&v, Sign sign=Sign::POS)
		: _data_ptr(std::move(v)), _sign(sign) {
		if (_data().empty()) {
			_data().push_back(0);
		}
	}

	/** use the `.copy()` method instead. */
	BigInt(const BigInt &v) = delete;
	BigInt(BigInt &&v) = default;
	/** use the `.copy()` method instead. */
	BigInt& operator=(const BigInt &v) = delete;
	BigInt& operator=(BigInt &&v) = default;

	/**
	 *
	 * @return a new copy of this BigInt.
	 */
	CONSTEXPR_AUTO
	copy() const noexcept -> BigInt {
		BigInt result;
		result._data() = _data();
		result._sign = _sign;
		return result;
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
	size() const noexcept -> size_type {
		return _data().size();
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const noexcept -> uint64_t {
		return index >= size() ? 0 : _data()[index];
	}

	CONSTEXPR_VOID
	set(size_type index, uint64_t digit) {
#if BIGINT_ENABLE_BOUNDS_CHECKS
		utils::check_bounds(index, size());
#endif
		_data()[index] = digit;
	}

	CONSTEXPR_VOID
	append(uint64_t v) { _data().push_back(v); }

	CONSTEXPR_VOID
	remove_last() {
		if (_data().size() > 1) {
			_data().pop_back();
		} else {
			_data()[0] = 0;
		}
	}

	BIGINT_TRACY_CONSTEXPR_VOID
	cleanup() {
		BIGINT_TRACY_ZONE_SCOPED;
		_private::cleanup(_data());
	}

	CONSTEXPR_VOID
	resize(size_type size) {
		// size 0 clears the BigInt and sets its value to 0.
		_data().resize(std::max<size_type>(1, size));
		if (size == 0) {
			_data()[0] = 0;
		}
	}

	CONSTEXPR_VOID
	resize(size_type size, const uint64_t default_digit) {
		// size 0 clears the BigInt and sets its value to 0.
		_data().resize(std::max<size_type>(1, size), default_digit);
		if (size == 0) {
			_data()[0] = 0;
		}
	}

	CONSTEXPR_VOID
	reserve(size_type size) {
		_data().reserve(std::max<size_type>(1, size));
	}

	CONSTEXPR_AUTO
	_span() noexcept -> std::span<uint64_t> {
		return std::span{_data()};
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> std::span<const uint64_t> {
		return std::span{_data().data(), _data().size()};
	}

	/**
	 * @return the underlying BigInt instance.
	 */
	CONSTEXPR_AUTO
	_bigint() const noexcept -> const BigInt& {
		return *this;
	}

public:
	[[nodiscard]] auto
	__data_for_testing_only() const -> DigitsVec {
		DigitsVec result = _data(); // copy!
		result.resize(size() + 1);
		std::copy(_data().begin(), _data().end(), result.begin());
		result.back() = is_neg(*this) ? 1 : 0;
		return result;
	}

private:
	CONSTEXPR_AUTO
	_data() const noexcept -> const DigitsVec& { return *_data_ptr; }
	CONSTEXPR_AUTO
	_data() noexcept -> DigitsVec& { return *_data_ptr; }

	[[no_unique_address]] DigitsVecPtr _data_ptr;
	Sign _sign; // adds another 8 bytes :(
};

}

// IntegralAdapter
namespace bigint::_private {

class IntegralAdapter: public IBigIntLike {
public:
	using size_type = BigInt::size_type;

	constexpr explicit
	IntegralAdapter(const std::integral auto value)
		: _value(utils::constexpr_abs(value)), _sign(get_sign(value))
	{}

	/**
	 * @return a new copy of this IntegralAdapter.
	 */
	CONSTEXPR_AUTO
	copy() const noexcept -> IntegralAdapter {
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return _sign;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> size_type {
		return 1;
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const noexcept -> uint64_t {
		return index >= 1 ? 0 : _value;
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> std::span<const uint64_t> {
		return std::span{&_value, 1};
	}

private:
	uint64_t _value;
	Sign _sign;
};
}


// is_zero(), is_neg(), ...:
namespace bigint {

CONSTEXPR_AUTO
is_zero(const std::span<const uint64_t> &value) -> bool {
	if (value.empty() || value.size() == 1 && value[0] == 0)
		return true;

	for (BigInt::size_type i = value.size(); i --> 0;) {
		if (value[i])
			return false;
	}
	return true;
}

CONSTEXPR_AUTO
is_zero(const is_BigInt_like auto &value) -> bool {
	return is_zero(value._span());
}


CONSTEXPR_AUTO
is_neg(const is_BigInt_like auto &value) -> bool {
	return !is_zero(value) && value.sign() == Sign::NEG;
}


CONSTEXPR_AUTO
is_pos(const is_BigInt_like auto &value) -> bool {
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

template <typename T>
CONSTEXPR_AUTO
rshifted(const std::span<T>& span, uint64_t shifted) -> std::span<T> {
	shifted = span.size() > shifted ? shifted : span.size();
	return span.subspan(shifted);
}

template <typename T>
CONSTEXPR_AUTO
rmasked(const std::span<T>& span, uint64_t shifted, uint64_t mask_size) -> std::span<T> {
	shifted = span.size() > shifted ? shifted : span.size();
	mask_size = std::min<std::size_t>(span.size() - shifted, mask_size);
	return span.subspan(shifted, mask_size);
}

}


// abs(), neg:
namespace bigint::_private {

class BigIntNeg;
class BigIntAbsNeg;

class BigIntAbs: public IBigIntLike {
public:
	using size_type = BigInt::size_type;

public:
	explicit constexpr
	BigIntAbs(const BigInt& lhs) :
		_lhs(lhs) {}

	/**
	 * @return a new copy of this BigIntAbs.
	 */
	CONSTEXPR_AUTO
	copy() const noexcept -> BigIntAbs {
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return Sign::POS;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> size_type {
		return _lhs.size();
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const -> uint64_t {
		return _lhs[index];
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> std::span<const uint64_t> {
		return _lhs._span();
	}

	/**
	 * @return the underlying BigInt instance.
	 */
	CONSTEXPR_AUTO
	_bigint() const noexcept -> const BigInt& { return _lhs; }

private:
	const BigInt& _lhs;
};

class BigIntNeg: IBigIntLike {
public:
	using size_type = BigInt::size_type;

public:

	explicit constexpr
	BigIntNeg(const BigInt& lhs) :
	_lhs(lhs) {}

	/**
	 * @return a new copy of this BigIntNeg.
	 */
	CONSTEXPR_AUTO
	copy() const noexcept -> BigIntNeg {
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return _private::neg(_lhs.sign());
	}

	CONSTEXPR_AUTO
	size() const noexcept -> size_type {
		return _lhs.size();
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const -> uint64_t {
		return _lhs[index];
	}
	CONSTEXPR_AUTO
	_span() const noexcept -> std::span<const uint64_t> {
		return _lhs._span();
	}

	/**
	 * @return the underlying BigInt instance.
	 */
	CONSTEXPR_AUTO
	_bigint() const noexcept -> const BigInt& { return _lhs; }

private:
	const BigInt& _lhs;
};

class BigIntAbsNeg: IBigIntLike {
public:
	using size_type = BigInt::size_type;

public:
	explicit constexpr
	BigIntAbsNeg(const BigInt& lhs) :
	_lhs(lhs) {}

	/**
	 * @return a new copy of this BigIntAbsNeg.
	 */
	CONSTEXPR_AUTO
	copy() const noexcept -> BigIntAbsNeg {
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return Sign::NEG;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> size_type {
		return _lhs.size();
	}

	CONSTEXPR_AUTO
	operator[](size_type index) const -> uint64_t {
		return _lhs[index];
	}

	CONSTEXPR_AUTO
	_span() const noexcept -> std::span<const uint64_t> {
		return _lhs._span();
	}

	/**
	 * @return the underlying BigInt instance.
	 */
	CONSTEXPR_AUTO
	_bigint() const noexcept -> const BigInt& { return _lhs; }

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
	return _private::BigIntAbs(a._bigint());
}

CONSTEXPR_AUTO
abs(const _private::BigIntAbsNeg& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a._bigint());
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
operator-(const _private::BigIntNeg& a) -> const BigInt& {
	return a._bigint();
}

CONSTEXPR_AUTO
operator-(const _private::BigIntAbs& a) -> _private::BigIntAbsNeg {
	return _private::BigIntAbsNeg(a._bigint());
}

CONSTEXPR_AUTO
operator-(const _private::BigIntAbsNeg& a) -> _private::BigIntAbs {
	return _private::BigIntAbs(a._bigint());
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
namespace bigint::_private {

CONSTEXPR_AUTO
lshift_safe(uint64_t a, uint64_t b) {
	return (b >= 64) ? 0 : a << b; // shifting by 64 bits for 64 bit int is undefined.
}

CONSTEXPR_AUTO
rshift_safe(uint64_t a, uint64_t b) {
	return (b >= 64) ? 0 : a >> b; // shifting by 64 bits for 64 bit int is undefined.
}

BIGINT_TRACY_CONSTEXPR_VOID
lshift(const std::span<uint64_t>& result, const std::span<const uint64_t>& a, std::make_unsigned_t<BigInt::size_type> digits) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(!is_zero(a));
	assert(result.size() >= a.size() + digits / 64);

	const BigInt::size_type start = digits / 64;
	digits %= 64;

	const auto last = rshift_safe(a[a.size()-1], 64-digits);
	if (digits > 0 && last != 0) {
		result[a.size() + start] = last;
	}

	for (BigInt::size_type i = a.size(); i --> 1;) {
		const uint64_t lo = rshift_safe(a[i-1], 64-digits);
		const uint64_t hi = a[i] << digits;
		result[i + start] = lo | hi;
	}
	result[start] = a[0] << digits;

	std::fill_n(result.begin(), start, 0);
}

BIGINT_TRACY_CONSTEXPR_VOID
rshift(const std::span<uint64_t>& result, const std::span<const uint64_t>& a, std::make_unsigned_t<BigInt::size_type> digits) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(!is_zero(a));
	assert(a.size() > digits / 64);
	assert(result.size() >= a.size() - digits / 64);

	const BigInt::size_type start = digits / 64;
	digits %= 64;

	for (BigInt::size_type i = start; i < a.size()-1; ++i) {
		const uint64_t lo = a[i] >> digits;
		const uint64_t hi = lshift_safe(a[i+1], 64-digits);
		result[i - start] = lo | hi;
	}

	const auto last = a[a.size()-1] >> digits;
	result[a.size() - start - 1] = last;
}

}

// bitwise shift operations:
namespace bigint {

/**
 * @brief shifts the bits of `a` to the left by `n` bits. Supports assignment operations `lshift(a, a, n)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param n how far to move the bits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
lshift(BigInt& result, const is_BigInt_like auto &a, std::make_unsigned_t<BigInt::size_type> n) {
	if (is_zero(a)) {
		result.resize(0);
		return;
	}
	result.sign() = a.sign();
	auto asize = a.size();
	result.resize(asize + n / 64 + 1, 0);
	_private::lshift(result._span(), a._span().first(asize), n);
	result.cleanup();
}

/**
 * @brief shifts the bits of `a` to the right by `n` bits. Supports assignment operations `rshift(a, a, n)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param n how far to move the bits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
rshift(BigInt& result, const is_BigInt_like auto &a, std::make_unsigned_t<BigInt::size_type> n) {
	const BigInt::size_type start = n / 64;
	if (is_zero(a) || a.size() <= start) {
		result.resize(0);
		return;
	}
	result.sign() = a.sign();
	result.resize(std::max(result.size(), a.size() - start));
	_private::rshift(result._span(), a._span(), n);
	result.resize(a.size() - start);
	result.cleanup();
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator<<(const is_BigInt_like auto &a, std::make_unsigned_t<BigInt::size_type> digits) -> BigInt {
	BigInt result;
	lshift(result, a, digits);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator>>(const is_BigInt_like auto &a, std::make_unsigned_t<BigInt::size_type> digits) -> BigInt {
	BigInt result;
	rshift(result, a, digits);
	return result;
}


BIGINT_TRACY_CONSTEXPR_AUTO
operator<<=(BigInt &a, std::make_unsigned_t<BigInt::size_type> digits) -> BigInt& {
	lshift(a, a, digits);
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator>>=(BigInt &a, std::make_unsigned_t<BigInt::size_type> digits) -> BigInt& {
	if (digits == 0) {
		return a;
	}
	rshift(a, a, digits);
	return a;
}

}


// comparison operators:
namespace bigint {

BIGINT_TRACY_CONSTEXPR_AUTO
operator<=>(const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) -> std::strong_ordering {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b)) {

		if (a.size() > b.size()) {
			for (BigInt::size_type i = a.size(); i --> b.size();) {
				if (a[i]) { return std::strong_ordering::greater; }
			}
		} else if (b.size() > a.size()) {
			for (BigInt::size_type i = b.size(); i --> a.size();) {
				if (b[i]) { return std::strong_ordering::less; }
			}
		}

		BigInt::size_type i;
		for (i = std::min(a.size(), b.size()); i --> 1 && a[i] == b[i];) {
			// do nothing
		}
		return a[i] <=> b[i];
	}

	return is_zero(a) ? std::strong_ordering::less : std::strong_ordering::greater;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator<=>(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> std::strong_ordering {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b) && a.sign() == b.sign()) {
		return a._span() <=> b._span();
	}

	return (is_neg(a) || is_pos(b)) ? std::strong_ordering::less : std::strong_ordering::greater;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator<(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) < 0;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator>(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) > 0;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator<=(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) <= 0;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator>=(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) >= 0;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator==(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) == 0;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator!=(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> bool {
	return (a <=> b) != 0;
}


CONSTEXPR_AUTO
operator<=>(const is_BigInt_like auto &a, std::integral auto b) -> std::strong_ordering {
	if (a.size() > 1) {
		return is_neg(a) ? std::strong_ordering::less : std::strong_ordering::greater;
	} else if (is_neg(a)) {
		return (b < 0) ? a[0] <=> static_cast<uint64_t>(-b) : std::strong_ordering::less;
	} else {
		return (b < 0) ? std::strong_ordering::greater : a[0] <=> static_cast<uint64_t>(b);
	}
}

CONSTEXPR_AUTO
operator<(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) < 0;
}

CONSTEXPR_AUTO
operator>(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) > 0;
}

CONSTEXPR_AUTO
operator<=(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) <= 0;
}

CONSTEXPR_AUTO
operator>=(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) >= 0;
}

CONSTEXPR_AUTO
operator==(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) == 0;
}

CONSTEXPR_AUTO
operator!=(const is_BigInt_like auto &a, std::integral auto b) -> bool {
	return (a <=> b) != 0;
}

}



// bitwise AND, OR, XOR ignoring sign:
namespace bigint::_private {

/**
 * @brief bitwise-ANDs two integers ignoring their sign. `a.size()` *must* be equal or greater than `b.size()`. Supports assignment operations `bitwise_and_ignore_sign(a, a, b)` or even `bitwise_and_ignore_sign(a, a, a)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= min(a.size(), b.size())`.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_and_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	assert(a.size() >= b.size());
	assert(result.size() >= b.size());

	uint64_t i = 0;
	for (; i < b.size(); ++i) {
		result[i] = a[i] & b[i];
	}

	for (; i < result.size(); ++i) {
		result[i] = 0;
	}
}

/**
 * @brief bitwise-ORs two integers ignoring their sign. `a.size()` *must* be equal or greater than `b.size()`. Supports assignment operations `bitwise_or_ignore_sign(a, a, b)` or even `bitwise_or_ignore_sign(a, a, a)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_or_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	assert(a.size() >= b.size());
	assert(result.size() >= a.size());

	uint64_t i = 0;
	for (; i < b.size(); ++i) {
		result[i] = a[i] | b[i];
	}

	for (; i < a.size(); ++i) {
		result[i] = a[i];
	}

	for (; i < result.size(); ++i) {
		result[i] = 0;
	}
}

/**
 * @brief bitwise-XORs two integers ignoring their sign. `a.size()` *must* be equal or greater than `b.size()`. Supports assignment operations `bitwise_xor_ignore_sign(a, a, b)` or even `bitwise_xor_ignore_sign(a, a, a)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_xor_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	assert(a.size() >= b.size());
	assert(result.size() >= a.size());

	uint64_t i = 0;
	for (; i < b.size(); ++i) {
		result[i] = a[i] ^ b[i];
	}

	for (; i < a.size(); ++i) {
		result[i] = a[i];
	}

	for (; i < result.size(); ++i) {
		result[i] = 0;
	}
}

}


// bitwise AND, OR, XOR operations:
namespace bigint {

/**
 * @brief bitwise-ANDs two integers. Supports assignment operations `bitwise_and(a, a, b)` or even `bitwise_and(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_and(BigInt& result, is_BigInt_like auto& a, const is_BigInt_like auto& b) {
	result.sign() = is_neg(a) && is_neg(b) ? Sign::NEG : Sign::POS;

	result.resize(std::min(a.size(), b.size()));
	if (b.size() > a.size()) { // put the number with more digits first.
		_private::bitwise_and_ignore_sign(result._span(), b._span(), a._span());
	} else {
		_private::bitwise_and_ignore_sign(result._span(), a._span(), b._span());
	}
	result.cleanup();
}

/**
 * @brief bitwise-ORs two integers. Supports assignment operations `bitwise_or(a, a, b)` or even `bitwise_or(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_or(BigInt& result, is_BigInt_like auto& a, const is_BigInt_like auto& b) {
	result.sign() = is_neg(a) || is_neg(b) ? Sign::NEG : Sign::POS;

	const auto a_size = a.size();
	result.resize(std::max(a.size(), b.size()));
	const auto a_span = a._span().first(a_size);
	if (b.size() > a_size) { // put the number with more digits first.
		_private::bitwise_or_ignore_sign(result._span(), b._span(), a_span);
	} else {
		_private::bitwise_or_ignore_sign(result._span(), a_span, b._span());
	}
	result.cleanup();
}

/**
 * @brief bitwise-XORs two integers. Supports assignment operations `bitwise_xor(a, a, b)` or even `bitwise_xor(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
bitwise_xor(BigInt& result, is_BigInt_like auto& a, const is_BigInt_like auto& b) {
	result.sign() = is_neg(a) ^ is_neg(b) ? Sign::NEG : Sign::POS;

	const auto a_size = a.size();
	result.resize(std::max(a.size(), b.size()));
	const auto a_span = a._span().first(a_size);
	if (b.size() > a_size) { // put the number with more digits first.
		_private::bitwise_xor_ignore_sign(result._span(), b._span(), a_span);
	} else {
		_private::bitwise_xor_ignore_sign(result._span(), a_span, b._span());
	}
	result.cleanup();
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator&(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	bitwise_and(result, const_cast<decltype(a)&>(a), b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator&=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	bitwise_and(a, a, b);
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator|(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	bitwise_or(result, const_cast<decltype(a)&>(a), b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator|=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	bitwise_or(a, a, b);
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator^(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	bitwise_xor(result, const_cast<decltype(a)&>(a), b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator^=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	bitwise_xor(a, a, b);
	return a;
}

}

// add / sub ignoring sign:
namespace bigint::_private {

/**
 * @brief adds two integers ignoring their sign. `a.size()` *must* be equal or greater than `b.size()`. Supports assignment operations `_add_ignore_sign(a, a, b)` or even `_add_ignore_sign(a, a, a)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`, if we know that there is no overflow, otherwise `result.size() > max(a.size(), b.size())`.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_add_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(a.size() >= b.size());
	assert(result.size() >= a.size());

	bool c = false; // carry
	BigInt::size_type i = 0;
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
 * @brief subtracts `b` from `a` ignoring their sign. `abs(a)` *must* be equal or greater than `abs(b)`. Supports assignment operations `_sub_ignore_sign_no_negative_result(a, a, b)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_sub_ignore_sign_no_negative_result(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(result.size() >= std::max(a.size(), b.size()));

	bool c = false; // carry
	BigInt::size_type i = 0;
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
		if (!is_zero(rshifted(b, a.size()))) {
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
 * @brief adds two integers ignoring their sign. Supports assignment operations `add_ignore_sign(a, a, b)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`, if we know that there is no overflow, otherwise `result.size() > max(a.size(), b.size())`.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
add_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	if (b.size() > a.size()) { // put the number with more digits first.
		_add_ignore_sign(result, b, a);
	} else {
		_add_ignore_sign(result, a, b);
	}
}

/**
 * @brief subtracts `b` from `a` ignoring their sign. Supports assignment operations `sub_ignore_sign(a, a, b)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
sub_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) -> Sign {
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

/**
 * @brief adds two integers. Supports assignment operations `add(a, a, b)` or even `add(a, a, a)`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`, if we know that there is no overflow, otherwise `result.size() > max(a.size(), b.size())`.
 * @param a the first operand.
 * @param b the second operand.
 * @param a_sign sign of the first operand.
 * @param b_sign sign of the second operand.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
add(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b, const Sign a_sign, const Sign b_sign) -> Sign {
	if (a_sign == b_sign) {
		add_ignore_sign(result, a, b);
		return a_sign;
	}
	if (a_sign == Sign::POS) {
		return sub_ignore_sign(result, a, b);
	}
	return sub_ignore_sign(result, b, a);
}

}


// addition:
namespace bigint {

/**
 * @brief adds `a` to `b`. Supports assignment operations `add(a, a, b)` or even `add(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
add(BigInt &result, is_BigInt_like auto &a, const is_BigInt_like auto &b) {
	const auto a_size = a.size();
	result.resize(std::max(a.size(), b.size()) + 1);
	const auto a_span = a._span().first(a_size);

	result.sign() = _private::add(result._span(), a_span, b._span(), a.sign(), b.sign());
	result.cleanup();
}

/**
 * @brief adds `a` to `b`. Supports assignment operations `add(a, a, b)` or even `add(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
add(BigInt &result, is_BigInt_like auto &a, const std::integral auto &b) {
	add(result, a, _private::IntegralAdapter{b});
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const is_BigInt_like auto &a, std::integral auto b) -> BigInt {
	return a + _private::IntegralAdapter{b};
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator+(std::integral auto a, const is_BigInt_like auto &b) -> BigInt {
	return b + _private::IntegralAdapter{a};
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	add(result, const_cast<decltype(a)&>(a), b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator+=(BigInt &a, std::integral auto b) -> BigInt& {
	add(a, a, _private::IntegralAdapter{b});
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator+=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	add(a, a, b);
	return a;
}

}


// subtraction:
namespace bigint {


/**
 * @brief subtracts `b` from `a`. Supports assignment operations `sub(a, a, b)` or even `sub(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
sub(BigInt &result, is_BigInt_like auto &a, const is_BigInt_like auto &b) {
	const auto a_size = a.size();
	result.resize(std::max(a.size(), b.size()) + 1);
	const auto a_span = a._span().first(a_size);

	result.sign() = _private::add(result._span(), a_span, b._span(), a.sign(), _private::neg(b.sign()));
	result.cleanup();
}

/**
 * @brief subtracts `b` from `a`. Supports assignment operations `add(a, a, b)` or even `add(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
sub(BigInt &result, is_BigInt_like auto &a, const std::integral auto &b) {
	sub(result, a, _private::IntegralAdapter{b});
}

/**
 * @brief subtracts `b` from `a`. Supports assignment operations `add(a, a, b)` or even `add(a, a, a)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
sub(BigInt &result, std::integral auto &a, const is_BigInt_like auto &b) {
	auto a_ = _private::IntegralAdapter{a};
	sub(result, a_, b);
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const is_BigInt_like auto &a, std::integral auto b) -> BigInt {
	return a - _private::IntegralAdapter{b};
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator-(std::integral auto a, const is_BigInt_like auto &b) -> BigInt {
	return _private::IntegralAdapter{a} - b;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	sub(result, const_cast<decltype(a)&>(a), b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator-=(BigInt &a, std::integral auto b) -> BigInt& {
	sub(a, a, _private::IntegralAdapter{b});
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator-=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	sub(a, a, b);
	return a;
}


}


// multiplication uint64_t x uint64_t:
namespace bigint {

struct MultResult {
	uint64_t lo, hi;

	[[nodiscard]] explicit constexpr
	operator BigInt() const {
		BigInt result{};
		assert(result.size() == 1);
		result.set(0, lo);
		result.append(hi);
		result.cleanup();
		return result;
	}
};

/**
 * @brief multiples `a` and `b`.
 * @param a the first operand.
 * @param b the second operand.
 * @return the two-digit result.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
mult(uint64_t a, uint64_t b) -> MultResult {
	MultResult result;
	utils::mul_u128(a, b, &result.hi, &result.lo);
	return result;
}

}

// multiplication helpers:
namespace bigint {
enum class MultiplicationAlgorithm {
	NAIVE = 0,
	KARATSUBA = 1,
};

/**
 * @brief heuristic to determine what multiplication algorithm to use.
 * @param a_size size of the first operand.
 * @param b_size size of the second operand.
 * @return the multiplication algorithm to use.
 */
CONSTEXPR_AUTO
determine_multiplication_algorithm(BigInt::size_type a_size, BigInt::size_type b_size) -> MultiplicationAlgorithm {
	const auto min_size = std::min(a_size, b_size);
	const auto max_size = std::max(a_size, b_size);
	bool const use_karatsuba = max_size >= _private::MIN_DIGITS_FOR_MULT_KARATSUBA && (min_size >= _private::MIN_DIGITS_FOR_MULT_KARATSUBA || min_size > max_size >> 1);
	return use_karatsuba ? MultiplicationAlgorithm::KARATSUBA : MultiplicationAlgorithm::NAIVE;
}

/**
 * Holds temporaries used during Karatsuba multiplication.
 */
struct KaratsubaStepTemps {
	DigitsVecPtr ac;
	DigitsVecPtr bd;
	DigitsVecPtr ab_cd;
	DigitsVecPtr a_b;
	DigitsVecPtr c_d;

	std::unique_ptr<KaratsubaStepTemps> local_temps;

	CONSTEXPR_AUTO
	ac_span() -> std::span<uint64_t> { return std::span<uint64_t>{*ac}; };
	CONSTEXPR_AUTO
	bd_span() -> std::span<uint64_t> { return std::span<uint64_t>{*bd}; };
	CONSTEXPR_AUTO
	ab_cd_span() -> std::span<uint64_t> { return std::span<uint64_t>{*ab_cd}; };
	CONSTEXPR_AUTO
	a_b_span() -> std::span<uint64_t> { return std::span<uint64_t>{*a_b}; };
	CONSTEXPR_AUTO
	c_d_span() -> std::span<uint64_t> { return std::span<uint64_t>{*c_d}; };
};

}

// multiplication:
namespace bigint::_private {

CONSTEXPR_AUTO
mult_sign(Sign a, Sign b) -> Sign {
	return a != b ? Sign::NEG : Sign::POS;
}


/**
 * @brief multiples `a` and `b`. Supports assignment operations `_mult_naive_ignore_sign(a, a, b)`.
 * @param result the result will be put in here. Size requirement: `result.size() == a.size() + 1`.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_mult_naive_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, uint64_t b) {
	assert(result.size() >= a.size() + 1);
	uint64_t c = 0; // carry
	BigInt::size_type i = 0;
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

/**
 * @brief multiples `a` and `b` ignoring their signs and adds that to `result`:  `result += a * b;`
 * @param result the result will be put in here. Size requirement: `result.size() == a.size() + 1`.
 * @param a the first operand
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_addmul_naive_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, uint64_t b) {
	assert(result.size() == a.size() + 1);

	uint64_t c = 0; // carry
	BigInt::size_type i = 0;
	for (; i < a.size(); i++) {
		const auto rc = mult(a[i], b);
		auto mult_i = rc.lo + c;
		c = rc.hi + (mult_i < c ? 1 : 0); // account for addition overflow
		auto result_i = result[i] + mult_i;
		c += (result_i < mult_i ? 1 : 0); // account for addition overflow
		result[i] = result_i;
	}

	// handle last multiplication carry
	result[i] = c;
}

/**
 * @brief multiples `a` and `b` ignoring their signs.
 * @param result the result will be put in here. Size requirement: `result.size() == a.size() + b.size()`.
 * @param a the operand with the most digits.
 * @param b the operand with the least digits.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_mult_naive_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a, const std::span<const uint64_t> &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(result.size() == a.size() + b.size());

	// first iteration step (performed inline):
	_mult_naive_ignore_sign(rmasked(result, 0, a.size() + 1), a, b[0]);

	// all other iteration steps:
	for (BigInt::size_type i = 1; i < b.size(); i++) {
		_addmul_naive_ignore_sign(rmasked(result, i, a.size() + 1), a, b[i]);
	}
}

// forward declaration:
BIGINT_TRACY_CONSTEXPR_VOID
mult_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a_, const std::span<const uint64_t> &b_, std::unique_ptr<KaratsubaStepTemps>& karatsuba_temps);

/**
 * @brief multiplies two integers ignoring their sign using the Karatsuba algorithm. `a.size()` *must* be equal or greater than `b.size()`.
 * @param result the result will be put in here. Size requirement: `result.size() >= max(a.size(), b.size())`, if we know that there is no overflow, otherwise `result.size() > max(a.size(), b.size())`.
 * @param lhs the operand with the most digits.
 * @param rhs the operand with the least digits.
 * @param temps temporaries for karatsuba multiplication.
 */
BIGINT_TRACY_CONSTEXPR_VOID
_mult_karatsuba_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &lhs, const std::span<const uint64_t> &rhs, KaratsubaStepTemps& temps) {
	BIGINT_TRACY_ZONE_SCOPED;
	// xx = mm(ac) + m((a+b) * (c+d) - ac - bd) + (bd)
	assert(lhs.size() >= rhs.size());

	if (!temps.local_temps) {
		temps.local_temps = std::make_unique<KaratsubaStepTemps>();
	}

	auto n = std::max(lhs.size(), rhs.size());
	const auto mid = n >> 1;

	const auto a = rmasked(lhs, mid, lhs.size());
	const auto b = rmasked(lhs, 0, mid);
	const auto c = rmasked(rhs, mid, rhs.size());
	const auto d = rmasked(rhs, 0, mid);

	auto &local_temps = temps.local_temps;

	temps.ac->resize(a.size() + c.size());
	mult_ignore_sign(temps.ac_span(), a, c, local_temps);
	cleanup(*temps.ac);

	temps.bd->resize(b.size() + d.size());
	mult_ignore_sign(temps.bd_span(), b, d, local_temps);
	cleanup(*temps.bd);


	temps.a_b->resize(std::max(a.size(), b.size()) + 1);
	add_ignore_sign(temps.a_b_span(), a, b);
	cleanup(*temps.a_b);

	temps.c_d->resize(std::max(c.size(), d.size()) + 1);
	add_ignore_sign(temps.c_d_span(), c, d);
	cleanup(*temps.c_d);

	temps.ab_cd->resize(temps.a_b->size() + temps.c_d->size());
	mult_ignore_sign(temps.ab_cd_span(), temps.a_b_span(), temps.c_d_span(), local_temps);
	cleanup(*temps.ab_cd);

	[[maybe_unused]] auto sign = sub_ignore_sign(temps.ab_cd_span(), temps.ab_cd_span(), temps.ac_span());
	cleanup(*temps.ab_cd);
	[[maybe_unused]] auto sign2 = sub_ignore_sign(temps.ab_cd_span(), temps.ab_cd_span(), temps.bd_span());
	cleanup(*temps.ab_cd);

	std::ranges::copy(*temps.bd, result.begin());
	std::fill(result.begin() + temps.bd->size(), result.end(), 0);

	const auto result_shifted1 = rshifted(result, mid);
	_add_ignore_sign(result_shifted1, result_shifted1, rmasked(temps.ab_cd_span(), 0, result_shifted1.size()));

	const auto result_shifted2 = rshifted(result, mid << 1);
	_add_ignore_sign(result_shifted2, result_shifted2, rmasked(temps.ac_span(), 0, result_shifted2.size()));
}


/**
 * @brief shortcuts for multiplication.
 *
* @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 *
 * @return `true` if a shortcut was taken successfully.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
_mult_ignore_sign_shortcuts(const std::span<uint64_t> &result, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b) -> bool {
	if (is_zero(a) || is_zero(b)) {
		std::ranges::fill(result, 0);
		return true;
	}
	return false;
}

/**
 * @brief shortcuts for multiplication.
 *
* @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 *
 * @return `true` if a shortcut was taken successfully.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
_mult_ignore_sign_shortcuts(BigInt &result, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b) -> bool {
	if (is_zero(a) || is_zero(b)) {
		result.resize(0);
		return true;
	}
	return false;
}


/**
 * @brief multiples `a` and `b`.
 * @param result the result will be put in here. Size requirement: `result.size() == a.size() + b.size()`.
 * @param a_ the first operand.
 * @param b_ the second operand.
 * @param karatsuba_temps temporaries for karatsuba multiplication. Can be a nullptr. Will be filled only if needed.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult_ignore_sign(const std::span<uint64_t> &result, const std::span<const uint64_t> &a_, const std::span<const uint64_t> &b_, std::unique_ptr<KaratsubaStepTemps>& karatsuba_temps) {
	if (_mult_ignore_sign_shortcuts(result, a_, b_)) {
		return;
	}

	auto [a, b] = a_.size() >= b_.size() ? std::tie(a_, b_) : std::tie(b_, a_);
	assert(a.size() >= b.size());

	switch (determine_multiplication_algorithm(a.size(), b.size())) {
	case MultiplicationAlgorithm::NAIVE:
		_mult_naive_ignore_sign(result, a, b);
		return;
	case MultiplicationAlgorithm::KARATSUBA:
		if (!karatsuba_temps) {
			karatsuba_temps = std::make_unique<KaratsubaStepTemps>();
		}
		_mult_karatsuba_ignore_sign(result, a, b, *karatsuba_temps);
		return;
	}
}


}

// multiplication:
namespace bigint {

/**
 * @brief multiples `a` and `b`. Supports assignment operations `mult(a, a, b)`.
 * @param result the result will be put in here. can be the same address as `a`.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult(BigInt &result, is_BigInt_like auto &a, std::unsigned_integral auto b) {
	if (b == 0 || is_zero(a)) {
		result.resize(0);
		return;
	}
	const auto a_size = a.size();
	result.resize(a.size() + 1);
	const auto a_span = a._span().subspan(0, a_size);
	_private::_mult_naive_ignore_sign(result._span(), a_span, b);
	result.sign() = a.sign();
	result.cleanup();
}

/**
 * @brief multiples `a` and `b`. Supports assignment operations `mult(a, a, b)`.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult(BigInt &result, is_BigInt_like auto &a, std::signed_integral auto b) {
	mult(result, a, static_cast<uint64_t>(llabs(b)));
	if (b < 0) {
		result.sign() = _private::neg(result.sign());
	}
}

/**
 * @brief multiples two integers using the naïve multiplication algorithm.
 * @param result the result will be put in here.
 * @param lhs the first operand.
 * @param rhs the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult_naive(BigInt &result, const is_BigInt_like auto &lhs, const is_BigInt_like auto &rhs) {
	assert_release_msg(&result != &lhs._bigint(), "result and first argument must be separate instances");
	assert_release_msg(&result != &rhs._bigint(), "result and second argument must be separate instances");
	if (_private::_mult_ignore_sign_shortcuts(result, lhs._span(), rhs._span())) {
		return;
	}
	result.resize(lhs.size() + rhs.size());
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
	if (rhs.size() > lhs.size()) { // put the number with more digits first.
		_private::_mult_naive_ignore_sign(result._span(), rhs._span(), lhs._span());
	} else {
		_private::_mult_naive_ignore_sign(result._span(), lhs._span(), rhs._span());
	}
	result.cleanup();
}

/**
 * @brief multiples two integers using the Karatsuba multiplication algorithm.
 * @param result the result will be put in here.
 * @param lhs the first operand.
 * @param rhs the second operand.
 * @param temps temporaries used by the algorithm.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult_karatsuba(BigInt &result, const is_BigInt_like auto &lhs, const is_BigInt_like auto &rhs, KaratsubaStepTemps& temps) {
	assert_release_msg(&result != &lhs._bigint(), "result and first argument must be separate instances");
	assert_release_msg(&result != &rhs._bigint(), "result and second argument must be separate instances");
	if (_private::_mult_ignore_sign_shortcuts(result, lhs._span(), rhs._span())) {
		return;
	}
	result.resize(lhs.size() + rhs.size());
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
	if (rhs.size() > lhs.size()) { // put the number with more digits first.
		_private::_mult_karatsuba_ignore_sign(result._span(), rhs._span(), lhs._span(), temps);
	} else {
		_private::_mult_karatsuba_ignore_sign(result._span(), lhs._span(), rhs._span(), temps);
	}
	result.cleanup();
}

/**
 * @brief multiples two integers.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 * @param karatsuba_temps temporaries for karatsuba multiplication. Can be a nullptr. Will be filled only if needed.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult(BigInt &result, const is_BigInt_like auto &a, const is_BigInt_like auto &b, std::unique_ptr<KaratsubaStepTemps>& karatsuba_temps) {
	assert_release_msg(&result != &a._bigint(), "result and first argument must be separate instances");
	assert_release_msg(&result != &b._bigint(), "result and second argument must be separate instances");
	result.resize(a.size() + b.size());
	_private::mult_ignore_sign(result._span(), a._span(), b._span(), karatsuba_temps);
	result.sign() = _private::mult_sign(a.sign(), b.sign());
	result.cleanup();
}


/**
 * @brief multiples two integers.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mult(is_BigInt_like auto &result, const is_BigInt_like auto &a, const is_BigInt_like auto &b) {
	std::unique_ptr<KaratsubaStepTemps> temps;
	mult(result, a, b, temps);
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator*(const is_BigInt_like auto &a, std::integral auto b) -> BigInt {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<decltype(b)>, uint64_t, int64_t>;
	BigInt result;
	mult(result, const_cast<decltype(a)&>(a), static_cast<TRHS2>(b));
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator*(std::integral auto a, const is_BigInt_like auto &b) -> BigInt {
	return b * a;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator*(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	mult(result, a, b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator*=(is_BigInt_like auto &a, std::integral auto b) -> decltype(a)& {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<decltype(b)>, uint64_t, int64_t>;
	mult(a, a, static_cast<TRHS2>(b));
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator*=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	BigInt result;
	mult(result, a, b);
	std::swap(a, result);
	return a;
}

}


// DivModResult:
namespace bigint {

template<class Q, class R=Q>
struct DivModResult {
	typedef Q QQ;
	typedef R RR;
	Q q; R r;
};

}


// divmod ignoring sign:
namespace bigint::_private {

BIGINT_TRACY_CONSTEXPR_AUTO
_correct_d_and_subtract(const std::span<uint64_t> &x, const std::span<const uint64_t> &b, uint64_t d, const std::span<uint64_t> &temp) -> uint64_t {
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
 * @param result the result will be put in here. \n
 *        Size requirement: `result.q.size() == a.size() - b.size() + 1` if `ignore_quotient==true`, otherwise there's no size requirement at all. \n
 *        Size requirement: `result.r.size() == a.size() + 1`.
 * @param a the dividend
 * @param b the divisor
 * @param e estimator (?) for the divisor
 * @param temp a temporary. Size requirement: `temp.size() == b.size() + 1`
 * @return
 */
template <bool ignore_quotient>
BIGINT_TRACY_CONSTEXPR_VOID
_divide_loop(const DivModResult<std::span<uint64_t>>& result, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b, uint64_t e, const std::span<uint64_t> temp) {
	const auto na = a.size();
	const auto nb = b.size();
	/* na >= nb holds. */
	assert(nb > 1); // single digit division must be handled by dedicated divmod_ignore_sign_small(...).
	if constexpr (!ignore_quotient) {
		/* quotient can have maximum (na-nb+1) digits */
		assert(result.q.size() == na - nb + 1);
	}
	assert(result.r.size() == na + 1);
	assert(temp.size() == nb + 1);

	std::ranges::copy(a, result.r.begin());
	result.r.back() = 0;
	auto x_span = result.r.subspan(na - nb + 1);

	/* loop-invariant P: first m digits of ’a’ have been brought-down and processed. */
	for (auto i = na - nb + 1; i --> 0;) {
		x_span = std::span{x_span.data() - 1, static_cast<BigInt::size_type>(x_span.size() + 1)};
		uint64_t d = utils::div_u128_saturate(x_span[nb], x_span[nb-1], e); // yz/e;
		d = _correct_d_and_subtract(x_span, b, d, temp);
		if constexpr (!ignore_quotient) {
			result.q[i] = d;
		}
	}
	/* (loop-invariant P) AND (m=na) holds. */
	/* Now x contains the remainder. */
}


/**
 * @brief division & modulo ignoring any sign. Supports assignment operations `divmod(a, a, b)`.
 * @param quotient the quotient will be put in here. Size requirement: `result.size() == a.size()`.
 * @param a the dividend. Can be the same span as the quotient of the result (`result.q.data() == a.data()`).
 * @param b the divisor. Either `uint32_t` or `uint64_t`.
 * @return the remainder
 */
template <bool ignore_quotient>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod_ignore_sign_small(const std::span<uint64_t> quotient, const std::span<const uint64_t>& a, one_of<uint32_t, uint64_t> auto b) -> decltype(b){
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(a)) {
		std::ranges::fill(quotient, 0);
		return 0;
	}

	decltype(b) x_lo = 0;
	for (auto i = a.size(); i --> 0;) {
		uint64_t qi;
		if constexpr (std::is_same_v<decltype(b), uint32_t>) {
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
			quotient[i] = qi;
		}
	}

	return x_lo;
}


template <bool ignore_quotient>
BIGINT_TRACY_CONSTEXPR_VOID
_resize_result_for_divide_loop(BigInt& quotient, BigInt& remainder, DigitsVec& temp, const BigInt::size_type na, const BigInt::size_type nb) {
	if constexpr (!ignore_quotient) {
		// quotient can have maximum (na-nb+1) digits
		quotient.reserve(na - nb + 2);
		quotient.resize(na - nb + 1);
	}
	remainder.resize(na + 1);
	temp.resize(nb + 1);
}


/**
 * @brief division algorithm adapted from Nitin Verma, 2021, Implementing Basic Arithmetic for Large Integers: Division
 *
 * @param quotient the quotient result, will be untouched if `ignore_quotient==true`. Max size requirement: `quotient.size() == a.size() - b.size() + 1` (only if `ignore_quotient==true`).
 * @param remainder the dividend result, will be touched even if `ignore_remainder==true`! Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 *
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_VOID
_divmod_ignore_sign_big(BigInt& quotient, BigInt& remainder, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	BIGINT_TRACY_ZONE_SCOPED;

	assert(!is_zero(b));
	assert(!(is_zero(a) || b.size() > a.size()));
	assert(b.size() > 1); // single digit division must be handled by dedicated divmod_ignore_sign_small(...).

	uint64_t e = b.back();
	if (e < 1ull<<63) {
		/* normalization */
		const uint64_t f = utils::div_u128_saturate(1ull, 0ull, e + 1); // 1^64/(e + 1);

		auto& af = temp_af;
		af.resize(a.size() + 1);
		_mult_naive_ignore_sign(std::span<uint64_t>{af}, a, f);
		cleanup(af);

		auto& bf = temp_bf;
		bf.resize(b.size() + 1);
		_mult_naive_ignore_sign(std::span<uint64_t>{bf}, b, f);
		cleanup(bf);

		e = bf.back();

		_resize_result_for_divide_loop<ignore_quotient>(quotient, remainder, temp, af.size(), bf.size());
		_divide_loop<ignore_quotient>(DivModResult{quotient._span(), remainder._span()}, std::span<uint64_t>{af}, std::span<uint64_t>{bf}, e, std::span<uint64_t>{temp});

		if constexpr (!ignore_remainder) { // fix remainder:
			[[maybe_unused]] auto rm = divmod_ignore_sign_small<false>(remainder._span(), remainder._span(), f);
		}
	} else {
		_resize_result_for_divide_loop<ignore_quotient>(quotient, remainder, temp, a.size(), b.size());
		_divide_loop<ignore_quotient>(DivModResult{quotient._span(), remainder._span()}, a, b, e, std::span<uint64_t>{temp});
	}

	if constexpr (!ignore_quotient) {
		quotient.cleanup();
	}
	remainder.cleanup(); // remainder is also used by quotient.
}


/**
 * @brief shortcuts for division.
 *
 * @param quotient the quotient result, will be untouched if `ignore_quotient==true`. Size requirement: `quotient.size() == a.size() - b.size() + 1` (only if `ignore_quotient==true`).
 * @param remainder the dividend result, will be touched even if `ignore_remainder==true`! Size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 *
 * @return `true` if a shortcut was taken successfully.
 */
template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_AUTO
_divmod_ignore_sign_big_shortcuts(BigInt& quotient, BigInt& remainder, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b) -> bool {
	assert(!is_zero(b));

	if (is_zero(a) || b.size() > a.size()) {
		if constexpr (!ignore_quotient) {
			quotient.resize(0);
		}

		if constexpr (!ignore_remainder) {
			remainder.resize(a.size());
			std::ranges::copy(a, remainder._span().begin());
		} else {
			remainder.resize(1);
			// remainder could be any positive number. It's only used to signify that the remainder is non-zero.
			// Used for correcting a negative quotient in divmod(...)
			remainder.set(0, is_zero(a) ? 0 : 1);
		}
		return true;
	}
	if (b.size() == 1) {
		if constexpr (!ignore_quotient) {
			quotient.reserve(a.size() + 1);
			quotient.resize(a.size());
		}
		remainder.resize(1);
		remainder.set(0, divmod_ignore_sign_small<ignore_quotient>(quotient._span(), a, b[0]));
		return true;
	}

	return false;
}


/**
 * @brief division with remainder ignoring sign.
 *
 * @param quotient the quotient result, will be untouched if `ignore_quotient==true`. Max size requirement: `quotient.size() == a.size() - b.size() + 1` (only if `ignore_quotient==true`).
 * @param remainder the dividend result, will be touched even if `ignore_remainder==true`! Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
*
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_VOID
divmod_ignore_sign(BigInt& quotient, BigInt& remainder, const std::span<const uint64_t>& a, const std::span<const uint64_t>& b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(b)) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (_divmod_ignore_sign_big_shortcuts<ignore_quotient, ignore_remainder>(quotient, remainder, a, b)) {
		return;
	}
	_divmod_ignore_sign_big<ignore_quotient, ignore_remainder>(quotient, remainder, a, b, temp, temp_af, temp_bf);
}


template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_VOID
_fix_divmod_signs(BigInt& quotient, BigInt& remainder, const Sign a_sign, const is_BigInt_like auto &b) {
	if constexpr (!ignore_remainder) {
		if (!is_zero(remainder)) {
			remainder.sign() = a_sign;
			if (a_sign != b.sign()) {
				remainder += b;
			}
		}
	}
	if constexpr (!ignore_quotient) {
		quotient.sign() = _private::mult_sign(a_sign, b.sign());
		if (!is_zero(remainder) and quotient.sign() == Sign::NEG) {
			quotient -= 1;
		}
		quotient.cleanup();
	}
}


template <bool ignore_quotient, bool ignore_remainder, one_of<int32_t, uint32_t, int64_t, uint64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
_fix_divmod_signs(BigInt& quotient, TRHS& remainder, const Sign a_sign, const TRHS &b) {
	if constexpr (!ignore_remainder) {
		if (remainder != 0) {
			if (a_sign == Sign::NEG) {
				remainder = -remainder;
			}
			if (a_sign != _private::get_sign(b)) {
				remainder += b;
			}
		}
	}
	if constexpr (!ignore_quotient) {
		quotient.sign() = _private::mult_sign(a_sign, _private::get_sign(b));
		if (remainder != 0 and quotient.sign() == Sign::NEG) {
			quotient -= 1;
		}
		quotient.cleanup();
	}
}

/**
 * @brief division with remainder. Supports assignment operations `divmod(a, a, b)`.
 *
 * @param quotient the quotient result, will be untouched if `ignore_quotient==true`. Max size requirement: `quotient.size() == a.size() - b.size() + 1` (only if `ignore_quotient==true`).
 * @param a the dividend
 * @param b the divisor
 * @return the remainder
 */
template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(BigInt& quotient, const is_BigInt_like auto &a, one_of<int32_t, uint32_t, int64_t, uint64_t> auto b) -> decltype(b) {
	if constexpr (!ignore_quotient) {
		quotient.reserve(a.size() + 1);
		quotient.resize(a.size());
	}
	decltype(b) remainder = _private::divmod_ignore_sign_small<ignore_quotient>(quotient._span(), a._span(), utils::constexpr_abs(b));
	_private::_fix_divmod_signs<ignore_quotient, ignore_remainder>(quotient, remainder, a.sign(), b);
	return remainder;
}


/**
 * @brief integer division with remainder. Does NOT support assignment operations.
 *
 * @param quotient the quotient result, will be untouched if `ignore_quotient==true`. Max size requirement: `quotient.size() == a.size() - b.size() + 1` (only if `ignore_quotient==true`).
 * @param remainder the dividend result, will be touched even if `ignore_remainder==true`! Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 *
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
template <bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_VOID
divmod(BigInt& quotient, BigInt& remainder, const is_BigInt_like auto &a, const is_BigInt_like auto &b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	if constexpr (!ignore_quotient) {
		assert_release_msg(&quotient != &a._bigint(), "quotient and first argument must be separate instances");
		assert_release_msg(&quotient != &b._bigint(), "quotient and second argument must be separate instances");
		assert_release_msg(&quotient != &remainder, "remainder and second argument must be separate instances");
	}
	assert_release_msg(&remainder != &a._bigint(), "remainder and first argument must be separate instances");
	assert_release_msg(&remainder != &b._bigint(), "remainder and second argument must be separate instances");

	_private::divmod_ignore_sign<ignore_quotient, ignore_remainder>(quotient, remainder, a._span(), b._span(), temp, temp_af, temp_bf);
	_private::_fix_divmod_signs<ignore_quotient, ignore_remainder>(quotient, remainder, a.sign(), b);
}

}


// divmod:
namespace bigint {

/**
 * @brief integer division with remainder. Supports assignment operations `divmod(a, a, b)`.
 *
 * @param quotient the quotient result. Max size requirement: `quotient.size() == a.size() - b.size() + 1`.
 * @param a the dividend
 * @param b the divisor
 * @return the remainder
 */
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(BigInt& quotient, const is_BigInt_like auto &a, one_of<int32_t, uint32_t, int64_t, uint64_t> auto b) -> decltype(b) {
	return _private::divmod<false, false>(quotient, a, b);
}

/**
 * @brief integer division with remainder.
 *
 * @param a the dividend
 * @param b the divisor
 * @return the quotient and remainder
 */
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const is_BigInt_like auto &a, one_of<int32_t, uint32_t, int64_t, uint64_t> auto b) -> DivModResult<BigInt, decltype(b)> {
	DivModResult<BigInt, decltype(b)> result;
	result.r = divmod(result.q, a, b);
	return result;
}


/**
 * @brief integer division with remainder. Does NOT support assignment operations.
 *
 * @param quotient the quotient result. Max size requirement: `quotient.size() == a.size() - b.size() + 1`.
 * @param remainder the dividend result. Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 *
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
BIGINT_TRACY_CONSTEXPR_VOID
divmod(BigInt& quotient, BigInt& remainder, const is_BigInt_like auto &a, const is_BigInt_like auto &b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	_private::divmod<false, false>(quotient, remainder, a, b, temp, temp_af, temp_bf);
}

/**
 * @brief integer division with remainder.
 *
 * @param a the dividend
 * @param b the divisor
 * @return the quotient and remainder
 */
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> DivModResult<BigInt> {
	DivModResult<BigInt> result;
	DigitsVecPtr temp, temp_af, temp_bf;
	_private::divmod<false, false>(result.q, result.r, a, b, *temp, *temp_af, *temp_bf);
	return result;
}

}


// division:
namespace bigint {

/**
 * @brief integer division. Supports assignment operations `div(a, a, b)`.
 *
 * @param result the quotient result.
 * @param a the dividend
 * @param b the divisor
 */
BIGINT_TRACY_CONSTEXPR_VOID
div(BigInt &result, const is_BigInt_like auto &a, one_of<int32_t, uint32_t, int64_t, uint64_t> auto b) {
	[[maybe_unused]]auto remainder = _private::divmod<false, true>(result, a, b);
}

/**
 * @brief integer division. Does NOT support assignment operations.
 *
 * @param result the quotient result. Max size requirement: `quotient.size() == a.size() - b.size() + 1`.
 * @param a the dividend
 * @param b the divisor
 *
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
BIGINT_TRACY_CONSTEXPR_VOID
div(BigInt& result, const is_BigInt_like auto &a, const is_BigInt_like auto &b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	BigInt remainder;
	_private::divmod<false, true>(result, remainder, a, b, temp, temp_af, temp_bf);
}

/**
 * @brief integer division. Does NOT support assignment operations.
 *
 * @param result the quotient result. Max size requirement: `quotient.size() == a.size() - b.size() + 1`.
 * @param a the dividend
 * @param b the divisor
 */
BIGINT_TRACY_CONSTEXPR_VOID
div(BigInt& result, const is_BigInt_like auto &a, const is_BigInt_like auto &b) {
	BigInt remainder;
	DigitsVecPtr temp, temp_af, temp_bf;
	_private::divmod<false, true>(result, remainder, a, b, *temp, *temp_af, *temp_bf);
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const is_BigInt_like auto &a, one_of<uint32_t, int32_t, uint64_t, int64_t> auto b) -> BigInt {
	BigInt result;
	div(result, a, b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	div(result, a, b);
	return result;
}


BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, one_of<uint32_t, int32_t, uint64_t, int64_t> auto b) -> BigInt& {
	div(a, a, b);
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	if (b.size() == 1) { // we can perform division inplace:
		auto remainder = divmod(a, a, b[0]);
		a.sign() = _private::mult_sign(a.sign(), b.sign());
		// correct wrong corrections, caused by wrong sign of `b` supplied above:
		if (remainder != 0 && b.sign() == Sign::NEG) {
			a -= 1;
		}
	} else { // we cannot do it inplace:
		BigInt result;
		div(result, a, b);
		a = std::move(result);
	}
	return a;
}

}


// Modulo:
namespace bigint {

/**
 * @brief integer modulo.
 *
 * @param result the modulo result. Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 */
template <one_of<int32_t, uint32_t, int64_t, uint64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mod(TRHS &result, const is_BigInt_like auto &a, TRHS b) {
	BigInt quotient;
	result = _private::divmod<true, false>(quotient, a, b);
}

/**
 * @brief integer modulo.
 *
 * @param result the modulo result. Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 *
 * @param temp a temporary. Max size requirement: `temp.size() == b.size() + 2`.
 * @param temp_af a temporary. Max size requirement: `temp_af.size() == a.size() + 1`.
 * @param temp_bf a temporary. Max size requirement: `temp_bf.size() == b.size() + 1`.
 */
BIGINT_TRACY_CONSTEXPR_VOID
mod(BigInt& result, const is_BigInt_like auto &a, const is_BigInt_like auto &b, DigitsVec& temp, DigitsVec& temp_af, DigitsVec& temp_bf) {
	BigInt& quotient = result; // quotient won't be written to.
	_private::divmod<true, false>(quotient, result, a, b, temp, temp_af, temp_bf);
}

/**
 * @brief integer modulo.
 *
 * @param result the modulo result. Max size requirement: `result.r.size() == a.size() + 2`.
 * @param a the dividend
 * @param b the divisor
 */
BIGINT_TRACY_CONSTEXPR_VOID
mod(BigInt& result, const is_BigInt_like auto &a, const is_BigInt_like auto &b) {
	DigitsVecPtr temp, temp_af, temp_bf;
	mod(result, a, b, *temp, *temp_af, *temp_bf);
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const is_BigInt_like auto &a, one_of<uint32_t, int32_t, uint64_t, int64_t> auto b) -> decltype(b) {
	decltype(b) result;
	mod(result, a, b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const is_BigInt_like auto &a, const is_BigInt_like auto &b) -> BigInt {
	BigInt result;
	mod(result, a, b);
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator%=(BigInt &a, const one_of<uint32_t, int32_t, uint64_t, int64_t> auto &b) -> BigInt& {
	auto result = a % b;
	a.resize(1);
	a.set(0, utils::constexpr_abs(result));
	a.sign() = _private::get_sign(result);
	return a;
}

BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator%=(BigInt &a, const is_BigInt_like auto &b) -> BigInt& {
	a = a % b;
	return a;
}

}


namespace bigint::_private {

constexpr uint8_t calculate_base_power_64(uint32_t base) {
	// formula: result = floor(64 / log2(base))
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
	explicit constexpr base_conversion_64(uint32_t base) noexcept
		: base_power(calculate_base_power_64(base)), //  = 19 for base 10;
		division_base(utils::ipow(base, base_power)) {}
	uint8_t base_power;
	uint64_t division_base;
};

constexpr uint8_t calculate_base_power_32(uint32_t base) {
	// formula: result = floor(32 / log2(base))
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
	explicit constexpr base_conversion_32(uint32_t base) noexcept
		: base_power(calculate_base_power_32(base)), //  = 9 for base 10;
		division_base(static_cast<uint32_t>(utils::ipow(base, base_power))) {}
	uint8_t base_power;
	uint32_t division_base;
};

CONSTEXPR_AUTO
to_char(uint8_t d) -> char {
	return (d > 9 ? 'a' - 10 : '0') + d;
}

BIGINT_TRACY_CONSTEXPR_VOID
to_string_padded_generic(std::string& result, uint64_t val, uint_fast8_t base, uint_fast8_t len) {
	for (auto i = len; i --> 0;) {
		auto d = static_cast<uint_fast8_t>(val % base);
		val /= base;
		result.push_back(to_char(d));
	}
	assert(val == 0);
}

}


// to_string, from_string, & digit_sum:
namespace bigint {
namespace _private {
	BIGINT_TRACY_CONSTEXPR_AUTO
	to_string(const std::span<const uint64_t> &v, uint_fast8_t base, bool isNegative) -> std::string {
		if (base > 36 || base < 2) {
			std::string msg = "to_string only supports bases in the range 2 - 36 (inclusive).";
			throw std::invalid_argument(utils::error_msg(std::move(msg)));
		}
		auto conv = base_conversion_32(base);

		std::string result;
		if (conv.division_base != 0) {
			// DivModResult temp{BigInt{v, Sign::POS}, (uint32_t)0};
			BigInt q{v, Sign::POS};
			uint64_t r;
			while (q > 0) {
				auto digits = divmod(q, q, conv.division_base);
				to_string_padded_generic(result, digits, base, conv.base_power);
			}
			while (!result.empty() && result.back() == '0') {
				result.pop_back();
			}
			if (isNegative) {
				result.push_back('-');
			}
			std::ranges::reverse(result);

		} else { // special case for when base is a divider of 32.
			auto base_power = base_conversion_64{base}.base_power;

			result.reserve(base_power * (v.size()) + (isNegative ? 1 : 0));

			uint64_t last_digs = v.back();
			while (last_digs > 0) {
				auto d = static_cast<uint_fast8_t>(last_digs % base);
				last_digs /= base;
				result.push_back(to_char(d));
			}

			if (isNegative) {
				result.push_back('-');
			}
			std::ranges::reverse(result);

			const auto r0 = result.size();
			for (BigInt::size_type i = 1; i < v.size(); ++i) {
				result.append(base_power, '0');
				uint64_t digs = v[v.size() - i - 1];
				for (uint_fast8_t j = base_power; j --> 0;) {
					auto d = static_cast<uint_fast8_t>(digs % base);
					digs /= base;
					result.at(r0 + (i - 1) * base_power + j) = to_char(d);
				}
			}
		}
		if (result.empty()) {
			result.push_back('0');
		}
		return result;
	}
}


BIGINT_TRACY_CONSTEXPR_AUTO
to_string(const is_BigInt_like auto& v, uint8_t base = 10) -> std::string {
	auto result = _private::to_string(v._span(), base, is_neg(v));
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base2(const is_BigInt_like auto& v) -> std::string {
	return to_string(v, 2);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base8(const is_BigInt_like auto& v) -> std::string {
	return to_string(v, 8);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base10(const is_BigInt_like auto& v) -> std::string {
	return to_string(v);
}

BIGINT_TRACY_CONSTEXPR_AUTO
to_string_base16(const is_BigInt_like auto& v) -> std::string {
	return to_string(v, 16);
}


NODISCARD_AUTO
_to_debug_string_data(const is_BigInt_like auto& value) -> std::string {
	if (value.size() == 0) {
		return "{}";
	} else {
		std::ostringstream oss;
		oss << "{";

		BigInt::size_type i = 0;
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

NODISCARD_AUTO
to_debug_string(const is_BigInt_like auto& value) -> std::string {
	const std::string sign = value.sign() == Sign::POS ? "POS" : "NEG";
	return std::string{"IBigIntLike {_data: "} + _to_debug_string_data(value) + ", _sign: " + sign + "}";
}

BIGINT_TRACY_CONSTEXPR_AUTO
// from_string(const std::string_view input, uint_fast8_t base = 10)  default value for base is already defined in forward declaration.
from_string(const std::string_view input, uint_fast8_t base) -> BigInt {
	if (base > 36 || base < 2) {
		std::string msg = "from_string only supports bases in the range 2 - 36 (inclusive).";
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}
	const auto conv = _private::base_conversion_64(base);

	BigInt result{0};

	if (input.empty()) {
		return result;
	}

	const BigInt::size_type i0 = input[0] == '-' or input[0] == '+' ? 1 : 0;

	if (conv.division_base != 0) {
		auto expected_size = static_cast<BigInt::size_type>(std::ceil(static_cast<double>(input.size() - i0) * std::log2(base) / 64.)) + 1;
		if (expected_size > 2) {
			result.reserve(expected_size);
		}
		for (BigInt::size_type i = i0; i < input.size(); i += conv.base_power) {
			auto substr = std::string_view(input).substr(i, conv.base_power);
			auto mul = substr.size() == conv.base_power ? conv.division_base : utils::ipow(base, static_cast<uint8_t>(substr.size()));
			auto add = utils::stoull(substr, base);
			result *= mul;
			result += add;
		}

	} else { // special case for when base is a divider of 32.
		const auto digit_count = input.size() - i0;
		const auto expected_size = std::max<BigInt::size_type>(0, digit_count / conv.base_power + (digit_count % conv.base_power > 0 ? 1 : 0));
		result.resize(expected_size);

		BigInt::size_type i = input.size();
		for (BigInt::size_type k = 0; k < result.size(); k += 1, i -= conv.base_power) {
			const auto window_size = std::min<BigInt::size_type>(i - i0, conv.base_power);
			auto substr = std::string_view(input).substr(i - window_size, window_size);
			auto add = utils::stoull(substr, base);
			result.set(k, add);
		}
	}
	result.sign() = input[0] == '-' ? Sign::NEG : Sign::POS;
	return result;
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base2(const std::string_view input) -> BigInt {
	return from_string(input, 2);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base8(const std::string_view input) -> BigInt {
	return from_string(input, 8);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base10(const std::string_view input) -> BigInt {
	return from_string(input, 10);
}

BIGINT_TRACY_CONSTEXPR_AUTO
from_string_base16(const std::string_view input) -> BigInt {
	return from_string(input, 16);
}


inline std::ostream & operator<<(std::ostream& ostream, const is_BigInt_like auto& val) {
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

#include "utils/_bigint_tracy_undefines.h"
