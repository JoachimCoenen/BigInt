#ifndef __BIG_INT_H__
#define __BIG_INT_H__

#include <vector>
#include <string>
#include <limits>
#include <math.h>

#include <type_traits>

#define LOGGING(a)

namespace bigint {

template<class D, class R=D>
struct DivModResult { D d; R r; };

class IBigIntLike { };

template <typename T>
using isBigIntLike = typename std::enable_if<std::is_base_of<IBigIntLike, T>::value, T>::type;

template <typename T, typename Tt>
using isBigIntLikeT1 = typename std::enable_if<std::is_base_of<IBigIntLike, T>::value, Tt>::type;

template <typename T1, typename T2, typename Tt>
using isBigIntLikeT2 = typename std::enable_if<std::is_base_of<IBigIntLike, T1>::value && std::is_base_of<IBigIntLike, T2>::value, Tt>::type;

template <typename T>
struct BigIntBox_t {
	using type = T;

};

class BigInt;

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
class BigIntUnaryOperation : IBigIntLike {
public:
	BigIntUnaryOperation(const BigIntBox<T> lhs) : _lhs(lhs) {}

	inline std::size_t size() const { return _lhs.size(); }

	inline uint64_t operator[](std::size_t index) const {
		return _lhs[index];
	}

	inline uint64_t& operator[](std::size_t index) {
		return _lhs[index];
	}
protected:
	const BigIntBox<T> _lhs;
};

template <typename TLHS, typename TRHS>
class BigIntBinaryOperation : IBigIntLike {
public:
	BigIntBinaryOperation(const BigIntBox<TLHS> lhs, const BigIntBox<TRHS> rhs) :
		_lhs(lhs),
		_rhs(rhs) {}

	inline std::size_t size() const { return _lhs.size(); }

	inline uint64_t operator[](std::size_t index) const {
		return _lhs[index];
	}

	inline uint64_t& operator[](std::size_t index) {
		return _lhs[index];
	}

protected:
	const BigIntBox<TLHS> lhs() {
		return _lhs;
	}

	const BigIntBox<const TLHS> lhs() const {
		return _lhs;
	}

	const BigIntBox<TRHS> rhs() {
		return _rhs;
	}

	const BigIntBox<const TRHS> rhs() const {
		return _rhs;
	}

private:
	const BigIntBox<TLHS> _lhs;
	const BigIntBox<TRHS> _rhs;
};


inline static BigInt mult(uint64_t a, uint64_t b);

std::string to_string_base2(const BigInt &v);
std::string to_string_base8(const BigInt &v);
std::string to_string_base10(const BigInt &v);
std::string to_string_base16(const BigInt &v);
std::string to_string(const BigInt &v);

BigInt from_string_base2(const std::string &input);
BigInt from_string_base8(const std::string &input);
BigInt from_string_base10(const std::string &input);
BigInt from_string_base16(const std::string &input);
BigInt from_string(const std::string &input);

class BigInt : IBigIntLike
{

 public:
	BigInt(): BigInt(0) {}

	explicit BigInt(uint64_t v)
	{ _data.push_back(v); }

	explicit BigInt(const std::string &v)
		: BigInt(std::move(from_string(v))) {}

	explicit BigInt(const std::vector<uint64_t> &v)
		: _data(v) {} // todo: maby make it non-public?

	explicit BigInt(const std::vector<uint64_t> &&v)
		: _data(std::move(v)) {} // todo: maby make it non-public?

	BigInt(const std::initializer_list<uint64_t> v)
		: _data(v) {} // todo: maby make it non-public?

	~BigInt() {}


	BigInt(const BigInt &other) :// copy constructor
		_data(other._data) {
		LOGGING("copying: " << _data.size());
	}


	inline BigInt &operator=(const BigInt &other) { // copy assignment
		_data = other._data;
		LOGGING("assigning: " << _data.size());
		return *this;
	}

	inline std::size_t size() const { return _data.size(); }

	inline uint64_t operator[](std::size_t index) const {
		return (index >= size()) ? 0ull : _data.at(index);
	}

	inline uint64_t &operator[](std::size_t index)  {
		if (index >= _data.size()) {
			throw std::string(" index out of bound!");
		}
		return _data.at(index);
	}

	inline void append(uint64_t v) { _data.push_back(v); }

	inline void insertFront(uint64_t v) { _data.insert(_data.begin(), v); }

