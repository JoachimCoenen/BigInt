#ifndef __BIG_INT_H__
#define __BIG_INT_H__

#include "utils.h"

#include <cstdint>
#include <vector>
#include <string>
#include <type_traits>

#define LOGGING(a)

namespace bigint {

class IBigIntLike { };

template <typename T>
concept is_BigInt_like = std::is_base_of_v<IBigIntLike, T>;

template <typename T, typename T1, typename T2> // todo use concept everywhere!
concept one_of = std::same_as<T, T1> || std::same_as<T, T2>;

}


// forward declarations:
namespace bigint {

class BigInt;

CONSTEXPR_AUTO
to_string_base2(const BigInt &v) -> std::string;

CONSTEXPR_AUTO
to_string_base8(const BigInt &v) -> std::string;

CONSTEXPR_AUTO
to_string_base10(const BigInt &v) -> std::string;

CONSTEXPR_AUTO
to_string_base16(const BigInt &v) -> std::string;

CONSTEXPR_AUTO
to_string(const BigInt &v) -> std::string;

CONSTEXPR_AUTO
from_string_base2(const std::string_view input) -> BigInt;

CONSTEXPR_AUTO
from_string_base8(const std::string_view input) -> BigInt;

CONSTEXPR_AUTO
from_string_base10(const std::string_view input) -> BigInt;

CONSTEXPR_AUTO
from_string_base16(const std::string_view input) -> BigInt;

CONSTEXPR_AUTO
from_string(const std::string_view input) -> BigInt;


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


namespace bigint::_private {
CONSTEXPR_VOID
resizeBigInt(BigInt &a, std::size_t size);

}


// class BigInt:
namespace bigint {

enum class Sign: bool {
	POS = 0,
	NEG = 1
};

}


// class BigInt:
namespace bigint::_private {

CONSTEXPR_AUTO_DISCARD
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
	BigInt(const std::string_view v)
		: BigInt(from_string(v))
	{ }

	explicit constexpr
	BigInt(const std::vector<uint64_t> &v, Sign sign=Sign::POS)
		: _data(v), _sign(sign) // todo: maby make it non-public?
	{ }

	explicit constexpr
	BigInt(const std::vector<uint64_t> &&v, Sign sign=Sign::POS)
		: _data(std::move(v)), _sign(sign) // todo: maby make it non-public?
	{ }

	template<is_BigInt_like T>
	explicit constexpr
		BigInt(const T &v)
		: _data(v.size()), _sign(v.sign()) {
		for (size_t i = 0; i < v.size(); ++i) {
			_data[i] = v[i];
		}

	}

	constexpr
	~BigInt() {}

	constexpr
	BigInt(const BigInt &other) // copy constructor
		: _data(other._data), _sign(other._sign) {
		LOGGING("copying: " << _data.size());
	}

