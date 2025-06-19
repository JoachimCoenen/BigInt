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

TEST_UNARY_OPERATOR_BIGINT(Sqrt, BigInt, sqrt(a), get_all_sqrt_test_values())

TEST_UNARY_OPERATOR(Log2, BigInt, uint64_t, log2(a), get_all_log2_test_values(), uint64_t, res)

TEST_UNARY_OPERATOR(Log10, BigInt, uint64_t, log10(a), get_all_log10_test_values(), uint64_t, res)

TEST_BINARY_OPERATOR(Log, BigInt, BigInt, uint64_t, log(a, b), get_all_log_test_values(), uint64_t, res)

TEST_BINARY_OPERATOR_BIGINT(Pow, BigInt, uint64_t, pow(a, b), get_all_pow_test_values())

TEST_TRINARY_OPERATOR_BIGINT(PowMod, BigInt, BigInt, BigInt, pow_mod(a, b, c), get_all_powmod_test_values())

TEST_UNARY_OPERATOR(DigitSum_10, BigInt, uint64_t, digit_sum<10>(a), get_all_digit_sum_10_test_values(), uint64_t, res)

TEST_UNARY_OPERATOR(DigitSum_16, BigInt, uint64_t, digit_sum<16>(a), get_all_digit_sum_16_test_values(), uint64_t, res)

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

