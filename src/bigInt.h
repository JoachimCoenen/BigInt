#ifndef __BIG_INT_H__
#define __BIG_INT_H__

#include "utils.h"

#include <cstdint>
#include <math.h>
#include <vector>
#include <string>
#include <type_traits>

#define LOGGING(a)

namespace bigint {

class IBigIntLike { };

template <typename T>
concept is_BigInt_like1 = std::is_base_of_v<IBigIntLike, T>;

template <typename T1, typename T2>
concept is_BigInt_like2 = std::is_base_of_v<IBigIntLike, T1> && std::is_base_of_v<IBigIntLike, T2>;

template <typename T1, typename T2, typename T3>
concept is_BigInt_like3 = std::is_base_of_v<IBigIntLike, T1> && std::is_base_of_v<IBigIntLike, T2> && std::is_base_of_v<IBigIntLike, T3>;

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
from_string_base2(const std::string &input) -> BigInt;

CONSTEXPR_AUTO
from_string_base8(const std::string &input) -> BigInt;

CONSTEXPR_AUTO
from_string_base10(const std::string &input) -> BigInt;

CONSTEXPR_AUTO
from_string_base16(const std::string &input) -> BigInt;

CONSTEXPR_AUTO
from_string(const std::string &input) -> BigInt;

}


namespace bigint::_private {
CONSTEXPR_VOID
resizeBigInt(BigInt &a, std::size_t size);

}


// class BigInt:
namespace bigint {

class BigInt : public IBigIntLike
{

 public:
	constexpr
	BigInt(): BigInt(0) {}

	explicit constexpr
	BigInt(uint64_t v)
		: _data({v})
	{ }

	explicit constexpr
	BigInt(const std::string &v)
		: BigInt(std::move(from_string(v))) {}

	explicit constexpr
	BigInt(const std::vector<uint64_t> &v)
		: _data(v) {} // todo: maby make it non-public?

	explicit constexpr
	BigInt(const std::vector<uint64_t> &&v)
		: _data(std::move(v)) {} // todo: maby make it non-public?

	constexpr
	~BigInt() {}

	constexpr
	BigInt(const BigInt &other) // copy constructor
		: _data(other._data) {
		LOGGING("copying: " << _data.size());
	}

	CONSTEXPR_AUTO_DISCARD
	operator=(const BigInt &other) -> BigInt& { // copy assignment
		_data = other._data;
		LOGGING("assigning: " << _data.size());
		return *this;
	}

	// CONSTEXPR_AUTO_DISCARD
	// operator=(const BigInt &&other) -> BigInt& { // move assignment
	// 	todo: move assignement
	// }

	CONSTEXPR_AUTO
	size() const -> std::size_t {
		return _data.size();
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return (index >= size()) ? 0ull : _data.at(index);
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) -> uint64_t& {
		if (index >= _data.size()) {
			throw std::string(" index out of bound!");
		}
		return _data.at(index);
	}

	CONSTEXPR_VOID
	append(uint64_t v) { _data.push_back(v); }

	CONSTEXPR_VOID
	insertFront(uint64_t v) { _data.insert(_data.begin(), v); }

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

public:
	[[nodiscard]] auto
	__data_for_testing_only() const -> const std::vector<uint64_t>&{
		return _data;
	}

private:
	mutable std::vector<uint64_t> _data; // only for cleanups! // todo: mutable???!
	//uint32_t _lshift = 0;
};

}