	constexpr
	BigInt(BigInt &&other) // move constructor
		: _data(std::move(other._data)), _sign(other._sign) {
		LOGGING("moving: " << _data.size());
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(const BigInt &other) -> BigInt& { // copy assignment
		BigInt tmp(other);
		swap(*this, tmp);
		LOGGING("assigning: " << _data.size());
		return *this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(BigInt &&other) -> BigInt& { // move assignment
		BigInt tmp(std::move(other));
		swap(*this, tmp);
		LOGGING("move-assigning: " << _data.size());
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
		return (index >= size()) ? 0ull : _data.at(index);
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
		if (index >= _data.size()) {
			throw std::string(" index out of bound!");
		}
		_data.at(index) = digit;
	}

	CONSTEXPR_VOID
	append(uint64_t v) { _data.push_back(v); }

	CONSTEXPR_VOID
	cleanup() const { // todo: const???!
		for (auto i = _data.size(); 0 <--i;) {
			if (_data[i] == 0) {
				_data.pop_back();
			} else {
				break;
			}
		}
	}

private:
	CONSTEXPR_VOID
	resize(std::size_t size) {
		_data.resize(size, 0);
	}

	friend CONSTEXPR_VOID
	_private::resizeBigInt(BigInt &a, std::size_t size);

	friend CONSTEXPR_VOID
	swap(BigInt& a, BigInt& b) noexcept;

public:
	[[nodiscard]] auto
	__data_for_testing_only() const -> std::vector<uint64_t>{
		auto result = _data;
		result.push_back(is_neg(*this) ? 1 : 0);
		return result;
	}

private:
	mutable std::vector<uint64_t> _data; // only for cleanups! // todo: mutable???!
	Sign _sign;
};


CONSTEXPR_VOID
swap(BigInt& a, BigInt& b) noexcept {
	std::swap(a._data, b._data);
	std::swap(a._sign, b._sign);
}

}


// resize
namespace bigint::_private {
CONSTEXPR_VOID
resizeBigInt(BigInt &a, std::size_t size) {
	a.resize(size);
}

CONSTEXPR_AUTO
makeBigIntWithSize(std::size_t size) -> BigInt {
	auto r = BigInt();
	resizeBigInt(r, size); //OCTOPUS
	return r;
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

	constexpr
	~BigIntAdapter() noexcept {}

	constexpr
	BigIntAdapter(const BigIntAdapter &other) noexcept // copy constructor
		: _data(other._data)
	{ }

	constexpr
	BigIntAdapter(BigIntAdapter &&other) noexcept // copy constructor
		: _data(std::move(other._data))
	{ }

	CONSTEXPR_AUTO_DISCARD
	operator=(const BigIntAdapter &other) noexcept -> BigIntAdapter& { // copy assignment
		_data = other._data;
		return *this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(BigIntAdapter &&other) noexcept -> BigIntAdapter& { // move assignment
		_data = other._data;
		other._data = 0; // maybe?
		return *this;
	}

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
		return (index >= size()) ? 0 : (std::make_unsigned_t<T>)(_data < 0 ? -_data : _data);
	}

private:
	template<std::integral TT>
	friend CONSTEXPR_VOID
	swap(BigIntAdapter<TT>& a, BigIntAdapter<TT>& b) noexcept;

private:
	T _data;
};


template<std::integral T>
CONSTEXPR_VOID
swap(BigIntAdapter<T>& a, BigIntAdapter<T>& b) noexcept {
	std::swap(a._data, b._data);
}


}


// class BigIntAdapter2:
namespace bigint {

class BigIntAdapter2 : public IBigIntLike
{ // maybe use SSO instead? (SSO = Small String Optimization)

 public:
	explicit constexpr
	BigIntAdapter2(uint64_t lo, uint64_t hi, Sign sign=Sign::POS) noexcept
		 : _lo(lo), _hi(hi), _sign(sign)
	{ }

	constexpr
	~BigIntAdapter2() noexcept {}

	constexpr
	BigIntAdapter2(const BigIntAdapter2 &other) noexcept // copy constructor
		: _lo(other._lo), _hi(other._hi)
	{ }

	constexpr
	BigIntAdapter2(BigIntAdapter2 &&other) noexcept // copy constructor
		: _lo(std::move(other._lo)), _hi(std::move(other._hi))
	{ }

	CONSTEXPR_AUTO_DISCARD
	operator=(const BigIntAdapter2 &other) noexcept -> BigIntAdapter2& { // copy assignment
		_lo = other._lo;
		_hi = other._hi;
		return *this;
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(BigIntAdapter2 &&other) noexcept -> BigIntAdapter2& { // move assignment
		_lo = other._lo;
		_hi = other._hi;
		other._lo = 0; // maybe?
		other._hi = 0; // maybe?
		return *this;
	}

	CONSTEXPR_AUTO
	sign() const noexcept -> Sign {
		return _sign;
	}

	CONSTEXPR_AUTO
	size() const noexcept -> std::size_t {
		return _hi != 0 ? 2 : 1;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const noexcept -> uint64_t {
		switch (index) {
		case 0:
			return _lo;
		case 1:
			return _hi;
		default:
			return 0;
		}
	}

private:
	friend CONSTEXPR_VOID
	swap(BigIntAdapter2& a, BigIntAdapter2& b) noexcept;

private:
	uint64_t _lo;
	uint64_t _hi;
	Sign _sign;
};


CONSTEXPR_VOID
swap(BigIntAdapter2& a, BigIntAdapter2& b) noexcept {
	std::swap(a._lo, b._lo);
	std::swap(a._hi, b._hi);
	std::swap(a._sign, b._sign);
}


}


// is_zero(), is_neg(), ...:
namespace bigint {

template <is_BigInt_like T>
CONSTEXPR_AUTO
is_zero(const T &value) -> bool {
	return value.size() == 1 && value[0] == 0;
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

}


// rshifted & lshifted:
namespace bigint::_private {

template <typename T>
struct BigIntBox_t {
	using type = T;

};

template <>
struct BigIntBox_t<BigInt> {
	using type = BigInt&;

};
template <>
struct BigIntBox_t<const BigInt> {
	using type = const BigInt&;

};

template <typename T>
using BigIntBox = typename BigIntBox_t<T>::type;

template <typename T>
class BigIntLShifted : IBigIntLike {
public:
	constexpr BigIntLShifted(const BigIntBox<T> lhs, const uint64_t rhs) :
		_lhs(lhs), _rhs(rhs) {}

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
		return lhs().size() + _rhs;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return (index < _rhs) ? 0 : lhs()[index - _rhs];
	}

