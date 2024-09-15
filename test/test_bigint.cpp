#ifdef BIGINT_TRACY_ENABLE
#	include <tracy/Tracy.hpp>
#endif

#include "../src/bigint/bigInt.h"
#include "../src/bigint/utils.h"

#include "values_for_test.h"

#include <gtest/gtest.h>

namespace {
using namespace bigint;
using namespace test_data;
}

namespace test_utils {

template <size_t N, typename R, typename RT, typename... On>
inline void test_operation(
	const Operation<R, On...> operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<OperationTest<N>>& tests
) {
	const std::function<void(const R&, const R&, const std::string&, const On&...)>
	test_func = [&](const R& result, const R& expected, const std::string& test_str, [[maybe_unused]] const On&... on) -> void {
		// const auto& o1 = utils::get<0>(on...);
		// EXPECT_NE(&result, &o1) << "return value & first operand expected to be different reference." << test_str;
		EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
	};

	testOperationBase<N, R, On...>(operation, tests, test_func);
}

template <size_t N, typename R, typename RT, typename... On>
	requires std::same_as<R, utils::first_type_of<On...>>
			 && (!std::is_const_v<R>)
			 && (!std::is_const_v<utils::first_type_of<On...>>)
inline void test_assignment_operation(
	const Operation<R&, On...> operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<OperationTest<N>>& tests
) {
	const std::function<void(const R&, const R&, const std::string&, const On&...)>
		test_func = [&](const R& result, const R& expected, const std::string& test_str, [[maybe_unused]] const On&... on) -> void {
		const auto& o1 = utils::get<0>(on...);
		EXPECT_EQ((&result), (&o1)) << "return value & first operand expected to be same reference." << test_str;
		EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
	};

	testOperationBase<N, R&, On...>(operation, tests, test_func);
}

}

/* ***************************************************************************************
 * all tests needed:
 * NON-MATHS PUBLIC:
 *   NORMAL CONSTRUCTORS (explicit & implicit):
 *   COPY/MOVE CONSTRUCTORS / ASSIGNMENT:
 *   ...
 *
 * NON-MATHS PRIVATE:
 *   size() const
 *   uint64_t &operator[](std::size_t) const
 *   uint64_t &operator[](std::size_t)
 *   append(uint64_t v)
 *   insertFront(uint64_t v)
 *   cleanup()
 *   ...
 *
 *
 *
 * MATHS RELATED:
 *   UNARY OPERATORS:
 *     operator-() const
 *     operator+() const
 *      operator++()
 *      operator++(int)
 *      operator--()
 *      operator--(int)
 *
 *   BIARY OPERATORS:
 *     KINDS:
 *       +
 *       -
 *       *
 *       /
 *       %
 *       <<
 *       >>
 *       divmod
 *       power(?)
 *     PERMUTATIONS:
 *       BigInt, BigInt
 *       BigInt, uint64_t
 *       BigInt, int64_t
 *       BigInt, uint32_t
 *       BigInt, int32_t
 *       uint64_t, BigInt
 *       int64_t,  BigInt
 *       uint32_t, BigInt
 *       int32_t,  BigInt
 *
 *   SELF OPERATORS (what is their propper name?):
 *     KINDS:
 *       +=
 *       -=
 *       *=
 *       /=
 *       %=
 *       <<=
 *       >>=
 *     PERMUTATIONS:
 *       BigInt, BigInt
 *       BigInt, uint64_t
 *       BigInt, int64_t
 *       BigInt, uint32_t
 *       BigInt, int32_t
 *
 *   CMPARISONS:
 *     KINDS:
 *       <
 *       >
 *       <=
 *       >=
 *       ==
 *       !=
 *     PERMUTATIONS:
 *       BigInt, BigInt
 *       BigInt, uint64_t
 *       BigInt, int64_t
 *       BigInt, uint32_t
 *       BigInt, int32_t
 *       uint64_t, BigInt
 *       int64_t,  BigInt
 *       uint32_t, BigInt
 *       int32_t,  BigInt
 *
 *   FUNCTIONS:
 *     sqr
 *     sqrt?
 *     factorial
 *     digit_sum
 *     // maybe combinatoricsrelated functions, e.g.: P(), C()?
 *
 * AUXILLARY:
 *   fromString(BigInt)
 *   to_string(BigInt)
 *   to_stringPadded(BigInt)
 *   operator<<(std::ostream&, BigInt)
 *
 *
 *
 *
 */


// Constructors
namespace {

inline void expectSimpleInt(const std::vector<uint64_t>&actual, const std::vector<uint64_t>& expectedData) {
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

#define TEST_UNARY_OPERATOR(NAME, O1, R, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, Test##NAME##_##O1) {\
	test_operation<1, R, RT, O1>(\
		[](const O1& a) -> R { return OP; },\
		[](const R& res) -> RT { return GET_RT; },\
		TEST_VALUES\
	);\
}

#define TEST_UNARY_OPERATOR_BIGINT(NAME, O1, OP, TEST_VALUES) \
TEST_UNARY_OPERATOR(NAME, O1, BigInt, OP, TEST_VALUES, std::vector<uint64_t>, res.__data_for_testing_only())



#define TEST_BINARY_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, Test##NAME##_##O1##_##O2) {\
	test_operation<2, R, RT, O1, O2>(\
		[](const O1& a, const O2& b) -> R { return OP; },\
		[](const R& res) -> RT { return GET_RT; },\
		TEST_VALUES\
	);\
}

#define TEST_BINARY_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR(NAME, O1, O2, BigInt, OP, TEST_VALUES, std::vector<uint64_t>, res.__data_for_testing_only())


