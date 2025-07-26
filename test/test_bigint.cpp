#ifdef BIGINT_TRACY_ENABLE
#	include <tracy/Tracy.hpp>
#endif

#include "../src/bigint/bigInt.h"

#include "values_for_test.h"
#include "macros_for_tests.h"

namespace {
using namespace bigint;
using namespace test_data;
}

// Constructors
namespace {

inline void expectSimpleInt(const DigitsVec& actual, const DigitsVec& expectedData) {
	EXPECT_EQ(actual, expectedData);
}

TEST(HelloTest, TestCreateEmpty) {
	expectSimpleInt(BigInt{}.__data_for_testing_only(), {0, 0});
}

TEST(HelloTest, TestCreateFromUInt64) {
	const std::vector<uint64_t> numbers = {{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		123456,
		std::numeric_limits<uint32_t>::max() - 1,
		std::numeric_limits<uint32_t>::max(),
		std::numeric_limits<uint64_t>::max() - 1,
		std::numeric_limits<uint64_t>::max(),
	}};
	for (auto i : numbers) {
		expectSimpleInt(BigInt{i}.__data_for_testing_only(), {i, 0});
	}
}

TEST(HelloTest, TestCreateFromUInt32) {
	const std::vector<uint32_t> numbers = {{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
		123456,
		std::numeric_limits<uint32_t>::max() - 1,
		std::numeric_limits<uint32_t>::max(),
	}};
	for (auto i : numbers) {
		expectSimpleInt(BigInt{i}.__data_for_testing_only(), {i, 0});
	}
}

TEST(HelloTest, TestCreateFromString) {
	const std::vector<std::string> numbers = {{
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
		"123456",
		UINT32_MAX_M2,
		UINT32_MAX_M1,
		UINT32_MAX_00,
		UINT32_MAX_P1,
		UINT64_MAX_M2,
		UINT64_MAX_M1,
//		UINT64_MAX_P1,
	}};
	for (const auto &i : numbers) {
		expectSimpleInt(BigInt{i}.__data_for_testing_only(), {utils::stoull(i), 0});
	}

	const std::vector<std::string> numbers2 = {{
		UINT32_MAX_M2,
		UINT32_MAX_M1,
		UINT32_MAX_00,
		UINT32_MAX_P1,
		UINT64_MAX_M2,
		UINT64_MAX_M1,
		UINT64_MAX_P1,
		UINT64_MAX_P2,
		"61835470006151583405",
		"18446744082299486209",
		"18446744090889420804",
		"340282366920938463463374607431768211456",
		"340282366920938463500268095579187314689", // {1, 2, 1}
		"340282366920938463518714839652896866306",
		"618354700061515834059999999799999999991",
		"382362535088167210234626361716426516060166448586892714986920000001800000000081",
		"232931339070793815325001255056475653097414553258567232362355333800726680988985211211511618544321786091997319753227407650675307289491579353812743868042530535645583501676791893975824413269763335943643852169932557667460526773009345243875370397849117323089797180562438607414082502653894345431248479157325597883382872512363227313754806954943153356578030776827687214915975118411696925363193046840152997695054949759239436030221189399569495065261571909317195249060117631093190757937905478380376220775299132932095692407152418600171521692410082916811004893488551669708031326717521609627874636918587185311894953297033903530024987034571168265799599738597170087562202071727690098194094667637598749519054320005270025574623421370998076822409787859412171793316629735570507939613093936361860660116895996666614701661706651554237530968709455913093851747168282813733538360222849832599391587758749240972379619381546053404797147940009298302796846728059527866721345117244337673312761146439865070539453356727697657",
	}};
	for (const auto &i : numbers2) {
		EXPECT_EQ(to_string(BigInt{i}), (i));
		EXPECT_EQ(to_string(BigInt{"+" + i}), (i));
		EXPECT_EQ(to_string(BigInt{"-" + i}), "-" + i);
	}

	expectSimpleInt(BigInt{"61835470006151583405"}.__data_for_testing_only(), {6495237785022928557, 3, 0});
	EXPECT_EQ(to_string(BigInt{"61835470006151583405"}), "61835470006151583405");

	expectSimpleInt(BigInt{"340282366920938463500268095579187314689"}.__data_for_testing_only(), {1, 2, 1, 0});
	EXPECT_EQ(to_string(BigInt{"340282366920938463500268095579187314689"}), "340282366920938463500268095579187314689");
}

}


