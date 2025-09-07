#ifdef BIGINT_TRACY_ENABLE
#	include <tracy/Tracy.hpp>
#endif

#include "../src/bigint/bigInt.h"
#include "../src/bigint/math.h"

#include "values_for_test.h"
#include "macros_for_tests.h"

namespace {
using namespace bigint;
using namespace test_data;
}


// sqrt, log2, pow, pow_mod, etc.
namespace {

auto double_to_str(double x) -> std::string {
	// allows or tiny variations in the last two decimal digits.
	auto x_str = std::to_string(x);
	auto dot_pos = x_str.find('.');
	if (dot_pos == std::string::npos) {
		return x_str;
	}
	if (dot_pos >= x_str.size() - 3) {
		return x_str;
	}
	return x_str.substr(0, x_str.size() - 2);
}

TEST_UNARY_OPERATOR_BIGINT(Sqrt, BigInt, sqrt(a), get_all_sqrt_test_values())

TEST_UNARY_OPERATOR(Log2, BigInt, uint64_t, log2(a), get_all_log2_test_values(), uint64_t, res)

TEST_UNARY_OPERATOR(Log2d, BigInt, double, log2d(a), get_all_log2d_test_values(), std::string, double_to_str(res))

TEST_UNARY_OPERATOR(Log10, BigInt, uint64_t, log10(a), get_all_log10_test_values(), uint64_t, res)

TEST_BINARY_OPERATOR(Log, BigInt, BigInt, uint64_t, log(a, b), get_all_log_test_values(), uint64_t, res)

TEST_BINARY_OPERATOR_BIGINT(Pow, BigInt, uint64_t, pow(a, b), get_all_pow_test_values())

TEST_TRINARY_OPERATOR_BIGINT(PowMod, BigInt, BigInt, BigInt, pow_mod(a, b, c), get_all_powmod_test_values())

TEST_UNARY_OPERATOR(DigitSum,   BigInt, uint64_t, digit_sum(a), get_all_digit_sum_10_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_2, BigInt, uint64_t, digit_sum(a, 2), get_all_digit_sum_2_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_3, BigInt, uint64_t, digit_sum(a, 3), get_all_digit_sum_3_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_5, BigInt, uint64_t, digit_sum(a, 5), get_all_digit_sum_5_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_10, BigInt, uint64_t, digit_sum(a, 10), get_all_digit_sum_10_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_13, BigInt, uint64_t, digit_sum(a, 13), get_all_digit_sum_13_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_16, BigInt, uint64_t, digit_sum(a, 16), get_all_digit_sum_16_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_32, BigInt, uint64_t, digit_sum(a, 32), get_all_digit_sum_32_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_35, BigInt, uint64_t, digit_sum(a, 35), get_all_digit_sum_35_test_values(), uint64_t, res)
TEST_UNARY_OPERATOR(DigitSum_36, BigInt, uint64_t, digit_sum(a, 36), get_all_digit_sum_36_test_values(), uint64_t, res)

}


// combinatorics
namespace {

TEST_BINARY_OPERATOR_BIGINT(Perm, uint32_t, uint32_t, perm(a, b), get_all_perm_test_values())

TEST_BINARY_OPERATOR_BIGINT(Comb, uint32_t, uint32_t, comb(a, b), get_all_comb_test_values())

}


// number theory
namespace {

TEST_BINARY_OPERATOR_BIGINT(Gcd, BigInt, BigInt, gcd(a, b), get_all_gcd_test_values())

TEST_BINARY_OPERATOR_BIGINT(Lcm, BigInt, BigInt, lcm(a, b), get_all_lcm_test_values())

}