#define TEST_ASSIGN_OPERATOR(NAME, O1, O2, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, TestI##NAME##_##O1##_##O2) {\
	test_assignment_operation<2, O1, RT, O1, O2>(\
		[](O1& a, const O2& b) -> O1& { return OP; },\
		[](const O1& res) -> RT { return GET_RT; },\
		TEST_VALUES\
	);\
}

#define TEST_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES) \
TEST_ASSIGN_OPERATOR(NAME, BigInt, O2, OP, TEST_VALUES, std::vector<uint64_t>, res.__data_for_testing_only())



#define TEST_TRINARY_OPERATOR(NAME, O1, O2, O3, R, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, Test##NAME##_##O1##_##O2##_##O3) {\
	test_operation<3, R, RT, O1, O2, O3>(\
		[](const O1& a, const O2& b, const O3& c) -> R { return OP; },\
		[](const R& res) -> RT { return GET_RT; },\
		TEST_VALUES\
	);\
}

#define TEST_TRINARY_OPERATOR_BIGINT(NAME, O1, O2, O3, OP, TEST_VALUES) \
TEST_TRINARY_OPERATOR(NAME, O1, O2, O3, BigInt, OP,TEST_VALUES, std::vector<uint64_t>, res.__data_for_testing_only())


// Bitwise Shift
namespace {

TEST_BINARY_OPERATOR_BIGINT(LShift, BigInt, uint64_t, a << b, get_all_lshift_test_values())

TEST_BINARY_OPERATOR_BIGINT(RShift, BigInt, uint64_t, a >> b, get_all_rshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(LShift, uint64_t, a <<= b, get_all_lshift_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(RShift, uint64_t, a >>= b, get_all_rshift_test_values())

}



// Addition
namespace {

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

TEST_BINARY_OPERATOR_BIGINT(Mul, BigInt, BigInt, a * b, get_all_mul_test_values())

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

TEST_ASSIGN_OPERATOR_BIGINT(Mul, uint64_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, int64_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, uint32_t, a *= b, get_all_mul_test_values())

TEST_ASSIGN_OPERATOR_BIGINT(Mul, int32_t, a *= b, get_all_mul_test_values())

}


#define TEST_DIV_BY_ZERO(NAME, O1, O2, OP) \
TEST(HelloTest, Test##NAME##ByZero_##O1##_##O2) {\
	O1 a = str_to_int<O1>("15");\
	const O2 b = str_to_int<O2>("0");\
	EXPECT_THROW([[maybe_unused]] auto x = OP, std::domain_error);\
}\
TEST(HelloTest, Test##NAME##ZeroByZero_##O1##_##O2) {\
	O1 a = str_to_int<O1>("0");\
	const O2 b = str_to_int<O2>("0");\
	EXPECT_THROW([[maybe_unused]] auto y = OP, std::domain_error);\
}


#define TEST_DIV_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
TEST_BINARY_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT)\
TEST_DIV_BY_ZERO(NAME, O1, O2, OP)

#define TEST_DIV_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES)\
TEST_DIV_BY_ZERO(NAME, O1, O2, OP)

#define TEST_DIV_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES) \
TEST_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES)\
TEST_DIV_BY_ZERO(I##NAME, BigInt, O2, OP)

// Division
namespace {

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

TEST_DIV_OPERATOR_BIGINT(Mod, BigInt, BigInt, a % b, get_all_mod_test_values())

TEST_DIV_OPERATOR(Mod, BigInt, uint64_t, uint64_t, a % b, get_all_mod_test_values(), uint64_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, int64_t, int64_t, a % b, get_all_mod_test_values(), int64_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, uint32_t, uint32_t, a % b, get_all_mod_test_values(), uint32_t, res)

TEST_DIV_OPERATOR(Mod, BigInt, int32_t, int32_t, a % b, get_all_mod_test_values(), int32_t, res)

TEST_DIV_ASSIGN_OPERATOR_BIGINT(Mod, BigInt, a %= b, get_all_mod_test_values())

}


// divmod
namespace {

template<typename RT>
using Divmod_RT = std::tuple<std::vector<uint64_t>, RT>;

template<typename R>
using Divmod_R = DivModResult<BigInt, R>;

#define TEST_DIVMOD(NAME, O1, O2, OP, RT, GET_RT) \
TEST_DIV_OPERATOR(NAME, O1, O2, Divmod_R<O2>, OP, get_all_divmod_test_values(), Divmod_RT<RT>, std::tuple(res.d.__data_for_testing_only(), GET_RT))

TEST_DIVMOD(Divmod, BigInt, BigInt, divmod(a, b), std::vector<uint64_t>, res.r.__data_for_testing_only())

TEST_DIVMOD(Divmod, BigInt, uint64_t, divmod(a, b), uint64_t, res.r)

TEST_DIVMOD(Divmod, BigInt, int64_t, divmod(a, b), int64_t, res.r)

TEST_DIVMOD(Divmod1, BigInt, uint32_t, divmod1(a, b), uint32_t, res.r)

TEST_DIVMOD(Divmod1, BigInt, int32_t, divmod1(a, b), int32_t, res.r)

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

// to_string
namespace {

TEST_UNARY_OPERATOR(to_string_10, BigInt, std::string, bigint::to_string_base10(a), get_all_to_string_10_test_values(), std::string, res)

TEST_UNARY_OPERATOR(to_string_16, BigInt, std::string, bigint::to_string_base16(a), get_all_to_string_16_test_values(), std::string, res)

}