// Bitwise Shift
namespace {

TEST_BINARY_OPERATOR_BIGINT_F(LShift, BigInt, uint64_t, lshift(res, a, b), get_all_lshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(LShift, uint64_t, lshift(a, a, b), get_all_lshift_test_values())

TEST_BINARY_OPERATOR_BIGINT(LShift, BigInt, uint64_t, a << b, get_all_lshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(LShift, uint64_t, a <<= b, get_all_lshift_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(RShift, BigInt, uint64_t, rshift(res, a, b), get_all_rshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(RShift, uint64_t, rshift(a, a, b), get_all_rshift_test_values())

TEST_BINARY_OPERATOR_BIGINT(RShift, BigInt, uint64_t, a >> b, get_all_rshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(RShift, uint64_t, a >>= b, get_all_rshift_test_values())

}

// Bitwise AND, OR, XOR
namespace {
TEST_BINARY_OPERATOR_BIGINT_F(BitwiseAnd, BigInt, BigInt, bitwise_and(res, a, b), get_all_bitwise_and_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(BitwiseAnd, BigInt, bitwise_and(a, a, b), get_all_bitwise_and_test_values())

TEST_BINARY_OPERATOR_BIGINT(BitwiseAnd, BigInt, BigInt, a & b, get_all_bitwise_and_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(BitwiseAnd, BigInt, a &= b, get_all_bitwise_and_test_values())


TEST_BINARY_OPERATOR_BIGINT_F(BitwiseOr, BigInt, BigInt, bitwise_or(res, a, b), get_all_bitwise_or_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(BitwiseOr, BigInt, bitwise_or(a, a, b), get_all_bitwise_or_test_values())

TEST_BINARY_OPERATOR_BIGINT(BitwiseOr, BigInt, BigInt, a | b, get_all_bitwise_or_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(BitwiseOr, BigInt, a |= b, get_all_bitwise_or_test_values())


TEST_BINARY_OPERATOR_BIGINT_F(BitwiseXOr, BigInt, BigInt, bitwise_xor(res, a, b), get_all_bitwise_xor_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(BitwiseXOr, BigInt, bitwise_xor(a, a, b), get_all_bitwise_xor_test_values())

TEST_BINARY_OPERATOR_BIGINT(BitwiseXor, BigInt, BigInt, a ^ b, get_all_bitwise_xor_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(BitwiseXor, BigInt, a ^= b, get_all_bitwise_xor_test_values())

}



// Addition
namespace {

TEST_BINARY_OPERATOR_BIGINT_F(Add, BigInt, BigInt, add(res, a, b), get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Add, BigInt, uint64_t, add(res, a, b), get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Add, BigInt, int64_t, add(res, a, b), get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Add, BigInt, uint32_t, add(res, a, b), get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Add, BigInt, int32_t, add(res, a, b), get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Add, BigInt, add(a, a, b), get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Add, uint64_t, add(a, a, b), get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Add, int64_t, add(a, a, b), get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Add, uint32_t, add(a, a, b), get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Add, int32_t, add(a, a, b), get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, BigInt, BigInt, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, BigInt, uint64_t, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, BigInt, int64_t, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, BigInt, uint32_t, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, BigInt, int32_t, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, uint64_t, BigInt, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, int64_t, BigInt, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, uint32_t, BigInt, a + b, get_all_add_test_values())

TEST_BINARY_OPERATOR_BIGINT(Add, int32_t, BigInt, a + b, get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Add, BigInt, a += b, get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Add, uint64_t, a += b, get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Add, int64_t, a += b, get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Add, uint32_t, a += b, get_all_add_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Add, int32_t, a += b, get_all_add_test_values())

}


// Subtraction
namespace {

TEST_BINARY_OPERATOR_BIGINT_F(Sub, BigInt, BigInt, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, BigInt, uint64_t, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, BigInt, int64_t, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, BigInt, uint32_t, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, BigInt, int32_t, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, uint64_t, BigInt, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, int64_t, BigInt, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, uint32_t, BigInt, sub(res, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Sub, int32_t, BigInt, sub(res, a, b), get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Sub, BigInt, sub(a, a, b), get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Sub, uint64_t, sub(a, a, b), get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Sub, int64_t, sub(a, a, b), get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Sub, uint32_t, sub(a, a, b), get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Sub, int32_t, sub(a, a, b), get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, BigInt, BigInt, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, BigInt, uint64_t, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, BigInt, int64_t, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, BigInt, uint32_t, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, BigInt, int32_t, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, uint64_t, BigInt, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, int64_t, BigInt, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, uint32_t, BigInt, a - b, get_all_sub_test_values())

TEST_BINARY_OPERATOR_BIGINT(Sub, int32_t, BigInt, a - b, get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Sub, BigInt, a -= b, get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Sub, uint64_t, a -= b, get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Sub, int64_t, a -= b, get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Sub, uint32_t, a -= b, get_all_sub_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Sub, int32_t, a -= b, get_all_sub_test_values())

}


// Multiplication
namespace {

TEST_BINARY_OPERATOR_BIGINT_F(Mul, BigInt, uint64_t, mult(res, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Mul, BigInt, int64_t, mult(res, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Mul, BigInt, uint32_t, mult(res, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Mul, BigInt, int32_t, mult(res, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(MulNaive, BigInt, BigInt, DigitsVec tmp; mult_naive(res, a, b, tmp), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(MulKaratsuba_f1, BigInt, BigInt, KaratsubaStepTemps tmp; mult_karatsuba(res, a, b, tmp), get_all_mul_test_values())
TEST_BINARY_OPERATOR_BIGINT_F(MulKaratsuba_f2, BigInt, BigInt, KaratsubaStepTemps tmp; mult_karatsuba(res, a, b, tmp), get_all_mul_karatsuba_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Mul_f1, BigInt, BigInt, mult(res, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT_F(Mul_f2, BigInt, BigInt, DigitsVec tmp; utils::UniquePtr<KaratsubaStepTemps> tmp2; mult(res, a, b, tmp, tmp2), get_all_mul_test_values())
TEST_BINARY_OPERATOR_BIGINT_F(Mul_f3, BigInt, BigInt, DigitsVec tmp; utils::UniquePtr<KaratsubaStepTemps> tmp2; mult(res, a, b, tmp, tmp2), get_all_mul_karatsuba_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Mul, uint64_t, mult(a, a, b), get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Mul, int64_t, mult(a, a, b), get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Mul, uint32_t, mult(a, a, b), get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT_F(Mul, int32_t, mult(a, a, b), get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, BigInt, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(MulKaratsuba, BigInt, BigInt, a * b, get_all_mul_karatsuba_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, uint64_t, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, int64_t, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, uint32_t, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, int32_t, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, uint64_t, BigInt, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, int64_t, BigInt, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, uint32_t, BigInt, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, int32_t, BigInt, a * b, get_all_mul_test_values())

TEST_BINARY_OPERATOR_BIGINT(Mul, uint64_t, uint64_t, BigInt{mult(a, b)}, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, BigInt, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(MulKaratsuba, BigInt, a *= b, get_all_mul_karatsuba_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, uint64_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, int64_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, uint32_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, int32_t, a *= b, get_all_mul_test_values())

}


#define _TEST_DIV_BY_ZERO(NAME, O1, O2, OP) \
TEST(HelloTest, Test##NAME##ByZero_##O1##_##O2) {\
	O1 a = str_to_int<O1>("15");\
	const O2 b = str_to_int<O2>("0");\
	EXPECT_THROW([[maybe_unused]] auto x = [&](){OP;}(), std::domain_error);\
}\
TEST(HelloTest, Test##NAME##ZeroByZero_##O1##_##O2) {\
	O1 a = str_to_int<O1>("0");\
	const O2 b = str_to_int<O2>("0");\
	EXPECT_THROW([[maybe_unused]] auto x = [&](){OP;}(), std::domain_error);\
}

#define TEST_DIV_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
TEST_BINARY_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT)\
_TEST_DIV_BY_ZERO(NAME, O1, O2, return OP)

#define TEST_DIV_OPERATOR_F(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
TEST_BINARY_OPERATOR_F(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
_TEST_DIV_BY_ZERO(F##NAME, O1, O2, R res; OP; return res)

#define TEST_DIV_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES)\
_TEST_DIV_BY_ZERO(NAME, O1, O2, return OP)

#define TEST_DIV_OPERATOR_BIGINT_F(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR_BIGINT_F(NAME, O1, O2, OP, TEST_VALUES)\
_TEST_DIV_BY_ZERO(F##NAME, O1, O2, BigInt res; OP; return res)

#define TEST_DIV_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES) \
TEST_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES)\
_TEST_DIV_BY_ZERO(I##NAME, BigInt, O2, return OP)

#define TEST_DIV_ASSIGN_OPERATOR_BIGINT_F(NAME, O2, OP, TEST_VALUES) \
TEST_ASSIGN_OPERATOR_BIGINT_F(NAME, O2, OP, TEST_VALUES)\
_TEST_DIV_BY_ZERO(IF##NAME, BigInt, O2, OP; return a)

// Division
namespace {

TEST_DIV_OPERATOR_BIGINT_F(Div, BigInt, BigInt, div(res, a, b), get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT_F(Div, BigInt, uint64_t, div(res, a, b), get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT_F(Div, BigInt, int64_t, div(res, a, b), get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT_F(Div, BigInt, uint32_t, div(res, a, b), get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT_F(Div, BigInt, int32_t, div(res, a, b), get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT_F(Div, uint64_t, div(a, a, b), get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT_F(Div, int64_t, div(a, a, b), get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT_F(Div, uint32_t, div(a, a, b), get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT_F(Div, int32_t, div(a, a, b), get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT(Div, BigInt, BigInt, a / b, get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT(Div, BigInt, uint64_t, a / b, get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT(Div, BigInt, int64_t, a / b, get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT(Div, BigInt, uint32_t, a / b, get_all_div_test_values())

TEST_DIV_OPERATOR_BIGINT(Div, BigInt, int32_t, a / b, get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Div, BigInt, a /= b, get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Div, uint64_t, a /= b, get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Div, int64_t, a /= b, get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Div, uint32_t, a /= b, get_all_div_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Div, int32_t, a /= b, get_all_div_test_values())

}


// Modulo
namespace {

TEST_DIV_OPERATOR_BIGINT_F(Mod, BigInt, BigInt, mod(res, a, b), get_all_mod_test_values())

TEST_DIV_OPERATOR_F(Mod, BigInt, uint64_t, uint64_t, mod(res, a, b), get_all_mod_test_values(), uint64_t, res)

TEST_DIV_OPERATOR_F(Mod, BigInt, int64_t, int64_t, mod(res, a, b), get_all_mod_test_values(), int64_t, res)

TEST_DIV_OPERATOR_F(Mod, BigInt, uint32_t, uint32_t, mod(res, a, b), get_all_mod_test_values(), uint32_t, res)

TEST_DIV_OPERATOR_F(Mod, BigInt, int32_t, int32_t, mod(res, a, b), get_all_mod_test_values(), int32_t, res)

TEST_DIV_OPERATOR_BIGINT(Mod, BigInt, BigInt, a % b, get_all_mod_test_values())

TEST_DIV_OPERATOR(Mod, BigInt, uint64_t, uint64_t, a % b, get_all_mod_test_values(), uint64_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, int64_t, int64_t, a % b, get_all_mod_test_values(), int64_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, uint32_t, uint32_t, a % b, get_all_mod_test_values(), uint32_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, int32_t, int32_t, a % b, get_all_mod_test_values(), int32_t, res)

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, BigInt, a %= b, get_all_mod_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, uint64_t, a %= b, get_all_mod_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, int64_t, a %= b, get_all_mod_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, uint32_t, a %= b, get_all_mod_test_values())

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, int32_t, a %= b, get_all_mod_test_values())

}


// divmod
namespace {

template<typename RT>
using Divmod_RT = std::tuple<DigitsVec, RT>;

template<typename R>
using Divmod_R = DivModResult<BigInt, R>;

#define TEST_DIVMOD(NAME, O1, O2, OP, RT, GET_RT) \
TEST_DIV_OPERATOR(NAME, O1, O2, Divmod_R<O2>, OP, get_all_divmod_test_values(), Divmod_RT<RT>, std::tuple(res.q.__data_for_testing_only(), GET_RT))

#define TEST_DIVMOD_F(NAME, O1, O2, OP, RT, GET_RT) \
TEST_DIV_OPERATOR_F(NAME, O1, O2, Divmod_R<O2>, OP, get_all_divmod_test_values(), Divmod_RT<RT>, std::tuple(res.q.__data_for_testing_only(), GET_RT))

TEST_DIVMOD_F(Divmod, BigInt, BigInt, DigitsVec temp; DigitsVec temp_af; DigitsVec temp_bf; divmod(res.q, res.r, a, b, temp, temp_af, temp_bf), DigitsVec, res.r.__data_for_testing_only())

TEST_DIVMOD_F(Divmod, BigInt, uint64_t, res.r = divmod(res.q, a, b), uint64_t, res.r)

TEST_DIVMOD_F(Divmod, BigInt, int64_t, res.r = divmod(res.q, a, b), int64_t, res.r)

TEST_DIVMOD_F(Divmod, BigInt, uint32_t, res.r = divmod(res.q, a, b), uint32_t, res.r)

TEST_DIVMOD_F(Divmod, BigInt, int32_t, res.r = divmod(res.q, a, b), int32_t, res.r)

TEST_DIVMOD(Divmod, BigInt, BigInt, divmod(a, b), DigitsVec, res.r.__data_for_testing_only())

TEST_DIVMOD(Divmod, BigInt, uint64_t, divmod(a, b), uint64_t, res.r)

TEST_DIVMOD(Divmod, BigInt, int64_t, divmod(a, b), int64_t, res.r)

TEST_DIVMOD(Divmod, BigInt, uint32_t, divmod(a, b), uint32_t, res.r)

TEST_DIVMOD(Divmod, BigInt, int32_t, divmod(a, b), int32_t, res.r)

}


// to_string
namespace {

TEST_UNARY_OPERATOR(to_string, BigInt, std::string, to_string(a), get_all_to_string_10_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_base2, BigInt, std::string, to_string_base2(a), get_all_to_string_2_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_base8, BigInt, std::string, to_string_base8(a), get_all_to_string_8_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_base10, BigInt, std::string, to_string_base10(a), get_all_to_string_10_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_base16, BigInt, std::string, to_string_base16(a), get_all_to_string_16_test_values(), std::string, res)

TEST_UNARY_OPERATOR(to_string_2, BigInt, std::string, to_string(a, 2), get_all_to_string_2_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_3, BigInt, std::string, to_string(a, 3), get_all_to_string_3_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_5, BigInt, std::string, to_string(a, 5), get_all_to_string_5_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_10, BigInt, std::string, to_string(a, 10), get_all_to_string_10_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_13, BigInt, std::string, to_string(a, 13), get_all_to_string_13_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_16, BigInt, std::string, to_string(a, 16), get_all_to_string_16_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_32, BigInt, std::string, to_string(a, 32), get_all_to_string_32_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_35, BigInt, std::string, to_string(a, 35), get_all_to_string_35_test_values(), std::string, res)
TEST_UNARY_OPERATOR(to_string_36, BigInt, std::string, to_string(a, 36), get_all_to_string_36_test_values(), std::string, res)

}


// from_string
namespace {
using string = std::string;

TEST_UNARY_OPERATOR_BIGINT(from_string, string, from_string(a), get_all_from_string_10_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_base2, string, from_string_base2(a), get_all_from_string_2_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_base8, string, from_string_base8(a), get_all_from_string_8_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_base10, string, from_string_base10(a), get_all_from_string_10_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_base16, string, from_string_base16(a), get_all_from_string_16_test_values())

TEST_UNARY_OPERATOR_BIGINT(from_string_2, string, from_string(a, 2), get_all_from_string_2_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_3, string, from_string(a, 3), get_all_from_string_3_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_5, string, from_string(a, 5), get_all_from_string_5_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_10, string, from_string(a, 10), get_all_from_string_10_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_13, string, from_string(a, 13), get_all_from_string_13_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_16, string, from_string(a, 16), get_all_from_string_16_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_32, string, from_string(a, 32), get_all_from_string_32_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_35, string, from_string(a, 35), get_all_from_string_35_test_values())
TEST_UNARY_OPERATOR_BIGINT(from_string_36, string, from_string(a, 36), get_all_from_string_36_test_values())

}


// user-defined literal
namespace {

TEST(HelloTest, TestUserDefinedLiteralHex) {
	const std::vector<std::pair<std::string, BigInt>> numbers2 = {{
		{"0", 0x0_big},
		{"1", 0x1_big},
		{"2", 0x2_big},
		{"3", 0x3_big},
		{"4", 0x4_big},
		{"5", 0x5_big},
		{"6", 0x6_big},
		{"7", 0x7_big},
		{"8", 0x8_big},
		{"9", 0x9_big},
		{"10", 0xa_big},
		{"11", 0xb_big},
		{"12", 0xc_big},
		{"13", 0xd_big},
		{"14", 0xe_big},
		{"15", 0xf_big},
		{"16", 0x10_big},
		{"17", 0x11_big},
		{"4294967293", 0xfffffffd_big},
		{"4294967294", 0xfffffffe_big},
		{"4294967295", 0xffffffff_big},
		{"4294967296", 0x100000000_big},
		{"18446744073709551613", 0xfffffffffffffffd_big},
		{"18446744073709551614", 0xfffffffffffffffe_big},
		{"18446744073709551616", 0x10000000000000000_big},
		{"18446744073709551617", 0x10000000000000001_big},
		{"61835470006151583405", 0x3'5a23'b85a'2bfc'96ad_big},
		{"18446744082299486209", 0x10000000200000001_big},
		{"18446744090889420804", 0x10000000400000004_big},
		{"340282366920938463463374607431768211456", 0x100000000000000000000000000000000_big},
		{"340282366920938463500268095579187314689", 0x100000000000000020000000000000001_big},
		{"340282366920938463518714839652896866306", 0x100000000000000030000000000000002_big},
		{"618354700061515834059999999799999999991", 0x1d132c88702d85e0679102e151ac22ff7_big},
		{"382362535088167210234626361716426516060166448586892714986920000001800000000081", 0x34d59868d6a265dbe51797263285b864eea849ad8a4c2dc9b29b16c7205fc5051_big},
		{"232931339070793815325001255056475653097414553258567232362355333800726680988985211211511618544321786091997319753227407650675307289491579353812743868042530535645583501676791893975824413269763335943643852169932557667460526773009345243875370397849117323089797180562438607414082502653894345431248479157325597883382872512363227313754806954943153356578030776827687214915975118411696925363193046840152997695054949759239436030221189399569495065261571909317195249060117631093190757937905478380376220775299132932095692407152418600171521692410082916811004893488551669708031326717521609627874636918587185311894953297033903530024987034571168265799599738597170087562202071727690098194094667637598749519054320005270025574623421370998076822409787859412171793316629735570507939613093936361860660116895996666614701661706651554237530968709455913093851747168282813733538360222849832599391587758749240972379619381546053404797147940009298302796846728059527866721345117244337673312761146439865070539453356727697657", 0x6176aa074c9a272bb3ea750c4c772ac86dcf5470f8dbc57f66df3351c93f2879cf40bb1f3a3180f749e816863af60ca218923ad40254bd57e39dff05c66358228cea7de2f4a457b1dbbf21ac2312bbc0d7e9ffeb84cdead003565687a88b55ea990dd6f21a7f24f73f629505e5da46540ae48449804a5f95d2f5f0fa83cb9e5e43bb77d30605884f89dae3d330df6a7519bf80788766a92c214e9180fbdf83bc96430dd5bd2d11e8329c6f05bcdfc6e05492185823d3ab1c1964f065ad9f17dd03b569b0bbc0d39020d95d830336d8ad79e54f93cdfb4fcef77874a92403efae6d1af9363d383ac6e1e02de475fbb52003c02516cfac87a53139a3a6ac9b8950092d1899a2b62727343b5d66ad83651291e113a46158a2c73ae4d243dc45ed57733dfec305bbc26fd8fabeb583733cbc22b680e186a0d523cb62f8dcf42d98272a8c3bd8747246bdf1ee5dceaf547920ec22b7f6b4874f8c9aef3785630a96e7ab588fcb045c7536051c21a8e27bd38c406949770c447d7b943f3c6e9c5a3ba8df7be334d17f7243ab9cee6f5907b2128b8b2c2760dc00eb9d80f9_big},
	}};
	for (const auto&[str, bigint] : numbers2) {
		EXPECT_EQ(to_string(bigint), str);
	}
}

TEST(HelloTest, TestUserDefinedLiteralDec) {
	const std::vector<std::pair<std::string, BigInt>> numbers2 = {{
		{"0", 0_big},
		{"1", 1_big},
		{"2", 2_big},
		{"3", 3_big},
		{"4", 4_big},
		{"5", 5_big},
		{"6", 6_big},
		{"7", 7_big},
		{"8", 8_big},
		{"9", 9_big},
		{"10", 10_big},
		{"11", 11_big},
		{"12", 12_big},
		{"13", 13_big},
		{"14", 14_big},
		{"15", 15_big},
		{"16", 16_big},
		{"17", 17_big},
		{"4294967293", 4294967293_big},
		{"4294967294", 4294967294_big},
		{"4294967295", 4294967295_big},
		{"4294967296", 4294967296_big},
		{"18446744073709551613", 18446744073709551613_big},
		{"18446744073709551614", 18446744073709551614_big},
		{"18446744073709551616", 18446744073709551616_big},
		{"18446744073709551617", 18446744073709551617_big},
		{"61835470006151583405", 61835470006151583405_big},
		{"18446744082299486209", 18446744082299486209_big},
		{"18446744090889420804", 18446744090889420804_big},
		{"340282366920938463463374607431768211456", 340282366920938463463374607431768211456_big},
		{"340282366920938463500268095579187314689", 340282366920938463500268095579187314689_big},
		{"340282366920938463518714839652896866306", 340282366920938463518714839652896866306_big},
		{"618354700061515834059999999799999999991", 618354700061515834059999999799999999991_big},
		{"382362535088167210234626361716426516060166448586892714986920000001800000000081", 382362535088167210234626361716426516060166448586892714986920000001800000000081_big},
		{"232931339070793815325001255056475653097414553258567232362355333800726680988985211211511618544321786091997319753227407650675307289491579353812743868042530535645583501676791893975824413269763335943643852169932557667460526773009345243875370397849117323089797180562438607414082502653894345431248479157325597883382872512363227313754806954943153356578030776827687214915975118411696925363193046840152997695054949759239436030221189399569495065261571909317195249060117631093190757937905478380376220775299132932095692407152418600171521692410082916811004893488551669708031326717521609627874636918587185311894953297033903530024987034571168265799599738597170087562202071727690098194094667637598749519054320005270025574623421370998076822409787859412171793316629735570507939613093936361860660116895996666614701661706651554237530968709455913093851747168282813733538360222849832599391587758749240972379619381546053404797147940009298302796846728059527866721345117244337673312761146439865070539453356727697657", 232931339070793815325001255056475653097414553258567232362355333800726680988985211211511618544321786091997319753227407650675307289491579353812743868042530535645583501676791893975824413269763335943643852169932557667460526773009345243875370397849117323089797180562438607414082502653894345431248479157325597883382872512363227313754806954943153356578030776827687214915975118411696925363193046840152997695054949759239436030221189399569495065261571909317195249060117631093190757937905478380376220775299132932095692407152418600171521692410082916811004893488551669708031326717521609627874636918587185311894953297033903530024987034571168265799599738597170087562202071727690098194094667637598749519054320005270025574623421370998076822409787859412171793316629735570507939613093936361860660116895996666614701661706651554237530968709455913093851747168282813733538360222849832599391587758749240972379619381546053404797147940009298302796846728059527866721345117244337673312761146439865070539453356727697657_big},
	}};
	for (const auto&[str, bigint] : numbers2) {
		EXPECT_EQ(to_string(bigint), str);
	}
}

TEST(HelloTest, TestUserDefinedLiteralOct) {
	const std::vector<std::pair<std::string, BigInt>> numbers2 = {{
		{"0", 00_big},
		{"1", 01_big},
		{"2", 02_big},
		{"3", 03_big},
		{"4", 04_big},
		{"5", 05_big},
		{"6", 06_big},
		{"7", 07_big},
		{"8", 010_big},
		{"9", 011_big},
		{"10", 012_big},
		{"11", 013_big},
		{"12", 014_big},
		{"13", 015_big},
		{"14", 016_big},
		{"15", 017_big},
		{"16", 020_big},
		{"17", 021_big},
		{"4294967293", 037777777775_big},
		{"4294967294", 037777777776_big},
		{"4294967295", 037777777777_big},
		{"4294967296", 040000000000_big},
		{"18446744073709551613", 01777777777777777777775_big},
		{"18446744073709551614", 01777777777777777777776_big},
		{"18446744073709551616", 02000000000000000000000_big},
		{"18446744073709551617", 02000000000000000000001_big},
		{"61835470006151583405", 06'550'435'605'505'377'113'255_big},
		{"18446744082299486209", 02000000000100000000001_big},
		{"18446744090889420804", 02000000000200000000004_big},
		{"340282366920938463463374607431768211456", 04000000000000000000000000000000000000000000_big},
		{"340282366920938463500268095579187314689", 04000000000000000000004000000000000000000001_big},
		{"340282366920938463518714839652896866306", 04000000000000000000006000000000000000000002_big},
		{"618354700061515834059999999799999999991", 07211454420700554136014744201341243260427767_big},
		{"382362535088167210234626361716426516060166448586892714986920000001800000000081", 064653141506552114566762427456230624133414473524111533051141334466246613307100577050121_big},
		{"232931339070793815325001255056475653097414553258567232362355333800726680988985211211511618544321786091997319753227407650675307289491579353812743868042530535645583501676791893975824413269763335943643852169932557667460526773009345243875370397849117323089797180562438607414082502653894345431248479157325597883382872512363227313754806954943153356578030776827687214915975118411696925363193046840152997695054949759239436030221189399569495065261571909317195249060117631093190757937905478380376220775299132932095692407152418600171521692410082916811004893488551669708031326717521609627874636918587185311894953297033903530024987034571168265799599738597170087562202071727690098194094667637598749519054320005270025574623421370998076822409787859412171793316629735570507939613093936361860660116895996666614701661706651554237530968709455913093851747168282813733538360222849832599391587758749240972379619381546053404797147940009298302796846728059527866721345117244337673312761146439865070539453356727697657", 03027325201646232116256637247241423073452620667172507037066742577315574632434447712074717201354371643060075644750055030727540624206111072650011245725374347377405614615301050635237361364510536616673744153021422567403277237775341146752640015262550365042652752462067267441517711173477305124057135510625005344410446002245762564572760765017134745710356673723014026102370473270751460676651650633760036103546522260412351060076757603571131030335267513210750145161570133633761560124444141301075165307014544740626554761373500732551541357006471004066256603006333305327474523711715766477167567416452222003737271550657446617234072615607401336216576732440007400450554765441722461163216465311561124004455061146425542344715035535315266033121122170211644302542426163534464441734213665273463677660602673604677307653726540671474570212664016060650152443626613706717205546023452430357303507110657370756273472572507444073021267755322072370623273633605306052267172553043745404270724660121604152161173647061003224456703042175367120771706723426435650676757431515057734441653471671572620366204505613130235406700035347300371_big},
	}};
	for (const auto&[str, bigint] : numbers2) {
		EXPECT_EQ(to_string(bigint), str);
	}
}

TEST(HelloTest, TestUserDefinedLiteralBin) {
	const std::vector<std::pair<std::string, BigInt>> numbers2 = {{
		{"0", 0b0_big},
		{"1", 0b1_big},
		{"2", 0b10_big},
		{"3", 0b11_big},
		{"4", 0b100_big},
		{"5", 0b101_big},
		{"6", 0b110_big},
		{"7", 0b111_big},
		{"8", 0b1000_big},
		{"9", 0b1001_big},
		{"10", 0b1010_big},
		{"11", 0b1011_big},
		{"12", 0b1100_big},
		{"13", 0b1101_big},
		{"14", 0b1110_big},
		{"15", 0b1111_big},
		{"16", 0b10000_big},
		{"17", 0b10001_big},
		{"4294967293", 0b11111111111111111111111111111101_big},
		{"4294967294", 0b11111111111111111111111111111110_big},
		{"4294967295", 0b11111111111111111111111111111111_big},
		{"4294967296", 0b100000000000000000000000000000000_big},
		{"18446744073709551613", 0b1111111111111111111111111111111111111111111111111111111111111101_big},
		{"18446744073709551614", 0b1111111111111111111111111111111111111111111111111111111111111110_big},
		{"18446744073709551616", 0b10000000000000000000000000000000000000000000000000000000000000000_big},
		{"18446744073709551617", 0b10000000000000000000000000000000000000000000000000000000000000001_big},
		{"61835470006151583405", 0b11'01011010'00100011'10111000'01011010'00101011'11111100'10010110'10101101_big},
		{"18446744082299486209", 0b10000000000000000000000000000001000000000000000000000000000000001_big},
		{"18446744090889420804", 0b10000000000000000000000000000010000000000000000000000000000000100_big},
		{"340282366920938463463374607431768211456", 0b100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000_big},
		{"340282366920938463500268095579187314689", 0b100000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000001_big},
		{"340282366920938463518714839652896866306", 0b100000000000000000000000000000000000000000000000000000000000000110000000000000000000000000000000000000000000000000000000000000010_big},
		{"618354700061515834059999999799999999991", 0b111010001001100101100100010000111000000101101100001011110000001100111100100010000001011100001010100011010110000100010111111110111_big},
		{"382362535088167210234626361716426516060166448586892714986920000001800000000081", 0b110100110101011001100001101000110101101010001001100101110110111110010100010111100101110010011000110010100001011011100001100100111011101010100001001001101011011000101001001100001011011100100110110010100110110001011011000111001000000101111111000101000001010001_big},
	}};
	for (const auto&[str, bigint] : numbers2) {
		EXPECT_EQ(to_string(bigint), str);
	}
}

}