	CONSTEXPR_VOID
	cleanup() {
		lhs().cleanup();
	}

private:
	BigIntBox<T> _lhs;
	uint64_t _rhs;

	CONSTEXPR_AUTO
	lhs() const -> const T& { return _lhs; }
	CONSTEXPR_AUTO
	lhs() -> T& { return _lhs; }
};

template <is_BigInt_like TLHS>
inline auto lshifted(const TLHS &a, uint64_t b) {
	return BigIntLShifted<const TLHS>(a, b);
}

template <typename T>
class BigIntRShifted : IBigIntLike {
public:
	constexpr BigIntRShifted(const BigIntBox<T> lhs, const uint64_t rhs) :
		_lhs(lhs), _rhs(rhs) {}

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
		return lhs().size() - _rhs;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		if (index < 0) {
			throw std::string(" index out of bound!");
		}
		return lhs()[index + _rhs];
	}

	CONSTEXPR_VOID
	set(std::size_t index, uint64_t digit) {
		if (index < 0) {
			throw std::string(" index out of bound!");
		}
		lhs().set(index + _rhs, digit);
	}

	CONSTEXPR_VOID
	append(uint64_t v) { lhs().append(v); }

	CONSTEXPR_VOID
	cleanup() {
		lhs().cleanup();
	}

private:
	BigIntBox<T> _lhs;
	uint64_t _rhs;

	CONSTEXPR_AUTO
	lhs() const -> const T& { return _lhs; }
	CONSTEXPR_AUTO
	lhs() -> T& { return _lhs; }
};

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshifted(const TLHS &a, uint64_t b) {
	return BigIntRShifted<const TLHS>(a, b);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshifted(TLHS &a, uint64_t b) {
	return BigIntRShifted<TLHS>(a, b);
}

}


// abs(), neg:
namespace bigint::_private {

template <typename T>
class BigIntAbs: IBigIntLike {
public:
	constexpr BigIntAbs(const BigIntBox<T> lhs) :
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
	BigIntBox<T> _lhs;

	CONSTEXPR_AUTO
	lhs() const -> const T& { return _lhs; }
};

template <typename T>
class BigIntNeg: IBigIntLike {
public:
	constexpr BigIntNeg(const BigIntBox<T> lhs) :
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
	BigIntBox<T> _lhs;

	CONSTEXPR_AUTO
	lhs() const -> const T& { return _lhs; }
};

}


// abs(), neg:
namespace bigint {

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
abs(const TLHS &a) {
	return _private::BigIntAbs<const TLHS>(a);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
operator-(const TLHS &a) {
	return _private::BigIntNeg<const TLHS>(a);
}

}


// bitwise shift operations:
namespace bigint {

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
lshiftBits(const TLHS &vec, uint64_t digits) -> BigInt {
	BigInt result{std::vector<uint64_t>(digits / 64, 0), vec.sign()};
	digits %= 64;
	result.append(vec[0] << digits);
	for (uint64_t i = 1; i < vec.size(); ++i) {
		const uint64_t lo = digits > 0 ? vec[i-1] >> (64-digits) : 0;
		const uint64_t hi = vec[i] << digits;
		result.append(lo | hi);
	}
	if (digits > 0) {
		result.append(vec[vec.size()-1] >> (64-digits));
	}
	return result;
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
rshiftBits(const TLHS &vec, uint64_t digits) -> BigInt {
	BigInt result{0, vec.sign()};
	const uint64_t start = digits / 64;
	digits %= 64;
	for (uint64_t i = start; i < vec.size()-1; ++i) {
		const uint64_t lo = vec[i] >> digits;
		const uint64_t hi = vec[i+1] << (64-digits);
		result.append(lo | hi);
	}
	result.append(vec.back() >> digits);
	return result;
}

}


// comparison operators:
namespace bigint {

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator<=>(const TLHS &a, const TRHS &b) -> std::strong_ordering {
	if (is_zero(a) && is_zero(b)) {
		return std::strong_ordering::equal;
	}

	if (!is_zero(a) && !is_zero(b) && a.sign() == b.sign()) {
		std::size_t i;
		for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
			// do nothin'
		}
		return a[i] <=> b[i];
	}

