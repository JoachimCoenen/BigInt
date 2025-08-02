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
BIGINT_TRACY_CONSTEXPR_AUTO
sqrt(const is_BigInt_like auto& y) -> BigInt {
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
BIGINT_TRACY_CONSTEXPR_AUTO
calculate_squares(const utils::Span<const uint64_t>& base, const utils::Span<const uint64_t>& y) -> std::vector<DigitsVec> {
	constexpr uint8_t exp_bits_max = 64;
	std::vector<DigitsVec> squares;

	utils::UniquePtr<KaratsubaStepTemps> karatsuba_temps;

	for (uint8_t i = 1; i < exp_bits_max; ++i) {
		utils::Span last_square = i == 1 ? base : utils::Span<uint64_t>{squares.back()};

		squares.emplace_back(last_square.size() * 2);
		utils::Span<uint64_t> square{squares.back()};
		mult_ignore_sign(square, last_square, last_square, karatsuba_temps);
		cleanup(squares.back());
		if (square <=> y > 0) { // if (square > y)
			squares.pop_back();
			break;
		}
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
BIGINT_TRACY_CONSTEXPR_AUTO
log(const is_BigInt_like auto& base, const is_BigInt_like auto& y) -> uint64_t {
	if (base <= 1) {
		throw std::domain_error{utils::error_msg("integer log for a base less or equal to one is undefined.")};
	}
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	if (base > y) {
		return 0;
	}

	const auto squares = _private::calculate_squares(base._span(), y._span());

	uint64_t result = 0;
	BigInt temp{y};
	BigInt temp2;
	BigInt reminder; // not used
	DigitsVec temp_d, temp_af, temp_bf;

	for (auto i = static_cast<uint8_t>(squares.size()); i --> 0;) {
		const utils::Span<const uint64_t> square (squares[i]);
		if (square <=> temp._span() <= 0) {  // (square <= temp)
			// temp2 = temp / square:
			_private::divmod_ignore_sign<false, true>(temp2, reminder, temp._span(), square, temp_d, temp_af, temp_bf);
			std::swap(temp, temp2);

			uint64_t mask = 1ull << (i + 1);
			result |= mask;
		}
	}
	if (base <= temp) {
		uint64_t mask = 1ull << 0;
		result |= mask;
	}
	return result;
}


/**
 * @brief calculates the integer logarithm of `y` for base 10. E.g.: `log10(1000) == 3`.
 * @param y the value to get the logarithm of.
 * @return the integer logarithm base 10 of y.
 * @throws std::domain_error if y <= 0
 */
BIGINT_TRACY_CONSTEXPR_AUTO
log10(const is_BigInt_like auto& y) -> uint64_t {
	BIGINT_TRACY_ZONE_SCOPED;
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	return log(_private::IntegralAdapter{10}, y);
}


/**
 * @brief calculates the integer logarithm of `y` for base 2. E.g.: `log2(7) == 3`. Runs in O(1) time.
 * @param y the value to get the logarithm of.
 * @return the integer logarithm base 2 of y.
 * @throws std::domain_error if y <= 0
 */
BIGINT_TRACY_CONSTEXPR_AUTO
log2(const is_BigInt_like auto& y) -> BigInt::size_type {
	BIGINT_TRACY_ZONE_SCOPED;
	if (!is_pos(y)) {
		throw std::domain_error{utils::error_msg("integer log of a non-positive number is undefined.")};
	}

	return 64 * y.size() - utils::clzll(y[y.size() - 1]) - 1;
}


/**
 * @brief raises `base` to the power of `exp`. E.g.: `pow(10, 3) == 1000`. if you need to calculate `pow(a, b) % m` use `pow_mod()` instead.
 * @param base the base.
 * @param exp the exponent.
 * @return base^exp.
 * @throws std::domain_error if base == exp == 0
 */
BIGINT_TRACY_CONSTEXPR_AUTO
pow(const is_BigInt_like auto& base, uint64_t exp) -> BigInt {
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
	BigInt temp2;

	utils::UniquePtr<KaratsubaStepTemps> karatsuba_temps;

	const auto exp_bits = static_cast<uint8_t>(64 - utils::clzll(exp));
	for (uint8_t i = 0; i < exp_bits; ++i) {
		const auto mask = 1ull << i;
		if (exp & mask) {
			//result *= temp;
			mult(temp2, result, temp, karatsuba_temps);
			std::swap(result, temp2);
		}
		if (i+1 < exp_bits) {
			mult(temp2, temp, temp, karatsuba_temps);
			std::swap(temp, temp2);
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
BIGINT_TRACY_CONSTEXPR_AUTO
pow_mod(const is_BigInt_like auto& base, const is_BigInt_like auto& exp, const is_BigInt_like auto& mod) -> BigInt {
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

	DigitsVec temp_mod;
	DigitsVec temp_af;
	DigitsVec temp_bf;
	temp_mod.resize(mod.size() + 2);
	temp_af.resize(mod.size() * 2 + 1);
	temp_bf.resize(mod.size() + 1);
	BigInt result{1};
	BigInt temp; // = base % mod;
	BigInt temp2;
	BigInt temp3;
	utils::UniquePtr<KaratsubaStepTemps> karatsuba_temps;

	result.reserve(mod.size() * 2 + 2);
	temp.reserve(mod.size() * 2 + 2);
	temp2.reserve(mod.size() * 2);
	temp3.reserve(mod.size() * 2);

	// temp = base % mod;
	_private::divmod<true, false>(result, temp, base, mod, temp_mod, temp_af, temp_bf); // result is just a placeholder here and is never read from or written to.

	for (uint64_t i = 0; i < exp_bits; ++i) {
		const auto mask = 1ull << (i % 64);
		if (exp[i / 64] & mask) {
			// result = (result * temp) % mod;
			mult(temp2, result, temp, karatsuba_temps);
			_private::divmod<true, false>(temp3, result, temp2, mod, temp_mod, temp_af, temp_bf); // temp3 is just a placeholder here and is never read from or written to.
		}
		if (i + 1 < exp_bits) { // don´t square at the end of the last loop, it just wasts CPU cycles.
			// temp = (temp * temp) % mod;
			mult(temp3, temp, temp, karatsuba_temps);
			_private::divmod<true, false>(temp2, temp, temp3, mod, temp_mod, temp_af, temp_bf); // temp2 is just a placeholder here and is never read from or written to.
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
lehmer(BigInt& U, BigInt& V, BigInt& R, BigInt& temp) {
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
	mult(R, U, x_i);
	mult(temp, V, y_i);
	R += temp;

	U *= x_im1;
	mult(temp, V, y_im1);
	U += temp;

	std::swap(V, R);
}

BIGINT_TRACY_CONSTEXPR_AUTO
gcd_internal(const BigInt& Uu, const BigInt& Vv) -> BigInt {
	BIGINT_TRACY_ZONE_SCOPED;
	// we require abs(Uu) >= abs(Vv)

	constexpr auto k = 63;

	BigInt U = Uu; U.sign() = Sign::POS;
	BigInt V = Vv; V.sign() = Sign::POS;
	BigInt R, temp; // temporaries

	while (!is_zero(V)) {
		if (log2(U) - log2(V) <= k/2) {
			_private::lehmer<k>(U, V, R, temp);
		}

		mod(temp, U, V); // temp = U % V;
		std::swap(U, V);
		std::swap(V, temp);
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


namespace _private {
/**
 * @brief Sums all digits in the given base ignoring any sign. E.g.: `digit_sum<10>(-12955) == 1 + 2 + 9 + 5 + 5 == 22`.
 * Supported bases are 2 - 64 (inclusive). The bases 2, 4, 8, 16, and 32 are considerable faster than any other base.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
digit_sum(const utils::Span<const uint64_t> v, uint_fast8_t base = 10) -> uint64_t {
	BIGINT_TRACY_ZONE_SCOPED;
	if (base > 64 || base < 2) {
		std::string msg = "digit_sum only supports bases in the range 2 - 64 (inclusive).";
		throw std::invalid_argument(utils::error_msg(std::move(msg)));
	}
	const auto conv = _private::base_conversion_64(base);

	if (conv.division_base != 0) {
		BigInt q{v, Sign::POS};

		uint64_t sum = 0ull;
		while (!is_zero(q)) {
			auto digits = divmod(q, q, conv.division_base);
			while (digits > 0) {
				sum += digits % base;
				digits /= base;
			}
			q.cleanup();
		}
		return sum;

	} else { // special case for when base is a divider of 32.
		const auto base_power = _private::base_conversion_64{base}.base_power;
		uint64_t sum = 0ull;
		for (BigInt::size_type i = 0; i < v.size(); ++i) {
			uint64_t digs = v[i];
			for (BigInt::size_type j = 0; j < base_power; ++j) {
				sum += digs % base;
				digs /= base;
			}
		}
		return sum;
	}
}
}

/**
 * @brief Sums all digits in the given base ignoring any sign. E.g.: `digit_sum<10>(-12955) == 1 + 2 + 9 + 5 + 5 == 22`.
 * Supported bases are 2 - 64 (inclusive). The bases 2, 4, 8, 16, and 32 are considerable faster than any other base.
 */
BIGINT_TRACY_CONSTEXPR_AUTO
digit_sum(const is_BigInt_like auto& v, uint_fast8_t base = 10) -> uint64_t {
	return _private::digit_sum(v._span(), base);
}

}


#include "_bigint_tracy_undefines.h"