// resize
namespace bigint::_private {
CONSTEXPR_VOID
resizeBigInt(BigInt &a, std::size_t size) {
	a.resize(size);
}

CONSTEXPR_AUTO
makeBigIntWithSize(std::size_t size) -> BigInt{
	auto r = BigInt();
	resizeBigInt(r, size); //OCTOPUS
	return r;
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
	size() const -> std::size_t {
		return lhs().size() + _rhs;
	}

	CONSTEXPR_AUTO
	operator[](std::size_t index) const -> uint64_t {
		return (index < _rhs) ? 0 : lhs()[index - _rhs];
	}

//	inline uint64_t& operator[](std::size_t index) {
//		return (index < rhs) ? 0 : lhs[index - _rhs]; // todo dont return 0 when out of bounds!
//	}

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

template <typename TLHS>
inline auto lshifted(const TLHS &a, uint64_t b) {
	return BigIntLShifted<const TLHS>(a, b);
}

template <typename T>
class BigIntRShifted : IBigIntLike {
public:
	constexpr BigIntRShifted(const BigIntBox<T> lhs, const uint64_t rhs) :
		_lhs(lhs), _rhs(rhs) {}

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

	CONSTEXPR_AUTO
	operator[](std::size_t index) -> uint64_t& {
		if (index < 0) {
			throw std::string(" index out of bound!");
		}
		return lhs()[index + _rhs];
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

template <typename TLHS>
CONSTEXPR_AUTO
rshifted(const TLHS &a, uint64_t b) {
	return BigIntRShifted<const TLHS>(a, b);
}

template <typename TLHS>
CONSTEXPR_AUTO
rshifted(TLHS &a, uint64_t b) {
	return BigIntRShifted<TLHS>(a, b);
}

}


// bitwise shift operations:
namespace bigint {

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
lshiftBits(const TLHS &vec, uint64_t digits) -> BigInt {
	BigInt result(std::vector<uint64_t>(digits / 64, 0));
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

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
rshiftBits(const TLHS &vec, uint64_t digits) -> BigInt {
	BigInt result;//(digits / 64, 0);
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

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator<(const TLHS &a, const TRHS &b) {
	std::size_t i;
	for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
		// do nothin'
	}
	return a[i] < b[i];
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator>(const TLHS &a, const TRHS &b) {
	std::size_t i;
	for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
		// do nothin'
	}
	return a[i] > b[i];
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator<(const TLHS &a, const uint64_t &b) {
	return a < BigInt(b);
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator>(const TLHS &a, const uint64_t &b) {
	return a > BigInt(b);
}

}


// addition:
namespace bigint {

template <typename TRES, typename TLHS>
	requires is_BigInt_like2<TRES, TLHS>
CONSTEXPR_VOID
add(TRES &result, TLHS &a, const uint64_t b) {
	result[0] = a[0] + b;
	uint8_t c = (result[0] < b) ? 1: 0; // carry

	// propagate carry:
	for (auto i = 1ull; c && i < result.size(); i++) {
		result[i] = a[i] + c;
		c = (result[i] < c) ? 1: 0;
	}
	if (c) {
		result.append(c);
	}
	LOGGING("adding: " << result.size());
}

template <typename TRES, typename TLHS, typename TRHS>
	requires is_BigInt_like3<TRES, TLHS, TRHS>
CONSTEXPR_VOID
add(TRES &result, TLHS &a, const TRHS &b) {
	uint8_t c = 0; // carry
	for (auto i = 0ull; i < result.size(); i++) {
		const auto bc = b[i] + c;
		result[i] = a[i] + bc;
		c = (result[i] < bc) ? 1: 0;
	}
	if (c) {
		result.append(c);
	}
	LOGGING("adding: " << result.size());
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator+(const TLHS &a, uint64_t b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(a.size());
	add(result, a, b);
	return result;
}

template <typename TRHS>
	requires is_BigInt_like1<TRHS>
CONSTEXPR_AUTO
operator+(uint64_t a, const TRHS &b) -> BigInt {
	return b + a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator+(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(std::max(a.size(), b.size()));
	add(result, a, b);
	return result;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, uint64_t b) -> TLHS& {
	_private::resizeBigInt(a, a.size());
	add(a, a, b);
	return a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO_DISCARD
operator+=(TLHS &a, const TRHS &b) -> TLHS& {
	_private::resizeBigInt(a, std::max(a.size(), b.size()));
	add(a, a, b);
	return a;
}

}


// subtraction:
namespace bigint {

template <typename TRES, typename TLHS>
	requires is_BigInt_like2<TRES, TLHS>
CONSTEXPR_VOID
sub(TRES &result, const TLHS &a, uint64_t b) {
	uint8_t c = (a[0] < b) ? 1: 0; // carry
	result[0] = a[0] - b;

	// propagate cary:
	for (auto i = 1ull; i < result.size(); i++) {
		const auto temp = c;
		c = (a[i] < c) ? 1: 0;
		result[i] = a[i] - temp;
	}
	if (c) {
		throw std::string("negative number!");
	}
	LOGGING("subtracting: " << result.size());
}

template <typename TRES, typename TLHS, typename TRHS>
	requires is_BigInt_like3<TRES, TLHS, TRHS>
CONSTEXPR_VOID
sub(TRES &result, const TLHS &a, const TRHS &b) {
	uint8_t c = 0; // carry
	for (auto i = 0ull; i < result.size(); i++) {
		const auto bc = b[i] + c;
		c = (a[i] < bc) ? 1: 0;
		result[i] = a[i] - bc;
	}
	if (c) {
		throw std::string("negative number!");
	}
	LOGGING("subtracting: " << result.size());
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator-(const TLHS &a, uint64_t b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(a.size());
	sub(result, a, b);
	return result;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator-(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(std::max(a.size(), b.size()));
	sub(result, a, b);
	result.cleanup();
	return result;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, uint64_t b) -> TLHS& {
	_private::resizeBigInt(a, a.size());
	sub(a, a, b);
	a.cleanup();
	return a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO_DISCARD
operator-=(TLHS &a, const TRHS &b) -> TLHS& {
	sub(a, a, b);
	a.cleanup();
	return a;
}


}


// multiplication:
namespace bigint {

CONSTEXPR_AUTO
mult(uint64_t a, uint64_t b) -> BigInt {
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
	return BigInt{std::vector<uint64_t>{r, c}};
}

template <typename TRES, typename TLHS>
	requires is_BigInt_like2<TRES, TLHS>
CONSTEXPR_VOID
mult(TRES &result, TLHS &a, uint64_t b) {
	uint64_t c = 0; // carry
	for (auto i = 0ull; i < a.size(); i++) {
		const auto rc = mult(a[i], b);
		result[i] = rc[0] + c;
		c = rc[1] + (result[i] < c ? 1 : 0); // account for addition overflow
	}
	if (c){
		result.append(c);
	};
	LOGGING("multiplying: " << result.size());
}

template <typename TRES, typename TLHS, typename TRHS>
	requires is_BigInt_like3<TRES, TLHS, TRHS>
CONSTEXPR_VOID
mult(TRES &result, const TLHS &a, const TRHS &b) {
	for (auto i = 0ull; i < b.size(); i++) {
		auto temp = a * b[i];
		result += _private::lshifted(temp, i);
	}
	LOGGING("multiplying: " << result.size());
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator*(const TLHS &a, uint64_t b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(a.size());
	mult(result, a, b);
	return result;
}

template <typename TRHS>
	requires is_BigInt_like1<TRHS>
CONSTEXPR_AUTO
operator*(uint64_t a, const TRHS &b) -> BigInt {
	return b * a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator*(const TLHS &a, const TRHS &b) -> BigInt {
	BigInt result = _private::makeBigIntWithSize(a.size() + b.size());
	mult(result, a, b);
	result.cleanup();
	return result;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO_DISCARD
operator*=(TLHS &a, uint64_t b) -> BigInt& {
	mult(a, a, b);
	return a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator*=(TLHS &a, const TRHS &b) -> BigInt& {
	mult(a, a, b);
	a.cleanup();
	return a;
}

}


// divmod:
namespace bigint {

template<class D, class R=D>
struct DivModResult { D d; R r; };

template <typename TLHS, typename TRHS, bool ignore_quotient = false>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
divmod(const TLHS &a, const TRHS &b) -> DivModResult<BigInt> {
	DivModResult<BigInt> r;
	r.r = a;
	if constexpr (!ignore_quotient) {
		_private::resizeBigInt(r.d, a.size() - b.size() + 1);
	}

	a.cleanup();
	if (a.size() == 1 && a[0] == 0) { // todo handle 0/0 !
		return {BigInt(0), BigInt(0)};
	}

	const auto aHi = a[a.size()-1];
	const auto ad  = a.size()*64 - (aHi==0 ? 64 : __builtin_clzll(aHi));
	const auto b2Hi = b[b.size()-1];
	const auto b2d = b.size()*64 - __builtin_clzll(b2Hi);
	const auto adbd2 = int64_t(ad - b2d);
	//uint64_t min = std::max(0ll, adbd2 - 1);
	const uint64_t max = std::max(0ll, adbd2 + 1ll);//%64; //std::numeric_limits<uint64_t>::max();
	uint64_t i = max;//(a.size() - b.size())*64;
	auto a2IsSmaller = false;

	while (a2IsSmaller || !(r.r < b)) {
		--i;
		auto p2 = lshiftBits(b, i%64);
		a2IsSmaller = _private::rshifted(r.r, i/64) < p2;
		if (! a2IsSmaller) {
			if constexpr (!ignore_quotient) {
				r.d[i/64] |= 1ull << i % 64;
			}
			auto rshift = _private::rshifted(r.r, i/64);
			rshift -= p2;
		}
	}
	LOGGING("divmod: " << a.size()<< " : " << b.size());
	r.r.cleanup();
	if constexpr (!ignore_quotient) {
		r.d.cleanup();
	}
	return r;
}

template <typename TLHS, bool ignore_quotient = false>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
divmod(const TLHS &a, const uint64_t &b) -> DivModResult<BigInt, uint64_t> {
	const auto res = divmod<TLHS, BigInt, ignore_quotient>(a, BigInt(b));
	return { res.d, res.r[0] };
}

template <typename TLHS, bool ignore_quotient = false>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
divmod1(const TLHS &a, const uint32_t &b) -> DivModResult<BigInt, uint64_t> { // todo: validate: Isn't uint32_t sufficient for remainder return type?
	BigInt q;
	if constexpr (!ignore_quotient) {
		_private::resizeBigInt(q, a.size());
	}
	BigInt y;
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
		y[i] = yi;

	}

	BigInt r = a - y;
	return { q, r[0] };
}

}


// division:
namespace bigint {

template <typename TRES, typename TLHS>
	requires is_BigInt_like2<TRES, TLHS>
CONSTEXPR_VOID
div(TRES &result, const TLHS &a, uint32_t b) {
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
		result[i] = r;
	}
}


template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, const uint32_t &b) -> BigInt {
	BigInt result;
	_private::resizeBigInt(result, a.size());
	div(result, a, b);
	return result;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, const uint64_t &b) -> BigInt {
	return divmod(a, b).d;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator/(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod(a, b).d;
}


template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, const uint32_t &b) -> BigInt& {
	div(a, a, b);
	return a;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, const uint64_t &b) -> BigInt& {
	const auto result = divmod(a, b).d;
	a = std::move(result);
	return a;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator/=(TLHS &a, const TRHS &b) -> BigInt& {
	const auto result = divmod(a, b).d;
	a = std::move(result);
	return a;
}

}


// Modulo:
namespace bigint {

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, const uint32_t &b) -> uint32_t {
	return divmod1<TLHS, true>(a, b).r;
}

template <typename TLHS>
	requires is_BigInt_like1<TLHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, const uint64_t &b) -> uint64_t {
	return divmod<TLHS, true>(a, b).r;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator%(const TLHS &a, const TRHS &b) -> BigInt {
	return divmod<TLHS, TRHS, true>(a, b).r;
}

template <typename TLHS, typename TRHS>
	requires is_BigInt_like2<TLHS, TRHS>
CONSTEXPR_AUTO
operator%=(const TLHS &a, const TRHS &b) -> BigInt& {
	const auto result = divmod<TLHS, TRHS, true>(a, b).r;
	a = std::move(result);
	return a;
}

}


namespace bigint::_private {
struct base_conversion {
	constexpr base_conversion(uint32_t base) noexcept
		: base_power(floor(64 / log2(base))), //  = 19 for base 10;
		division_base(std::pow(base, base_power)) {}
	uint32_t base_power;
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
	const auto conv = base_conversion(base);

	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();

	std::string result;
	while (tempDig.d > 0) {
		tempDig = divmod(tempDig.d, conv.division_base);
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
	return result;
}

template <int base>
CONSTEXPR_AUTO
from_string_generic(const std::string &input) -> BigInt {
	const auto conv = base_conversion(base);

	BigInt result{0};

	for (size_t i = 0; i < input.size(); i += conv.base_power) {
		auto substr = std::string_view(input).substr(i, conv.base_power);
		auto mul = substr.size() == conv.base_power ? conv.division_base : uint64_t(std::pow(base, substr.size()));
		auto add = utils::stoull(substr, base);
		result *= mul;
		result += BigInt{add};
	}
	//	result.cleanup();
	return result;
}

}


// misc math:
namespace bigint {

CONSTEXPR_AUTO
factorial(uint64_t n) -> BigInt {
	BigInt result(1);
	for (uint32_t i = 1; i <= n; ++i) {
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
	const auto base = _private::base_conversion{10}.division_base; // 19 is the larges value for n such that 10^n fits into 64 bits
	// constexpr auto base = uint64_t(10000000000000000000ull);
	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();

	uint64_t sum = 0ull;
	while (tempDig.d > 0) {
		tempDig = divmod(tempDig.d, base);
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
from_string_base2(const std::string &input) -> BigInt {
	return _private::from_string_generic<2>(input);
}

CONSTEXPR_AUTO
from_string_base8(const std::string &input) -> BigInt {
	return _private::from_string_generic<8>(input);
}

CONSTEXPR_AUTO
from_string_base10(const std::string &input) -> BigInt {
	return _private::from_string_generic<10>(input);
}

CONSTEXPR_AUTO
from_string_base16(const std::string &input) -> BigInt {
	return _private::from_string_generic<16>(input);
}

CONSTEXPR_AUTO
from_string(const std::string &input) -> BigInt {
	return from_string_base10(input);
}


inline std::ostream & operator<<(std::ostream &ostream, const BigInt &val) {
	return ostream << to_string(val);
}

}

#endif /* __BIG_INT_H__ */