	return (is_neg(a) || is_pos(b)) ? std::strong_ordering::less : std::strong_ordering::greater;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator<(const TLHS &a, const TRHS &b) {
	return (a <=> b) < 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator>(const TLHS &a, const TRHS &b) {
	return (a <=> b) > 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator<=(const TLHS &a, const TRHS &b) {
	return (a <=> b) <= 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator>=(const TLHS &a, const TRHS &b) {
	return (a <=> b) >= 0;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator==(const TLHS &a, const TRHS &b) {
	return (a <=> b) == 0;
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
operator<(const TLHS &a, const uint64_t &b) {
	return a < BigInt(b);
}

template <is_BigInt_like TLHS>
CONSTEXPR_AUTO
operator>(const TLHS &a, const uint64_t &b) {
	return a > BigInt(b);
}

}


// add / sub ignoring sign:
namespace bigint::_private {

/**
 * @brief add Ignores the sign of both operands
 * @param result
 * @param a
 * @param b
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
add_ignore_sign(TRES &result, TLHS &a, TRHS &b) {
	uint8_t c = 0; // carry
	for (auto i = 0ull; i < result.size(); i++) {
		const auto ai = a[i];
		result.set(i, ai + b[i] + c);
		c = (result[i] < ai || (c && result[i] == ai)) ? 1: 0;
	}
	if (c) {
		result.append(c);
	}
	LOGGING("adding: " << result.size());
}


/**
 * @brief sub Ignores the sign of both operands
 * @param result
 * @param a
 * @param b
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
_sub_ignore_sign_no_gegative_result_private(TRES &result, TLHS &a, TRHS &b) {
	uint8_t c = 0; // carry
	for (auto i = 0ull; i < result.size(); i++) {
		const auto ai = a[i];
		result.set(i, ai - b[i] - c);
		c = (result[i] > ai || (c && result[i] == ai)) ? 1: 0;
	}
	if (c) { // should NEVER happen.
		throw std::string("leftover carry! this should not have happened.");
	}
}


/**
 * @brief sub Ignores the sign of both operands
 * @param result
 * @param a
 * @param b
 */
template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
sub_ignore_sign(TRES &result, TLHS &a, TRHS &b) {
	const bool isNegative = abs(b) > abs(a);
	if (isNegative) {
		_sub_ignore_sign_no_gegative_result_private(result, b, a);
		result.sign() = Sign::NEG;
	} else {
		_sub_ignore_sign_no_gegative_result_private(result, a, b);
		result.sign() = Sign::POS;
	}
	LOGGING("subtracting: " << result.size());
}

}


// addition:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
add(TRES &result, TLHS &a, const TRHS &b) {
	_private::resizeBigInt(result, std::max(a.size(), b.size()));
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
CONSTEXPR_AUTO
operator+(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), BigIntAdapter(b));
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator+(TLHS a, const TRHS &b) -> BigInt {
	return b + a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator+(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	add(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, TRHS b) -> TLHS& {
	add(a, a, BigIntAdapter(b));
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, const TRHS &b) -> TLHS& {
	add(a, a, b);
	return a;
}

}


// subtraction:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_VOID
sub(TRES &result, TLHS &a, const TRHS &b) {
	_private::resizeBigInt(result, std::max(a.size(), b.size()));
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
CONSTEXPR_AUTO
operator-(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), BigIntAdapter(b));
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator-(TLHS a, const TRHS &b) -> BigInt {
	BigInt result;
	BigIntAdapter a_ {a};
	sub(result, a_, b);
	return result;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator-(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result;
	sub(result, const_cast<TLHS&>(a), b);
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, TRHS b) -> TLHS& {
	sub(a, a, BigIntAdapter(b));
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO_DISCARD
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

CONSTEXPR_AUTO
mult(uint64_t a, uint64_t b) -> BigIntAdapter2 {
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
	if (c != 0)
		return BigIntAdapter2{r, c};
	else
		return BigIntAdapter2{r, 0};
}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
CONSTEXPR_VOID
mult(TRES &result, TLHS &a, uint64_t b) {
	if (b == 0 || is_zero(a)) {
		result = BigInt{};
		return;
	}
	_private::resizeBigInt(result, a.size());

	uint64_t c = 0; // carry
	for (auto i = 0ull; i < a.size(); i++) {
		const auto rc = mult(a[i], b);
		result.set(i, rc[0] + c);
		c = rc[1] + (result[i] < c ? 1 : 0); // account for addition overflow
	}
	if (c){
		result.append(c);
	};
	result.sign() = a.sign();
	result.cleanup();
	LOGGING("multiplying: " << result.size());
}

template <is_BigInt_like TRES, is_BigInt_like TLHS>
CONSTEXPR_VOID
mult(TRES &result, TLHS &a, int64_t b) {
	mult(result, a, (uint64_t)llabs(b));
	if (b < 0) {
		result.sign() = _private::neg(result.sign());
	}
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
mult(const TLHS &a, const TRHS &b) -> BigInt {
	if (is_zero(a) || is_zero(b)) {
		return BigInt{};
	}
	BigInt result;
	_private::resizeBigInt(result, a.size() + b.size());

	for (auto i = 0ull; i < b.size(); i++) {
		auto temp = a * b[i];
		auto lshifted_temp = _private::lshifted(temp, i);
		_private::add_ignore_sign(result, result, lshifted_temp);
	}
	result.sign() = _private::mult_sign(a.sign(), b.sign());
	result.cleanup();
	LOGGING("multiplying: " << result.size());
	return result;
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO
operator*(const TLHS &a, TRHS b) -> BigInt {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<TRHS>, uint64_t, int64_t>;
	BigInt result;
	mult(result, const_cast<TLHS&>(a), (TRHS2)b);
	return result;
}

template <std::integral TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator*(TLHS a, const TRHS &b) -> BigInt {
	return b * a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator*(const TLHS &a, const TRHS &b) -> BigInt {
	return mult(a, b);
}

template <is_BigInt_like TLHS, std::integral TRHS>
CONSTEXPR_AUTO_DISCARD
operator*=(TLHS &a, TRHS b) -> BigInt& {
	using TRHS2 = std::conditional_t<std::is_unsigned_v<TRHS>, uint64_t, int64_t>;
	mult(a, a, (TRHS2)b);
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator*=(TLHS &a, const TRHS &b) -> BigInt& {
	a = std::move(mult(a, b));
	return a;
}

}


// DivModResult:
namespace bigint {

template<class D, class R=D>
struct DivModResult { D d; R r; };

}


// divmod ignoring sign:
namespace bigint::_private {

template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod_ignore_sign(const TLHS &a, const TRHS &b) -> DivModResult<BigInt> {
	a.cleanup();
	if (is_zero(a)) { // todo handle 0/0 !
		return {BigInt{0}, BigInt{0}};
	}
	if (b.size() > a.size()) {
		if constexpr (!ignore_remainder) {
			return {BigInt{0}, BigInt{a}};
		} else {
			return {BigInt{0}, BigInt{1}}; // remainder could be any positive number. it is only used to signify that the remiander is not zero.
		}
	}

	DivModResult<BigInt> r;
	r.r = BigInt{abs(a)};
	r.r.sign() = Sign::POS;
	if constexpr (!ignore_quotient) {
		_private::resizeBigInt(r.d, a.size() - b.size() + 1);
	}

	// because were muiltiplying the size by 64, this only works for BigInts with less tha 1 Exabyte of memory footprint.
	// (1 Exabyte == 1024 Petabyte)
	const auto aHi = a[a.size()-1];
	const auto ad  = a.size()*64 - (aHi==0 ? 64 : utils::clzll(aHi));
	const auto b2Hi = b[b.size()-1];
	const auto b2d = b.size()*64 - utils::clzll(b2Hi);
	const auto adbd2 = int64_t(ad - b2d);
	//uint64_t min = std::max(0ll, adbd2 - 1);
	const uint64_t max = std::max(0ll, adbd2 + 1ll);//%64; //std::numeric_limits<uint64_t>::max();
	uint64_t i = max;//(a.size() - b.size())*64;
	auto a2IsSmaller = false;

	while (a2IsSmaller || !(r.r < abs(b))) {
		--i;
		BigInt p2 = lshiftBits(b, i%64);
		a2IsSmaller = _private::rshifted(r.r, i/64) < abs(p2);
		if (! a2IsSmaller) {
			if constexpr (!ignore_quotient) {
				r.d.set(i/64, r.d[i/64] | 1ull << i % 64);
			}
			auto rshifted_r_r = _private::rshifted(r.r, i/64);
			_private::sub_ignore_sign(rshifted_r_r, rshifted_r_r, p2);
		}
	}

	if constexpr (!ignore_remainder) {
		r.r.cleanup();
	}
	if constexpr (!ignore_quotient) {
		r.d.cleanup();
	}
	LOGGING("divmod: " << a.size()<< " : " << b.size());
	return r;
}


template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod_ignore_sign(const TLHS &a, const uint64_t &b) -> DivModResult<BigInt, uint64_t> {
	const auto res = divmod<TLHS, BigIntAdapter<uint64_t>, ignore_quotient, ignore_remainder>(a, BigIntAdapter{b});
	return { res.d, res.r[0] };
}

}


// divmod:
namespace bigint {

template <is_BigInt_like TLHS, is_BigInt_like TRHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod(const TLHS &a, const TRHS &b) -> DivModResult<BigInt> {
	a.cleanup();
	if (is_zero(a)) { // todo handle 0/0 !
		return {BigInt(0), BigInt(0)};
	}

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
	LOGGING("divmod: " << a.size()<< " : " << b.size());
	return r;
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod(const TLHS &a, const int64_t &b) -> DivModResult<BigInt, int64_t> {
	const auto res = divmod<TLHS, BigIntAdapter<int64_t>, ignore_quotient, ignore_remainder>(a, BigIntAdapter{b});

	if constexpr (ignore_remainder) {
		return { res.d, 0 };
	} else {
		int64_t r = (int64_t)res.r[0];
		if (is_neg(res.r)) {
			r = -r;
		}
		return { res.d, r };
	}
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod(const TLHS &a, const uint64_t &b) -> DivModResult<BigInt, uint64_t> {
	const auto res = divmod<TLHS, BigIntAdapter<uint64_t>, ignore_quotient, ignore_remainder>(a, BigIntAdapter{b});
	return { res.d, res.r[0] };
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod1(const TLHS &a, uint32_t b) -> DivModResult<BigInt, uint32_t> { // todo: validate: Isn't uint32_t sufficient for remainder return type?
	BigInt q;
	if constexpr (!ignore_quotient) {
		_private::resizeBigInt(q, a.size());
	}
	BigInt y{0, a.sign()};
	_private::resizeBigInt(y, a.size());


	uint64_t c_div = 0ull;

	const auto ai_next = a[a.size()-1];
	const auto ai_hi_next = (ai_next >> 32) | c_div;
		  auto di_hi_next = ai_hi_next / b;
	const auto c_diw_next = (ai_hi_next % b) << 32;
		  auto ai_lo_next = (ai_next & 0xFFFFFFFFull) | c_diw_next;
	const auto c_ = di_hi_next >> 32;
	y.append(c_*b);

	for (auto i = a.size(); i-->0;) {
		const auto di_hi = di_hi_next;

		const auto ai_lo = ai_lo_next;
		const auto di_lo = ai_lo / b;
				   c_div = (ai_lo % b) << 32; // ???
		const auto di = (di_hi << 32) | di_lo;
		if constexpr (!ignore_quotient) {
				   q[i] = di;
		}

		const auto ai_next = (i != 0) ? a[i-1]: 0;
		const auto ai_hi_next = (i != 0) ? (ai_next >> 32) | c_div : 0;
				   di_hi_next = ai_hi_next / b;
		const auto c_diw_next = (ai_hi_next % b) << 32;
				   ai_lo_next = (ai_next & 0xFFFFFFFFull) | c_diw_next;

		const auto c_ = di_hi_next >> 32;
		const auto ji = di + c_;
		const auto yi = ji*b;
		y.set(i, yi);

	}

	if constexpr (!ignore_quotient) {
		q.sign() = _private::mult_sign(a.sign(), Sign::POS);
		if (a != y && q.sign() == Sign::NEG) {
			q -= 1;
		}
		q.cleanup();
	}
	if constexpr (ignore_remainder) {
		return { q, 0 };
	} else {
		BigInt r_big = a - y;
		uint32_t r = (uint32_t) r_big[0];
		if (r != 0 && is_neg(a)) {
			r = b - r;
		}
		return { q, r };
	}
}

template <is_BigInt_like TLHS, bool ignore_quotient = false, bool ignore_remainder = false>
CONSTEXPR_AUTO
divmod1(const TLHS &a, int32_t bb) -> DivModResult<BigInt, int32_t> { // todo: validate: Isn't uint32_t sufficient for remainder return type?
	if (bb < 0) {
		auto r = divmod1<_private::BigIntNeg<const TLHS>, ignore_quotient, ignore_remainder>(-a, (uint32_t) -bb);
		return DivModResult{std::move(r.d), -(int32_t)r.r};
	} else {
		auto r = divmod1<TLHS, ignore_quotient, ignore_remainder>(a, (uint32_t) bb);
		return DivModResult{std::move(r.d), (int32_t)r.r};
	}
}

}


// division:
namespace bigint {

template <is_BigInt_like TRES, is_BigInt_like TLHS>
CONSTEXPR_VOID
div(TRES &result, TLHS &a, uint32_t b) {
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

template <is_BigInt_like TRES, is_BigInt_like TLHS>
CONSTEXPR_VOID
div(TRES &result, TLHS &a, int32_t bb) {
	if (bb < 0) {
		//_private::BigIntNeg<TLHS>(a)
		auto neg_a = -a; // todo check undefined behavior with -a if result === a, because operator-() const-ifys a?
		div(result, neg_a, (uint32_t) -bb);
	} else {
		div(result, a, (uint32_t) bb);
	}
}


template <is_BigInt_like TLHS, one_of<uint32_t, int32_t> TRHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, TRHS b) -> BigInt {
	BigInt result;
	_private::resizeBigInt(result, a.size());
	div(result, const_cast<TLHS&>(a), b);
	result.cleanup();
	return result;
}

template <is_BigInt_like TLHS, one_of<uint64_t, int64_t> TRHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, TRHS b) -> BigInt {
	return divmod<TLHS, BigIntAdapter<TRHS>, false, true>(a, BigIntAdapter(b)).d;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, false, true>(a, b).d;
}


template <is_BigInt_like TLHS, one_of<uint32_t, int32_t> TRHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, TRHS b) -> TLHS& {
	div(a, a, b);
	a.cleanup();
	return a;
}

template <is_BigInt_like TLHS, one_of<uint64_t, int64_t> TRHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, TRHS b) -> TLHS& {
	const auto result = divmod<TLHS, BigIntAdapter<TRHS>, false, true>(a, BigIntAdapter(b)).d;
	a = std::move(result);
	return a;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, const TRHS &b) -> TLHS& {
	const auto result = divmod<TLHS, TRHS, false, true>(a, b).d;
	a = std::move(result);
	return a;
}

}


// Modulo:
namespace bigint {

template <is_BigInt_like TLHS, one_of<uint32_t, int32_t> TRHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, TRHS b) -> TRHS {
	return divmod1<TLHS, true>(a, b).r; // we can trunccate safely because the divisor only also is uint32_t.
}

template <is_BigInt_like TLHS, one_of<uint64_t, int64_t> TRHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, TRHS b) -> TRHS {
	return divmod<TLHS, true>(a, b).r;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, true>(a, b).r;
}

template <is_BigInt_like TLHS, is_BigInt_like TRHS>
CONSTEXPR_AUTO
operator%=(const TLHS &a, const TRHS &b) -> BigInt& {
	const auto result = divmod<TLHS, TRHS, true>(a, b).r;
	a = std::move(result);
	return a;
}

}


namespace bigint::_private {

constexpr uint8_t calculate_base_power(uint32_t base) {
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
		throw std::string("illegal base! it must be between 2 and 64 (inclusive). Also, please use constexpr!");
	}
}

struct base_conversion {
	consteval base_conversion(uint32_t base) noexcept
		: base_power(calculate_base_power(base)), //  = 19 for base 10;
		division_base(utils::consteval_pow(base, base_power)) {}
	uint8_t base_power;
	uint64_t division_base;
};

template <int base = 10>
CONSTEXPR_AUTO
to_string_padded_generic(uint64_t n, uint64_t len) -> std::string {
	std::string result(len, '0');
	for (uint64_t val = n; len-->0 && val != 0; val/=base)
		result[len] = '0' + val%base;
	return result;
}

template <int base>
CONSTEXPR_AUTO
to_string_generic(const BigInt &v) -> std::string {
	constexpr auto conv = base_conversion(base);

	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();
	tempDig.d.sign() = Sign::POS;

	std::string result;
	while (tempDig.d > 0) {
		tempDig = _private::divmod_ignore_sign(tempDig.d, conv.division_base);
		auto digs = tempDig.r;
		result.insert(0, to_string_padded_generic<base>(digs, conv.base_power));
		tempDig.d.cleanup();
	}
	//       15511210043330985984000000
	// 25! = 1551121  43330985984000000
	const auto index = result.find_first_of("123456789ABCDEF");
	result.erase(0, index);
	if (result.size() == 0) {
		result = "0";
	}
	if (is_neg(v)) {
		result.insert(0, "-");
	}
	return result;
}

template <int base>
CONSTEXPR_AUTO
from_string_generic(const std::string_view input) -> BigInt {
	constexpr auto conv = base_conversion(base);

	BigInt result{0};

	if (input.empty()) {
		return result;
	}

	const size_t i0 = input[0] == '-' or input[0] == '+' ? 1 : 0;

	for (size_t i = i0; i < input.size(); i += conv.base_power) {
		auto substr = std::string_view(input).substr(i, conv.base_power);
		auto mul = substr.size() == conv.base_power ? conv.division_base : (uint64_t)utils::consteval_pow(base, (uint8_t)substr.size());
		auto add = utils::stoull(substr, base);
		result *= mul;
		result += BigInt{add};
	}
	result.sign() = input[0] == '-' ? Sign::NEG : Sign::POS;
	return result;
}

}


// misc math:
namespace bigint {

CONSTEXPR_AUTO
factorial(uint32_t n) -> BigInt {
	BigInt result(1);
	for (uint64_t i = 1; i <= n; ++i) {
		result *= i;
	}
	return result;
}

CONSTEXPR_AUTO
toDigitSum(const BigInt &v) -> uint64_t {
	// 4294967296 =>
	// 1000000000
	// 1111111110
	// 18446744073709551616 =>
	// 10000000000000000000
	constexpr auto base = _private::base_conversion{10}.division_base; // 19 is the larges value for n such that 10^n fits into 64 bits
	// constexpr auto base = uint64_t(10000000000000000000ull);
	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();

	uint64_t sum = 0ull;
	while (tempDig.d > 0) {
		tempDig = _private::divmod_ignore_sign(tempDig.d, base);
		auto digs = tempDig.r;
		while (digs > 00) {
			sum += digs % 10;
			digs /= 10;
		}
		tempDig.d.cleanup();
	}
	return sum;
}

}


// to string & from string:
namespace bigint {

CONSTEXPR_AUTO
to_string_base2(const BigInt &v) -> std::string {
	return _private::to_string_generic<2>(v);
}

CONSTEXPR_AUTO
to_string_base8(const BigInt &v) -> std::string {
	return _private::to_string_generic<8>(v);
}

CONSTEXPR_AUTO
to_string_base10(const BigInt &v) -> std::string {
	return _private::to_string_generic<10>(v);
}

CONSTEXPR_AUTO
to_string_base16(const BigInt &v) -> std::string {
	return _private::to_string_generic<16>(v);
}

CONSTEXPR_AUTO
to_string(const BigInt &v) -> std::string {
	return to_string_base10(v);
}


CONSTEXPR_AUTO
from_string_base2(const std::string_view input) -> BigInt {
	return _private::from_string_generic<2>(input);
}

CONSTEXPR_AUTO
from_string_base8(const std::string_view input) -> BigInt {
	return _private::from_string_generic<8>(input);
}

CONSTEXPR_AUTO
from_string_base10(const std::string_view input) -> BigInt {
	return _private::from_string_generic<10>(input);
}

CONSTEXPR_AUTO
from_string_base16(const std::string_view input) -> BigInt {
	return _private::from_string_generic<16>(input);
}

CONSTEXPR_AUTO
from_string(const std::string_view input) -> BigInt {
	return from_string_base10(input);
}


inline std::ostream & operator<<(std::ostream &ostream, const BigInt &val) {
	return ostream << to_string(val);
}

}

#endif /* __BIG_INT_H__ */
