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

#ifdef BIGINT_TRACY_ENABLE
//      [[nodiscard]] constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO [[nodiscard]] inline auto
//      constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD inline auto
//      constexpr void
#	define BIGINT_TRACY_CONSTEXPR_VOID inline void
//      constexpr
#	define BIGINT_TRACY_CONSTEXPR inline
#else
//      [[nodiscard]] constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO CONSTEXPR_AUTO
//      constexpr auto
#	define BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD CONSTEXPR_AUTO_DISCARD
//      constexpr void
#	define BIGINT_TRACY_CONSTEXPR_VOID CONSTEXPR_VOID
//      constexpr
#	define BIGINT_TRACY_CONSTEXPR constexpr

#endif

#ifdef ZoneScoped
#	define BIGINT_TRACY_ZONE_SCOPED ZoneScoped
#else
#	define BIGINT_TRACY_ZONE_SCOPED
#endif


namespace bigint::_private {
/**
 * When Multiplying two BigInts A and B, the Karatsuba multiplication algorithm gets chosen if the total number of
 * digits in A and B together is equal to or exceeds this threshold.
 *
 * E.g.:
 * | digits ...          |                        |
 * | in A | in B | total | Algorithm used         |
 * | ==== | ==== | ===== | ====================== |
 * | 2    | 3    | 5     | naive (Big * Big)      |
 * | 31   | 32   | 63    | naive (Big * Big)      |
 * | 32   | 32   | 64    | Karatsuba              |
 * | 96   | 2    | 98    | Karatsuba              |
 * | 96   | 1    | 97    | naive (Big * uint64_t) |
 *
 * The optimal value seems to lie somewhere between 60 and 80, based on some basic testing.
 */
constexpr size_t MIN_TOTAL_DIGITS_FOR_MULT_KARATSUBA = 64;
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
neg(Sign sign) -> Sign {
	return (sign == Sign::POS) ? Sign::NEG : Sign::POS;
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
		for (auto i = _data.size(); 0 <--i;) {
			if (_data[i] == 0) {
				_data.pop_back();
			} else {
				break;
			}
		}
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


// copy_digits_to_from
namespace bigint::_private {

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
copy_digits_to_from(TLHS &to, const TRHS& from) {
	if (to.size() < from.size()) {
		auto msg = utils::concat(
			"Param 'to' must not have less digits than 'from'.",
			" to.size(): ", to.size(), " from.size(): ", from.size(), ".");
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}

	for(auto i = from.size(); i --> 0;) {
		to.set(i, from[i]);
	}
}

}


// class BigIntAdapter:
namespace bigint {

template<std::integral T>
class BigIntAdapter : public IBigIntLike
{ // maybe use SSO instead? (SSO = Small String Optimization)

 public:
	explicit constexpr
	BigIntAdapter(T v) noexcept
		: _data(v)
	{ }

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		if constexpr (std::is_signed_v<T>) {
			return _data < 0 ? Sign::NEG : Sign::POS;
		} else {
			return Sign::POS;
		}
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return 1;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const noexcept -> std::make_unsigned_t<T> {
		return index >= size() ? 0 : utils::constexpr_abs(_data);
	}

private:
	T _data;
};

}


// class BigIntAdapter2:
namespace bigint {

class BigIntAdapter2 : public IBigIntLike
{ // maybe use SSO instead? (SSO = Small String Optimization)
public:
	explicit constexpr
	BigIntAdapter2(uint64_t lo, uint64_t hi, Sign sign=Sign::POS) noexcept
		 : _data({lo, hi}), _sign(sign)
	{ }

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
		return _data.back() != 0 ? 2 : 1;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const noexcept -> uint64_t {
		return index >= 2 ? 0 : _data[index];
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
#if BIGINT_ENABLE_BOUNDS_CHECKS
		utils::check_bounds(index, 2);
#endif
		_data[index] = digit;
	}

private:
	std::array<uint64_t, 2> _data;
	Sign _sign;
};

}


// is_zero(), is_neg(), ...:
namespace bigint {

template <is_BigInt_like T>
CONSTEXPR_AUTO
is_zero(const T &value) -> bool {
	return value.size() == 0 || (value.size() == 1 && value[0] == 0);
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

template <typename T>
class BigIntLShifted : IBigIntLike {
	using T_Plain = std::remove_cvref_t<T>;
public:
	constexpr BigIntLShifted(T_Plain&& lhs, const uint64_t shifted) :
		_lhs(std::move(lhs)), _shifted(shifted) {}

	constexpr BigIntLShifted(std::remove_reference_t<T>& lhs, const uint64_t shifted) :
		_lhs(lhs), _shifted(shifted) {}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return lhs().sign();
	}

	CONSTEXPR_AUTO
	sign() noexcept -> Sign& {
		return lhs().sign();
	}

	CONSTEXPR_AUTO
	size() const -> std::size_t {
		return lhs().size() + _shifted;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return index < _shifted ? 0 : lhs()[index - _shifted];
	}

	BIGINT_TRACY_CONSTEXPR_VOID
	cleanup() {
		lhs().cleanup();
	}

private:
	T _lhs;
	uint64_t _shifted;

	CONSTEXPR_AUTO
	lhs() const -> const T_Plain& { return _lhs; }
	CONSTEXPR_AUTO
	lhs() -> T_Plain& { return _lhs; }
};

template <typename T>
class BigIntRShifted : IBigIntLike {
	using T_Plain = std::remove_cvref_t<T>;
public:
	constexpr BigIntRShifted(T_Plain&& lhs, const uint64_t _shifted) :
		_lhs(std::move(lhs)), _shifted(_shifted) {}

	constexpr BigIntRShifted(std::remove_reference_t<T>& lhs, const uint64_t shifted) :
		_lhs(lhs), _shifted(shifted) {}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return lhs().sign();
	}

	CONSTEXPR_AUTO
	sign() noexcept -> Sign& {
		return lhs().sign();
	}

	CONSTEXPR_AUTO
	size() const -> std::size_t {
		assert(lhs().size() >= _shifted);
		return lhs().size() - _shifted;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return lhs()[index + _shifted];
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
#if BIGINT_ENABLE_BOUNDS_CHECKS
		utils::check_bounds(index, size());
#endif
		lhs().set(index + _shifted, digit);
	}

	CONSTEXPR_VOID
	append(uint64_t v) { lhs().append(v); }

	BIGINT_TRACY_CONSTEXPR_VOID
	cleanup() {
		lhs().cleanup();
	}

private:
	T _lhs;
	uint64_t _shifted;

	CONSTEXPR_AUTO
	lhs() const -> const T_Plain& { return _lhs; }
	CONSTEXPR_AUTO
	lhs() -> T_Plain& { return _lhs; }
};

template <typename T>
class BigIntRMasked : IBigIntLike {
	using T_Plain = std::remove_cvref_t<T>;
public:
	constexpr BigIntRMasked(T_Plain&& lhs, const uint64_t shifted, const uint64_t mask_size) :
		_lhs(std::move(lhs)), _shifted(shifted), _mask_size(mask_size) {
		assert(_lhs.size() - _shifted >= mask_size);
	}

	constexpr BigIntRMasked(std::remove_reference_t<T>& lhs, const uint64_t shifted, const uint64_t mask_size) :
		_lhs(lhs), _shifted(shifted), _mask_size(mask_size) {
		assert(_lhs.size() - _shifted >= mask_size);
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return lhs().sign();
	}

	CONSTEXPR_AUTO
	size() const -> std::size_t {
		return _mask_size;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return index >= size() ? 0 : lhs()[index + _shifted];
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
#if BIGINT_ENABLE_BOUNDS_CHECKS
		utils::check_bounds(index, size());
#endif
		lhs().set(index + _shifted, digit);
	}

	CONSTEXPR_AUTO
	shifted() const -> uint64_t { return _shifted; }

	CONSTEXPR_AUTO
	mask_size() const -> uint64_t { return _mask_size; }

	CONSTEXPR_AUTO
	lhs() const -> const T_Plain& { return _lhs; }
	CONSTEXPR_AUTO
	lhs() -> T_Plain& { return _lhs; }

private:
	T _lhs;
	uint64_t _shifted;
	uint64_t _mask_size;
};


template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
lshifted(const TLHS& a, uint64_t shifted) {
	return BigIntLShifted<const TLHS&>(a, is_zero(a) ? 0 : shifted);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
lshifted(TLHS&& a, uint64_t shifted) {
	return BigIntLShifted<TLHS>(std::forward<TLHS>(a), is_zero(a) ? 0 : shifted);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshifted(const TLHS& a, uint64_t shifted) {
	auto old_size = a.size();
	auto shifted_new = old_size > shifted ? shifted : old_size;
	return BigIntRShifted<const TLHS&>(a, shifted_new);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshifted(TLHS& a, uint64_t shifted) {
	auto old_size = a.size();
	auto shifted_new = old_size > shifted ? shifted : old_size;
	return BigIntRShifted<TLHS&>(a, shifted_new);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshifted(TLHS&& a, uint64_t shifted) {
	auto old_size = a.size();
	auto shifted_new = old_size > shifted ? shifted_new : old_size;
	return BigIntRShifted<TLHS>(std::forward<TLHS>(a), shifted);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(const BigIntRMasked<const TLHS&>& a, uint64_t shifted, uint64_t mask_size) {
	auto old_size = a.size();
	auto old_size_shifted = old_size > shifted ? old_size - shifted : 0;
	return BigIntRMasked<const TLHS&>(a.lhs(), a.shifted() + shifted, std::min(old_size_shifted, mask_size));
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(const TLHS& a, uint64_t shifted, uint64_t mask_size) {
	auto old_size = a.size();
	auto old_size_shifted = old_size > shifted ? old_size - shifted : 0;
	return BigIntRMasked<const TLHS&>(a, shifted, std::min(old_size_shifted, mask_size));
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(BigIntRMasked<TLHS&>& a, uint64_t shifted, uint64_t mask_size) {
	auto old_size = a.size();
	auto old_size_shifted = old_size > shifted ? old_size - shifted : 0;
	return BigIntRMasked<const TLHS&>(a.lhs(), a.shifted() + shifted, std::min(old_size_shifted, mask_size));
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(TLHS& a, uint64_t shifted, uint64_t mask_size) {
	auto old_size = a.size();
	auto old_size_shifted = old_size > shifted ? old_size - shifted : 0;
	return BigIntRMasked<TLHS&>(a, shifted, std::min(old_size_shifted, mask_size));
}

/**
 * Convenience function to convert any IBigIntLike to a BigIntRMasked i order to reduce the amount of
 * template specializations needed.
 */
template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(const BigIntRMasked<const TLHS&>& a) {
	return BigIntRMasked<const TLHS&>(a.lhs(), a.shifted() , a.mask_size());
}

/**
 * Convenience function to convert any IBigIntLike to a BigIntRMasked i order to reduce the amount of
 * template specializations needed.
 */
template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rmasked(const TLHS& a) {
	return BigIntRMasked<const TLHS&>(a, 0, a.size());
}

}


// abs(), neg:
namespace bigint::_private {

template <typename T>
class BigIntAbs: IBigIntLike {
	using T_Plain = std::remove_cvref_t<T>;
public:
	explicit constexpr
	BigIntAbs(T_Plain&& lhs) :
		_lhs(std::move(lhs)) {}

	explicit constexpr
	BigIntAbs(const T_Plain& lhs) :
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

private:
	T _lhs;

	CONSTEXPR_AUTO
	lhs() const -> const T_Plain& { return _lhs; }
};

template <typename T>
class BigIntNeg: IBigIntLike {
	using T_Plain = std::remove_cvref_t<T>;
public:
	explicit constexpr
	BigIntNeg(T_Plain&& lhs) :
		_lhs(std::move(lhs)) {}

	explicit constexpr
	BigIntNeg(const T_Plain& lhs) :
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

private:
	T _lhs;

	CONSTEXPR_AUTO
	lhs() const -> const T_Plain& { return _lhs; }
};

}


// abs(), neg:
namespace bigint {

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
abs(const TLHS& a) {
	return _private::BigIntAbs<const TLHS&>(a);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
abs(TLHS&& a) {
	return _private::BigIntAbs<TLHS>(std::forward<TLHS>(a));
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
operator-(const TLHS& a) {
	return _private::BigIntNeg<const TLHS&>(a);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
operator-(TLHS&& a) {
	return _private::BigIntNeg<TLHS>(std::forward<TLHS>(a));
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

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator<=>(const TLHS &a, const TRHS &b) -> std::strong_ordering {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b) && a.sign() == b.sign()) {
		std::size_t i;
		for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
			// do nothing
		}
		return a[i] <=> b[i];
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
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
_add_ignore_sign(TRES &result, TLHS &a, TRHS &b) -> bool {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(a.size() >= b.size());

	bool c = false; // carry
	size_t i = 0;
	for (; i < b.size(); i++) {
		const auto ai = a[i];
		auto result_i = ai + b[i];
		if (c) { ++result_i; }
		c = result_i < ai || (c && result_i == ai);
		result.set(i, result_i);
	}

	for (; i < result.size(); i++) {
		const auto ai = a[i];
		result.set(i, ai + (c ? 1 : 0));
		c = ai == std::numeric_limits<decltype(ai)>::max() && c;
	}

	return c;
}

/**
 * @brief subtracts `b` from `a` ignoring their sign. `abs(a)` *must* be equal or greater than `abs(b)`.
* @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
_sub_ignore_sign_no_negative_result(TRES &result, TLHS &a, TRHS &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	assert(a.size() >= b.size());

	bool c = false; // carry
	size_t i = 0;
	for (; i < b.size(); i++) {
		const auto ai = a[i];
		auto result_i = ai - b[i];
		if (c) { --result_i; }
		c = result_i > ai || (c && result_i == ai);
		result.set(i, result_i);
	}

	for (; i < result.size(); i++) {
		const auto ai = a[i];
		result.set(i, ai - (c ? 1 : 0));
		c = ai == 0 && c;
	}
	if (c) { // should NEVER happen.
		auto msg = utils::concat(
			"leftover carry! abs(b) was greater than abs(a). This is not supported.",
			" c: ", c, ".");
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}
}

/**
 * @brief adds two integers ignoring their sign.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 * @return carry
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
add_ignore_sign(TRES &result, TLHS &a, TRHS &b) {
	if (b.size() > a.size()) { // put the number with more digits first.
		if (_add_ignore_sign(result, b, a)) {
			result.append(1);
		}
	} else {
		if (_add_ignore_sign(result, a, b)) {
			result.append(1);
		}
	}
}

/**
 * @brief subtracts `b` from `a` ignoring their sign.
 * @param result the result will be put in here.
 * @param a the first operand.
 * @param b the second operand.
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
sub_ignore_sign(TRES &result, TLHS &a, TRHS &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	const bool isNegative = abs(b) > abs(a);
	if (isNegative) {
		_sub_ignore_sign_no_negative_result(result, b, a);
		result.sign() = Sign::NEG;
	} else {
		_sub_ignore_sign_no_negative_result(result, a, b);
		result.sign() = Sign::POS;
	}
}

}


// addition:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
add(TRES &result, TLHS &a, const TRHS &b) {
	result.resize(std::max(a.size(), b.size()));
	if (is_pos(a)) {
		if (is_pos(b)) {
			_private::add_ignore_sign(result, a, const_cast<TRHS&>(b));
		} else {
			_private::sub_ignore_sign(result, a, const_cast<TRHS&>(b));
		}
	} else {
		if (is_pos(b)) {
			_private::sub_ignore_sign(result, const_cast<TRHS&>(b), a);
		} else {
			_private::add_ignore_sign(result, a, const_cast<TRHS&>(b));
			result.sign() = Sign::NEG;
		}
	}
	result.cleanup();
}


template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), BigIntAdapter(b));
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(TLHS a, const TRHS &b) -> BigInt {
	return b + a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator+(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, TRHS b) -> TLHS& {
	add(a, a, BigIntAdapter(b));
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
	result.resize(std::max(a.size(), b.size()));
	if (is_pos(a)) {
		if (is_pos(b)) {
			_private::sub_ignore_sign(result, a, const_cast<TRHS&>(b));
		} else {
			_private::add_ignore_sign(result, a, const_cast<TRHS&>(b));
		}
	} else {
		if (is_pos(b)) {
			_private::add_ignore_sign(result, a, const_cast<TRHS&>(b));
			result.sign() = Sign::NEG;
		} else {
			_private::sub_ignore_sign(result, const_cast<TRHS&>(b), a);
		}
	}
	result.cleanup();
}

template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), BigIntAdapter(b));
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(TLHS a, const TRHS &b) -> BigInt {
	BigInt result;
	BigIntAdapter a_ {a};
	sub(result, a_, b);
	return result;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator-(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, TRHS b) -> TLHS& {
	sub(a, a, BigIntAdapter(b));
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

BIGINT_TRACY_CONSTEXPR_AUTO
mult(uint64_t a, uint64_t b) -> BigIntAdapter2 {
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

	return BigIntAdapter2{r, c};
}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult(TRES &result, TLHS &a, uint64_t b) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0 || is_zero(a)) {
		result = BigInt{};
		return;
	}
	result.resize(a.size());

	uint64_t c = 0; // carry
	for (size_t i = 0; i < a.size(); i++) {
		const auto rc = mult(a[i], b);
		auto result_i = rc[0] + c;
		result.set(i, result_i);
		c = rc[1] + (result_i < c ? 1 : 0); // account for addition overflow
	}
	if (c){
		result.append(c);
	};
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

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
_mult_naive_ignore_sign(TRES &result, const TLHS &a, const TRHS &b) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(a) || is_zero(b)) {
		result = BigInt{};
		return;
	}

	result.resize(a.size() + b.size());

	BigInt temp;
	for (size_t i = 0; i < b.size(); i++) {
		mult(temp, a, b[i]);
		auto rshifted_result = _private::rmasked(result, i, temp.size() + 1);
		if (_private::_add_ignore_sign(rshifted_result, rshifted_result, temp)) {
			auto msg = utils::concat(
				"leftover carry! rshifted_result was not big enough. This is not supported."
				" rshifted_result.size(): ", rshifted_result.size(), " temp.size(): ", temp.size(), ".");
			throw std::invalid_argument(utils::error_msg(std::move(msg)));
		}
	}

	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
_mult_karatsuba_ignore_sign(TRES &result, const TLHS &lhs, const TRHS &rhs) {
	BIGINT_TRACY_ZONE_SCOPED;
	// xx = mm(ac) + m((a+b) * (c+d) - ac - bd) + (bd)
	if (is_zero(lhs) || is_zero(rhs)) {
		result = BigInt{};
		return;
	}
	if (lhs.size() + rhs.size() < _private::MIN_TOTAL_DIGITS_FOR_MULT_KARATSUBA) {
		_mult_naive_ignore_sign(result, rhs, lhs);
		return;
	}
	if (rhs.size() == 1) {
		mult(result, lhs, rhs[0]);
		result.sign() = Sign::POS;
		return;
	}
	if (lhs.size() == 1) {
		mult(result, rhs, lhs[0]);
		result.sign() = Sign::POS;
		return;
	}

	auto n = std::max(lhs.size(), rhs.size());

	const auto mid = n >> 1;

	const auto a = _private::rmasked(lhs, mid, lhs.size());
	const auto b = _private::rmasked(lhs, 0, mid);
	const auto c = _private::rmasked(rhs, mid, rhs.size());
	const auto d = _private::rmasked(rhs, 0, mid);

	BigInt ac;
	_mult_karatsuba_ignore_sign(ac, a, c);

	BigInt bd;
	_mult_karatsuba_ignore_sign(bd, b, d);

	BigInt ab_cd;
	{
		BigInt a_b;
		a_b.resize(std::max(a.size(), b.size()));
		_private::add_ignore_sign(a_b, a, b);

		BigInt c_d;
		c_d.resize(std::max(c.size(), d.size()));
		_private::add_ignore_sign(c_d, c, d);

		_mult_karatsuba_ignore_sign(ab_cd, _private::rmasked(a_b), _private::rmasked(c_d));
	}

	ab_cd -= ac;
	ab_cd -= bd;

	result = _private::lshifted(ac, mid << 1);
	result.resize(lhs.size() + rhs.size());
	auto result_shifted = _private::rshifted(result, mid);
	_private::_add_ignore_sign(result_shifted, result_shifted, ab_cd);
	result += bd;

	result.cleanup();
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult_naive(TRES &result, const TLHS &lhs, const TRHS &rhs) {
	if (rhs.size() > lhs.size()) { // put the number with more digits first.
		_mult_naive_ignore_sign(result, rhs, lhs);
	} else {
		_mult_naive_ignore_sign(result, lhs, rhs);
	}
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult_karatsuba(TRES &result, const TLHS &lhs, const TRHS &rhs) {
	if (rhs.size() > lhs.size()) { // put the number with more digits first.
		_mult_karatsuba_ignore_sign(result, _private::rmasked(lhs), _private::rmasked(rhs));
	} else {
		_mult_karatsuba_ignore_sign(result, _private::rmasked(rhs), _private::rmasked(lhs));
	}
	result.sign() = _private::mult_sign(lhs.sign(), rhs.sign());
}

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_VOID
mult(TRES &result, const TLHS &a, const TRHS &b) {
	if (a.size() + b.size() < _private::MIN_TOTAL_DIGITS_FOR_MULT_KARATSUBA) {
		mult_naive(result, a, b);
	} else {
		mult_karatsuba(result, a, b);
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

/**
 * @brief copy n most-significant digits (prefix) from s to d
 * @param d destination
 * @param s source
 * @param n number of digits
 */
template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
_prefix(TLHS& d, const TRHS& s, size_t n) {
	d.resize(n);
	_private::copy_digits_to_from(d, _private::rshifted(s, s.size() - n));
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
_correct_d_and_subtract(TLHS& x, const TRHS& b, uint64_t d) -> uint64_t {
	// all values are guaranteed to be positive.
	x -= b * d;
	if (is_neg(x)) {
		x += b;
		d -= 1;
		if (is_neg(x)) {
			x += b;
			d -= 1;
		}
	}
	// x is now positive again and (t = bd) holds.
	return d;
}

// forward declaration:
template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient, bool ignore_remainder>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod_ignore_sign(const TLHS& aa, const TRHS& bb) -> DivModResult<BigInt>;


/**
 * @brief division algorithm adapted from Nitin Verma, 2021, Implementing Basic Arithmetic for Large Integers: Division
 * @param a the dividend
 * @param b the divisor
 * @param e estimator (?) for the divisor
 * @return
 */
template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient>
BIGINT_TRACY_CONSTEXPR_AUTO
_divide_loop(const TLHS& a, const TRHS& b, uint64_t e) -> DivModResult<BigInt> {
	const auto na = a.size();
	const auto nb = b.size();
	const bool is_single_digit_division = nb == 1;
	/* na >= nb holds. */
	BigInt qt;
	if constexpr (!ignore_quotient) {
		/* quotient can have maximum (na-nb+1) digits */
		qt.resize(na-nb+1);
	}
	BigInt x;
	_prefix(x, a, nb-1);

	/* loop-invariant P: first m digits of ’a’ have been brought-down and processed. */
	for(auto m = nb; m <= na; ++m) {
		x.insert_front(a[na - m]);

		const uint64_t yz_lo = x[nb-1];
		const uint64_t yz_hi = x[nb];

		uint64_t d = utils::div_u128_saturate(yz_hi, yz_lo, e); // yz/e;

		if(is_single_digit_division) {
			x = BigIntAdapter2{yz_lo, yz_hi} - mult(e, d); // remainder is less than e, so must be single digit
		} else {
			d = _correct_d_and_subtract(x, b, d);
		}

		if constexpr (!ignore_quotient) {
			qt.set(qt.size() - 1 - m + nb, d);
		}
	}
	/* (loop-invariant P) AND (m=na) holds. */
	/* Now x contains the remainder. */

	if constexpr (!ignore_quotient) {
		qt.cleanup();
	}
	return {std::move(qt), std::move(x)};
}


/**
 * @brief division algorithm adapted from Nitin Verma, 2021, Implementing Basic Arithmetic for Large Integers: Division
 *
 * @param aa the dividend
 * @param bb the divisor
 * @return the result
 */
template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod_ignore_sign(const TLHS& aa, const TRHS& bb) -> DivModResult<BigInt> {
	BIGINT_TRACY_ZONE_SCOPED;

	if (is_zero(bb)) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(aa)) {
		return {BigInt{0}, BigInt{0}};
	}
	if (bb.size() > aa.size()) {
		if constexpr (!ignore_remainder) {
			return {BigInt{0}, BigInt{aa}};
		} else {
			return {BigInt{0}, BigInt{1}}; // remainder could be any positive number. It is only used to
			                                            // signify that the remainder is non-zero. Used for correcting a
			                                            // negative quotient in divmod(...)
		}
	}

	uint64_t e = bb[bb.size() - 1];
	if((bb.size() > 1) && (e < 1ull<<63)) {
		/* normalization */
		uint64_t f = utils::div_u128_saturate(1ull, 0ull, e + 1); // 1^64/(e + 1);
		const auto af = aa * f;
		const auto bf = bb * f;
		e = bf[bf.size() - 1];
		auto result = _divide_loop<BigInt, _private::BigIntAbs<const BigInt&>, ignore_quotient>(af, abs(bf), e);

		if constexpr (!ignore_remainder) {
			result.r = std::move(divmod_ignore_sign<BigInt, BigIntAdapter<uint64_t>, false, true>(result.r, BigIntAdapter{f}).d);
		}
		return result;
	} else {
		return _divide_loop<TLHS, _private::BigIntAbs<const TRHS&>, ignore_quotient>(aa, abs(bb), e);
	}
}

}


// divmod:
namespace bigint {

template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const TLHS &a, const TRHS &b) -> DivModResult<BigInt> {
	auto r = _private::divmod_ignore_sign<TLHS, TRHS, ignore_quotient, ignore_remainder>(a, b);

	r.r.cleanup(); //r.r is also used by quotient.
	if constexpr (!ignore_remainder) {
		if (!is_zero(r.r)) {
			if (is_neg(a)) {
				r.r.sign() = Sign::NEG;
			}
			if (a.sign() != b.sign()) {
				r.r += b;
			}
		}
	}
	if constexpr (!ignore_quotient) {
		r.d.sign() = _private::mult_sign(a.sign(), b.sign());
		if (!is_zero(r.r) and r.d.sign() == Sign::NEG) {
			r.d -= 1;
		}
		r.d.cleanup();
	}
	return r;
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const TLHS &a, int64_t b) -> DivModResult<BigInt, int64_t> {
	const auto res = divmod<TLHS, BigIntAdapter<int64_t>, ignore_quotient, ignore_remainder>(a, BigIntAdapter{b});
	auto r = static_cast<int64_t>(res.r[0]);
	return { res.d, is_neg(res.r) ? -r : r };
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod(const TLHS &a, uint64_t b) -> DivModResult<BigInt, uint64_t> {
	const auto res = divmod<TLHS, BigIntAdapter<uint64_t>, ignore_quotient, ignore_remainder>(a, BigIntAdapter{b});
	return { res.d, res.r[0] };
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod1(const TLHS &a, uint32_t b) -> DivModResult<BigInt, uint32_t> {
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(a)) {
		return {BigInt{0}, 0};
	}

	BigInt q;
	if constexpr (!ignore_quotient) {
		q.resize(a.size());
	}

	// y = q * b; but not always.
	BigInt y{0, a.sign()};
	if constexpr (!ignore_remainder) {
		y.resize(a.size() + 1);
	}

	uint64_t c_div = 0ull;
	uint64_t di_hi = 0ull;
	uint64_t ai_lo = 0ull;

	size_t i = a.size() + 1;
	bool is_first_iter = true;
	while (i > 0) {
		--i;
		uint64_t di = 0;
		if (!is_first_iter) {
			const auto di_lo = ai_lo / b;
			c_div = (ai_lo % b) << 32; // ???
			di = (di_hi << 32) | di_lo;

			if constexpr (!ignore_quotient) {
				q.set(i, di);
			}
		}

		const auto ai = (i != 0) ? a[i-1]: 0;
		const auto ai_hi = (i != 0) ? (ai >> 32) | c_div : 0;
				   di_hi = ai_hi / b;
		const auto c_diw = (ai_hi % b) << 32;
				   ai_lo = (ai & 0xFFFFFFFFull) | c_diw;

		if constexpr (!ignore_remainder) {
			const uint32_t c_ = di_hi >> 32;
			const auto ji = di + c_;
			const auto yi = ji*b; // todo: investigate why doesn't this overflow ??
			y.set(i, yi);
		}

		is_first_iter = false;
	}


	if constexpr (!ignore_quotient) {
		q.sign() = _private::mult_sign(a.sign(), Sign::POS);
		if (c_div != 0 && q.sign() == Sign::NEG) {
			q -= 1;
		}
		q.cleanup();
	}

	if constexpr (ignore_remainder) {
		return { q, 0 };
	} else {
		y.cleanup();
		BigInt r_big = a - y;
		auto r = static_cast<uint32_t>(r_big[0]);
		if (r != 0 && is_neg(a)) {
			r = b - r;
		}
		return { q, r };
	}
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
BIGINT_TRACY_CONSTEXPR_AUTO
divmod1(const TLHS &a, int32_t bb) -> DivModResult<BigInt, int32_t> {
	if (bb < 0) {
		auto r = divmod1<_private::BigIntNeg<const TLHS&>, ignore_quotient, ignore_remainder>(-a, static_cast<uint32_t>(-bb));
		return DivModResult{std::move(r.d), -static_cast<int32_t>(r.r)};
	} else {
		auto r = divmod1<TLHS, ignore_quotient, ignore_remainder>(a, static_cast<uint32_t>(bb));
		return DivModResult{std::move(r.d), static_cast<int32_t>(r.r)};
	}
}

}


// division:
namespace bigint {

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
div(BigInt &result, TLHS &a, uint32_t b) {
	BIGINT_TRACY_ZONE_SCOPED;
	if (b == 0) {
		throw std::domain_error{utils::error_msg("division by zero")};
	}
	if (is_zero(a)) {
		result = BigInt{0};
	}
	uint64_t c_lo = 0ull;
	for (auto i = a.size(); i-->0;) {
		const auto ai = a[i];
		const auto ai_hi = (ai >> 32) | c_lo;
		const auto r_hi = ai_hi / b;
		const auto c_hi = (ai_hi % b) << 32;

		const auto ai_lo = (ai & 0xFFFFFFFFull) | c_hi;
		const auto r_lo = ai_lo / b;
		c_lo = (ai_lo % b) << 32;
		const auto r = (r_hi << 32) | r_lo;
		result.set(i, r);
	}
	result.sign() = a.sign();
	if (c_lo != 0 && result.sign() == Sign::NEG) {
		result -= 1;
	}
}

template <is_BigInt_like TLHS>
BIGINT_TRACY_CONSTEXPR_VOID
div(BigInt &result, TLHS &a, int32_t bb) {
	if (bb < 0) {
		auto neg_a = -a; // todo check undefined behavior with -a if result === a, because operator-() const-ifies a?
		div(result, neg_a, static_cast<uint32_t>(-bb));
	} else {
		div(result, a, static_cast<uint32_t>(bb));
	}
}


template <is_BigInt_like TLHS, one_of<uint32_t, int32_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	result.resize(a.size());
	div(result, const_cast<TLHS&>(a), b);
	result.cleanup();
	return result;
}

template <is_BigInt_like TLHS, one_of<uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const TLHS &a, TRHS b) -> BigInt {
	return divmod<TLHS, BigIntAdapter<TRHS>, false, true>(a, BigIntAdapter(b)).d;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator/(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, false, true>(a, b).d;
}


template <one_of<uint32_t, int32_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, TRHS b) -> BigInt& {
	div(a, a, b);
	a.cleanup();
	return a;
}

template <one_of<uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, TRHS b) -> BigInt& {
	const auto result = divmod<BigInt, BigIntAdapter<TRHS>, false, true>(a, BigIntAdapter(b)).d;
	a = std::move(result);
	return a;
}

template <is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
operator/=(BigInt &a, const TRHS &b) -> BigInt& {
	const auto result = divmod<BigInt, TRHS, false, true>(a, b).d;
	a = std::move(result);
	return a;
}

}


// Modulo:
namespace bigint {

template <is_BigInt_like TLHS, one_of<uint32_t, int32_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const TLHS &a, TRHS b) -> TRHS {
	return divmod1<TLHS, true>(a, b).r; // we can truncate safely because the divisor only also is uint32_t.
}

template <is_BigInt_like TLHS, one_of<uint64_t, int64_t> TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const TLHS &a, TRHS b) -> TRHS {
	return divmod<TLHS, true>(a, b).r;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, true>(a, b).r;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
BIGINT_TRACY_CONSTEXPR_AUTO
operator%=(TLHS &a, const TRHS &b) -> BigInt& {
	a = divmod<TLHS, TRHS, true>(a, b).r;
	return a;
}

}


// misc math:
namespace bigint {

/**
 * @brief calculates the integer square root of y using Newton's method.
 * @param y the value to get the quare root of.
 * @return the integer square root of y.
 * @throws std::domain_error if y < 0
 */
template<is_BigInt_like T>
BIGINT_TRACY_CONSTEXPR_AUTO
sqrt(const T& y) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;

	if (is_neg(y)) {
		throw std::domain_error{utils::error_msg("integer square root of a negative number is undefined.")};
	}

	// sqrt(0) == 0; sqrt(1) == 1
	if (y <= 1) {
		return y;
	}

	// Initial estimate (must be too high)
	BigInt x0 = y >> 1;

	while (true)	{
		auto x1 = (x0 + y / x0) >> 1;
		if (!(x1 < x0))
			return x0;
		x0 = (x1 + y / x1) >> 1;
		if (!(x0 < x1))
			return x1;
	}
}

namespace _private {
template<is_BigInt_like BASE, is_BigInt_like T>
BIGINT_TRACY_CONSTEXPR_AUTO
calculate_squares(const BASE& base, const T& y) -> std::vector<BigInt> {
	constexpr uint8_t exp_bits_max = 64;
	std::vector<BigInt> squares;
	squares.push_back(base);
	for (uint8_t i = 1; i < exp_bits_max; ++i) {
		auto square = squares.back() * squares.back();
		if (square > y) {
			break;
		}
		squares.emplace_back(std::move(square));
	}
	return squares;
}
}


/**
 * @brief calculates the integer logarithm of `y` for the given `base`. E.g.: `log(10, 1000)` == 3.
 * @param base the base of the logarithm
 * @param y the value to get the logarithm of.
 * @return the integer logarithm of y.
 * @throws std::domain_error if base <= 1 or y <= 0
 */
template<is_BigInt_like BASE, is_BigInt_like T>
BIGINT_TRACY_CONSTEXPR_AUTO
log(const BASE& base, const T& y) -> uint64_t {
	if (base <= 1) {
		throw std::domain_error{utils::error_msg("integer log for a base less or equal to one is undefined.")};
	}
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	if (base > y) {
		return 0;
	}

	const auto squares = _private::calculate_squares(base, y);

	uint64_t result = 0;
	BigInt temp{y};

	for (auto i = static_cast<uint8_t>(squares.size()); i --> 0;) {
		const auto& square = squares[i];
		if (square <= temp) {
			temp /= square;
			uint64_t mask = 1ull << i;
			result |= mask;
		}
	}
	return result;
}


/**
 * @brief calculates the integer logarithm of `y` for base 10. E.g.: `log10(1000) == 3`.
 * @param y the value to get the logarithm of.
 * @return the integer logarithm base 10 of y.
 * @throws std::domain_error if y <= 0
 */
template<is_BigInt_like T>
BIGINT_TRACY_CONSTEXPR_AUTO
log10(const T& y) -> uint64_t {
	BIGINT_TRACY_ZONE_SCOPED;
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	return log(BigInt{10}, y);
}


/**
 * @brief calculates the integer logarithm of `y` for base 2. E.g.: `log2(7) == 3`. Runs in O(1) time.
 * @param y the value to get the logarithm of.
 * @return the integer logarithm base 2 of y.
 * @throws std::domain_error if y <= 0
 */
template<is_BigInt_like T>
BIGINT_TRACY_CONSTEXPR_AUTO
log2(const T& y) -> uint64_t {
	BIGINT_TRACY_ZONE_SCOPED;
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	return 64 * y.size() - utils::clzll(y[y.size() - 1]) - 1 ;
}


/**
 * @brief raises `base` to the power of `exp`. E.g.: `pow(10, 3) == 1000`. if you need to calculate `pow(a, b) % m` use `pow_mod()` instead.
 * @param base the base.
 * @param exp the exponent.
 * @return base^exp.
 * @throws std::domain_error if base == exp == 0
 */
template<is_BigInt_like BASE>
BIGINT_TRACY_CONSTEXPR_AUTO
pow(const BASE& base, uint64_t exp) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	if (exp == 0) {
		if (is_zero(base)) {
			throw std::domain_error{utils::error_msg("zero to the power of zero is undefined.")};
		} else {
			return BigInt{1};
		}
	} else if (is_zero(base)) {
		return BigInt{0};
	}

	BigInt result{1};
	BigInt temp{base};

	const auto exp_bits = static_cast<uint8_t>(64 - utils::clzll(exp));
	for (uint8_t i = 0; i < exp_bits; ++i) {
		const auto mask = 1ull << i;
		if (exp & mask) {
			result *= temp;
		}
		if (i+1 < exp_bits) {
			temp = temp * temp;
		}
	}
	return result;
}


/**
 * @brief raises `base` to the power of `exp` modulo `mod`. E.g.: `pow_mod(10, 3, 12) == 4`. This is *way* faster than doing `pow(base, exp) % mod`; especially for large `base` and `exp`.
 * @param base the base.
 * @param exp the exponent.
 * @param mod the modulo.
 * @return pow(base, exp) % mod.
 * @throws std::domain_error if base == exp == 0 or if mod == 0
 */
template<is_BigInt_like BASE, is_BigInt_like EXP, is_BigInt_like MOD>
BIGINT_TRACY_CONSTEXPR_AUTO
pow_mod(const BASE& base, const EXP& exp, const MOD& mod) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	if (is_zero(exp)) {
		if (is_zero(base)) {
			throw std::domain_error{utils::error_msg("zero to the power of zero is undefined.")};
		} else {
			return BigInt{1};
		}
	} else if (is_zero(base) || is_neg(exp) || mod == 1) {
		return BigInt{0};
	} else if (mod == 0) {
		throw std::domain_error{utils::error_msg("modulo must not be zero.")};
	}

	const uint64_t exp_bits = (exp.size() - 1) * 64 + (64 - utils::clzll(exp[exp.size()-1]));

	BigInt result{1};
	auto temp = base % mod;
	for (uint64_t i = 0; i < exp_bits; ++i) {
		const auto mask = 1ull << (i % 64);
		if (exp[i/64] & mask) {
			result = (result * temp) % mod;
		}
		if (i+1 < exp_bits) {
			temp = (temp * temp) % mod;
		}
	}
	return result;
}

}


// combinatorics:
namespace bigint {

BIGINT_TRACY_CONSTEXPR_AUTO
factorial(uint32_t n) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	BigInt result(1);
	for (uint64_t i = 1; i <= n; ++i) {
		result *= i;
	}
	return result;
}


/**
 * @brief Calculates the number of ways to choose `k` items from `n` items without repetition and with order.
 *        Evaluates to `n! / (n - k)!` when `k <= n` and evaluates to zero otherwise.
 * @param k the number of items to choose.
 * @param n the size of the pool of items to choose from.
 * @return n P k
 */
BIGINT_TRACY_CONSTEXPR_AUTO
perm(uint32_t n, uint32_t k) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	// factorial(n) / factorial(n-k);

	if (k > n) {
		return BigInt{0};
	}

	BigInt result(1);
	for (uint32_t i = n-k+1; i <= n; ++i) {
		result *= i;
	}
	return result;
}


/**
 * @brief Calculates the number of ways to choose `k` items from `n` items without repetition and without order.
 *        Evaluates to `n! / ((n - k)! * k!)` when `k <= n` and evaluates to zero otherwise.
 * @param k the number of items to choose.
 * @param n the size of the pool of items to choose from.
 * @return n C k
 */
BIGINT_TRACY_CONSTEXPR_AUTO
comb(uint32_t n, uint32_t k) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	// factorial(n) / ( factorial(n-k) * factorial(k) );
	//                 \_dividend 1_/   \_dividend 2_/
	// notice the symmetry between (n-k) and (k). We'll exploit that.

	if (k > n) {
		return BigInt{0};
	}

	if (k == 0 || k == n) {
		return BigInt{1};
	}

	auto d1 = n-k > k ? n-k : k; // exploit the symetry to skip as many unnecessary division as possible.
	auto d2 = n-k > k ? k : n-k;
	BigInt result(1);
	uint32_t j = 1;
	uint32_t i = d1+1;
	while (j <= d2) { // do division & multiplication in one to keep the intermediate values small.
		result *= i;
		result /= j; // division by 32 bit integer. is very fast compared to normal division.
		++i;
		++j;
	}
	if (i > 0) { // if i == 0 we had an overflow.
		while (i <= n) {
			result *= i;
			++i;
		}
	}
	return result;
}

}


// number theory:
namespace bigint {
namespace _private {

template<int k>
BIGINT_TRACY_CONSTEXPR_AUTO
lehmer_step(uint64_t& u_, uint64_t& v_, int64_t& x_im1, int64_t& x_i, int64_t& y_im1, int64_t& y_i) {
	uint64_t q_i = u_ / v_;
	const int64_t x_ip1 = x_im1 - q_i * x_i;
	const int64_t y_ip1 = y_im1 - q_i * y_i;

	uint64_t r_ = u_ - q_i * v_;
	u_ = v_;
	v_ = r_;

	// Determine if q_i is correct (Jebelean’s condition).
	if (v_ < utils::constexpr_abs(y_ip1) || u_ - v_ < utils::constexpr_abs(x_ip1 - x_i)) {
		return false; // break
	}

	// swap x and y
	x_im1 = y_i; y_im1 = x_i;
	x_i = y_ip1; y_i = x_ip1;
	return true; // continue
}

template<int k>
BIGINT_TRACY_CONSTEXPR_VOID
lehmer(BigInt& U, BigInt& V) {
	BIGINT_TRACY_ZONE_SCOPED;

	auto h = log2(U) < k-1 ? 0 : log2(U) + 1 - k;
	uint64_t u_ = (U >> h)[0];
	uint64_t v_ = (V >> h)[0];

	int64_t x_im1 = 1, x_i = 0;
	int64_t y_im1 = 0, y_i = 1;

	bool swap_xy = false;
	while (lehmer_step<k>(u_, v_, x_im1, x_i, y_im1, y_i)) {
		swap_xy = !swap_xy;
	}

	if (swap_xy) {
		std::swap(x_i, y_i);
		std::swap(x_im1, y_im1);
	}

	// We know q,,..., qi-l were correct, qi as incorrect.
	auto R = x_i * U + y_i * V;
	U *= x_im1;
	U += y_im1 * V;
	V = std::move(R);
}

BIGINT_TRACY_CONSTEXPR_AUTO
gcd_internal(const BigInt& Uu, const BigInt& Vv) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	// we require abs(Uu) >= abs(Vv)

	constexpr auto k = 63;

	BigInt U = Uu; U.sign() = Sign::POS;
	BigInt V = Vv; V.sign() = Sign::POS;

	while (!is_zero(V)) {
		if (log2(U) - log2(V) <= k/2) {
			_private::lehmer<k>(U, V);
		}

		auto R = U % V;
		U = std::move(V);
		V = std::move(R);
	}

	return U;
}
}

/**
 * @brief Calculates the greatest common divisor of the specified integer arguments. The result is never negative.
 *        Adapted from Jonathan Sorenson, 1995, An Analysis of Lehmer’s Euclidean GCD Algorithm
 * @return the greatest common divisor of the specified integer arguments.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
gcd(const BigInt& U, const BigInt& V) -> BigInt {
	if (abs(U) < abs(V)) {
		return _private::gcd_internal(V, U);
	} else {
		return _private::gcd_internal(U, V);
	}
}

/**
 * @brief Calculates the least common multiple of the specified integer arguments. The result is never negative.
 *        Adapted from Jonathan Sorenson, 1995, An Analysis of Lehmer’s Euclidean GCD Algorithm
 * @return the least common multiple of the specified integer arguments.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
lcm(const BigInt& u, const BigInt& v) -> BigInt {
	if (is_zero(u) || is_zero(v)) {
		return BigInt{0};
	}
	return abs(u) / gcd(u, v) * abs(v);
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

template <int base = 10>
BIGINT_TRACY_CONSTEXPR_AUTO
digit_sum(const BigInt& v) -> uint64_t {
	BIGINT_TRACY_ZONE_SCOPED;
	constexpr auto division_base = _private::base_conversion_32{base}.division_base; // 9 is the larges value for n such that 10^n fits into 32 bits

	if constexpr (division_base != 0) {
		DivModResult temp {v, (uint32_t)0};
		temp.d.sign() = Sign::POS;

		uint64_t sum = 0ull;
		while (!is_zero(temp.d)) {
			temp = divmod1(temp.d, division_base);
			uint32_t& digs = temp.r;
			while (digs > 0) {
				sum += digs % base;
				digs /= base;
			}
			temp.d.cleanup();
		}
		return sum;

	} else { // special case for when base is a divider of 32.
		constexpr auto base_power = _private::base_conversion_64{base}.base_power;
		uint64_t sum = 0ull;
		for (size_t i = 0; i < v.size(); ++i) {
			uint64_t digs = v[i];
			for (size_t j = 0; j < base_power; ++j) {
				sum += digs % base;
				digs /= base;
			}
		}
		return sum;
	}
}

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
			temp = divmod1(temp.d, conv.division_base);
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

#undef BIGINT_TRACY_CONSTEXPR_AUTO
#undef BIGINT_TRACY_CONSTEXPR_AUTO_DISCARD
#undef BIGINT_TRACY_CONSTEXPR_VOID
#undef BIGINT_TRACY_CONSTEXPR
#undef BIGINT_TRACY_ZONE_SCOPED