	inline void cleanup() const { // todo: const???!
		for (auto i = _data.size(); 0 <--i;) {
			if (_data[i] == 0) {
				_data.pop_back();
			} else {
				break;
			}
		}
	}

private:
	friend inline void resizeBigInt(BigInt &a, std::size_t size);

public:
	const std::vector<uint64_t>& __data_for_testing_only() const {
		return _data;
	}

private:
	mutable std::vector<uint64_t> _data; // only for cleanups! // todo: mutable???!
	//uint32_t _lshift = 0;
};


template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> toBigInt(const TLHS v) {
	BigInt result;
	for (int i = 0; i < v.size(); ++i) {
		result.append(v[i]);
	}
	return result;
}



#define BIG_INT_BINARY_OPERATOR(name, tlhs, trhs) class name : BigIntBinaryOperation<tlhs, trhs> { \
	using Base = BigIntBinaryOperation<tlhs, trhs>; \
	public: \
	name(BigIntBox<tlhs> lhs, BigIntBox<trhs> rhs) : Base(lhs, rhs) {}

#define BIG_INT_UNARY_OPERATOR(name, tlhs) class name : BigIntUnaryOperation<tlhs> { \
	using Base = BigIntUnaryOperation<tlhs>; \
	name(BigIntBox<tlhs> lhs) : Base(lhs) {}



inline void resizeBigInt(BigInt &a, std::size_t size) {
	a._data.resize(size, 0);
}

inline BigInt makeBigIntWithSize(std::size_t size) {
	auto r = BigInt();
	resizeBigInt(r, size); //OCTOPUS
	return r;
}

template <typename T>
BIG_INT_BINARY_OPERATOR(BigIntLShifted, T, uint64_t)
	inline std::size_t size() const { return Base::lhs().size() + Base::rhs(); }

	inline uint64_t operator[](std::size_t index) const {
		return (index < Base::rhs()) ? 0 : Base::lhs()[index - Base::rhs()];
	}

	inline uint64_t& operator[](std::size_t index) {
		return (index < Base::rhs()) ? 0 : Base::lhs()[index - Base::rhs()]; // todo dont return 0 when out of bounds!
	}

};

template <typename TLHS>
inline auto lshifted(const TLHS &a, const uint64_t &b) {
	return BigIntLShifted<const TLHS>(a, b);
}

template <typename TLHS>
inline auto lshiftedView(TLHS &a, const uint64_t &b) {
	return BigIntLShifted<TLHS>(a, b);
}

template <typename T>
BIG_INT_BINARY_OPERATOR(BigIntRShifted, T, uint64_t)
	inline std::size_t size() const { return Base::lhs().size() - Base::rhs(); }

	inline uint64_t operator[](std::size_t index) const {
		if (index < 0) {
			throw std::string(" index out of bound!");
		}
//		Base::_lhs[index] = 7;
		return Base::lhs()[index + Base::rhs()];
	}

	inline uint64_t &operator[](std::size_t index) {
		if (index < 0) {
			throw std::string(" index out of bound!");
		}
		return Base::lhs()[index + Base::rhs()];
	}

};

