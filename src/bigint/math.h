#pragma once

#include "bigInt.h"

#include "_bigint_tracy_defines.h"


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
	BigInt temp = base % mod;
	BigInt temp2;
	BigInt temp3;
	std::vector<uint64_t> temp_mod;
	for (uint64_t i = 0; i < exp_bits; ++i) {
		const auto mask = 1ull << (i % 64);
		if (exp[i/64] & mask) {
			// result = (result * temp) % mod;
			mult(temp2, result, temp);
			// result = temp2 % mod;
			result = _private::divmod<BigInt, MOD, true, false>(temp2, mod, temp_mod, std::move(result)).r;
		}
		if (i+1 < exp_bits) {
			// temp = (temp * temp) % mod;
			mult(temp3, temp, temp);
			// temp = temp3 % mod;
			temp = _private::divmod<BigInt, MOD, true, false>(temp3, mod, temp_mod, std::move(temp)).r;
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



#include "_bigint_tracy_undefines.h"