template <typename TLHS>
inline auto rshifted(TLHS &a, const uint64_t &b) {
	return BigIntRShifted<TLHS>(a, b);
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, bool> operator<(const TLHS &a, const TRHS &b) {
	std::size_t i;
	for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
		// do nothin'
	}
	return a[i] < b[i];
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, bool> operator>(const TLHS &a, const TRHS &b) {
	std::size_t i;
	for (i = std::max(a.size(), b.size()); i-->1 && a[i] == b[i];) {
		// do nothin'
	}
	return a[i] > b[i];
}

template <typename TLHS>
inline isBigIntLikeT1<TLHS, bool> operator<(const TLHS &a, const uint64_t &b) {
	return a < BigInt(b);
}

template <typename TLHS>
inline isBigIntLikeT1<TLHS, bool> operator>(const TLHS &a, const uint64_t &b) {
	return a > BigInt(b);
}


template <typename TRES, typename TLHS, typename TRHS>
inline static isBigIntLikeT2<TRES, TLHS, void> add(TRES &result, TLHS &a, const TRHS &b) {
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

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, BigInt> operator+(const TLHS &a, const TRHS &b) {
	BigInt result = makeBigIntWithSize(std::max(a.size(), b.size()));
	add(result, a, b);
	return result;
}

template <typename TRHS>
inline isBigIntLikeT1<TRHS, BigInt>& operator+=(BigInt &a, const TRHS &b) {
	resizeBigInt(a, std::max(a.size(), b.size()));
	add(a, a, b);
	return a;
}


template <typename TRES, typename TLHS, typename TRHS>
inline static isBigIntLikeT2<TRES, TLHS, void> sub(TRES &result, TLHS &a, const TRHS &b) {
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

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, BigInt> operator-(const TLHS &a, const TRHS &b) {
	BigInt result = makeBigIntWithSize(std::max(a.size(), b.size()));
	sub(result, a, b);
	result.cleanup();
	return result;
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, TLHS>& operator-=(TLHS &a, const TRHS &b) {
	sub(a, a, b);
	return a;
}


inline static BigInt mult(uint64_t a, uint64_t b) {
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
	return {r, c};
}

template <typename TRES, typename TLHS>
inline static isBigIntLikeT2<TRES, TLHS, void> mult(TRES &result, TLHS &a, uint64_t b) {
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

template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> operator*(const TLHS &a, uint64_t b) {
	BigInt result = makeBigIntWithSize(a.size());
	mult(result, a, b);
	return  result;
}

template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> &operator*=(TLHS &a, uint64_t b) {
	mult(a, a, b);
	return a;
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, BigInt> operator*(const TLHS &a, const TRHS &b) {
	BigInt result = makeBigIntWithSize(a.size() + b.size());

	for (auto i = 0ull; i < b.size(); i++) {
		auto temp = a * b[i];
		result += lshifted(temp, i);
	}

	result.cleanup();
	LOGGING("multiplying: " << result.size());
	return result;
}


template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> lshiftBits(const TLHS &vec, uint64_t digits) {
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
inline isBigIntLikeT1<TLHS, BigInt> rshiftBits(const TLHS &vec, uint64_t digits) {
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

template <typename TRHS>
inline isBigIntLikeT1<TRHS, DivModResult<BigInt>> divmod(const BigInt &a, const TRHS &b) {
	DivModResult<BigInt> r;
	r.r = a;
	resizeBigInt(r.d, a.size() - b.size() + 1);

	a.cleanup();
	if (a.size() == 1 && a[0] == 0) {
		return {BigInt(0), BigInt(0)};
	}

	const auto aHi = a[a.size()-1];
	const auto ad  = a.size()*64 - (aHi==0 ? 64 : __builtin_clzll(aHi));
	const auto b2Hi = b[b.size()-1];
	const auto b2d = b.size()*64 - __builtin_clzll(b2Hi);
	const auto adbd2 = int64_t(ad - b2d);
	//uint64_t min = std::max(0ll, adbd2 - 1);
	uint64_t max = std::max(0ll, adbd2 + 1ll);//%64; //std::numeric_limits<uint64_t>::max();
	uint64_t k;
	uint64_t i = max;//(a.size() - b.size())*64;
	auto a2IsSmaller = false;

	while (a2IsSmaller || !(r.r < b)) {
		--i;
		auto p2 = lshiftBits(b, i%64);
		a2IsSmaller = rshifted(r.r, i/64) < p2;
		if (! a2IsSmaller) {
			r.d[i/64] |= 1ull << i % 64;
			auto rshift = rshifted(r.r, i/64);
			rshift -= p2;
		}
	}
	LOGGING("divmod: " << a.size()<< " : " << b.size());
	r.d.cleanup();
	return r;
}

inline DivModResult<BigInt, uint64_t> divmod(const BigInt &a, const uint64_t &b) {
	const auto res = divmod(a, BigInt(b));
	return { res.d, res.r[0] };
}

template <typename TLHS>
inline DivModResult<BigInt, uint64_t> divmod1(const TLHS &a, const uint32_t &b) {
	BigInt d;
	resizeBigInt(d, a.size());
	BigInt y;
	resizeBigInt(y, d.size());

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
				   d[i] = di;

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
	return { d, r[0] };
}

// Division:

template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> operator/(const TLHS &a, const uint32_t &b) {
	BigInt result;
	resizeBigInt(result, a.size());

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

	return result;
}

template <typename TLHS>
inline isBigIntLikeT1<TLHS, BigInt> operator/(const TLHS &a, const uint64_t &b) {
	return divmod(a, b).d;
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, BigInt> operator/(const TLHS &a, const TRHS &b) {
	return divmod(a, b).d;
}

// Modulo:

template <typename TLHS>
inline isBigIntLikeT1<TLHS, uint32_t> operator%(const TLHS &a, const uint32_t &b) {
	BigInt y;
	resizeBigInt(y, a.size());

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
	return r[0];
}

inline uint64_t operator%(const BigInt &a, const uint64_t &b) {
	return divmod(a, b).r;
}

template <typename TLHS, typename TRHS>
inline isBigIntLikeT2<TLHS, TRHS, BigInt> operator%(const TLHS &a, const TRHS &b) {
	return divmod(a, b).r;
}


BigInt factorial(uint32_t n);

uint64_t toDigitSum(const BigInt &v);

inline std::ostream & operator<<(std::ostream &ostream, const BigInt &val) {
	return ostream << to_string(val);
}

}

#endif /* __BIG_INT_H__ */
