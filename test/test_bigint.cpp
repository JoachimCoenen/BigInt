#include "../src/bigInt.h"
#include "../src/utils.h"

#include <functional>
#include <variant>
#include <gtest/gtest.h>

namespace {

using namespace bigint;

// usefull constants
#define INT32_MAX_M2 "2147483645"
#define INT32_MAX_M1 "2147483646"
#define INT32_MAX_00 "2147483647"
#define INT32_MAX_P1 "2147483648"
#define INT32_MAX_P2 "2147483649"
#define INT64_MAX_M2 "9223372036854775805"
#define INT64_MAX_M1 "9223372036854775806"
#define INT64_MAX_00 "9223372036854775807"
#define INT64_MAX_P1 "9223372036854775808"
#define INT64_MAX_P2 "9223372036854775809"

#define UINT32_MAX_M2 "4294967293"
#define UINT32_MAX_M1 "4294967294"
#define UINT32_MAX_00 "4294967295"
#define UINT32_MAX_P1 "4294967296"
#define UINT32_MAX_P2 "4294967297"
#define UINT64_MAX_M2 "18446744073709551613"
#define UINT64_MAX_M1 "18446744073709551614"
#define UINT64_MAX_00 "18446744073709551615"
#define UINT64_MAX_P1 "18446744073709551616"
#define UINT64_MAX_P2 "18446744073709551617"


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


struct IgnoreTest {};
constexpr auto IGNORED = IgnoreTest{};


using ExpectedResult = std::variant<IgnoreTest, std::string_view>; // todo add exception option

struct ExpectedResults {
	ExpectedResult add;
	ExpectedResult sub;
	ExpectedResult mul;
	ExpectedResult div;
	ExpectedResult mod;
};

/**
 * minimum integer size required to fit value
 */
enum class ValType {
	U32 = 0,
	S32 = 1,
	N32 = 2,

	U64 = 3,
	S64 = 4,
	N64 = 5,

	BIG = 6,
};


/**
 * minimum integer size required to fit value
 */
enum class ParamType {
	U32 = 0,
	S32 = 1,

	U64 = 3,
	S64 = 4,

	BIG = 6,
};

using VT = ValType;
using PT = ParamType;

bool is_assignable(const ParamType param, const ValType value) {
	switch (param) {
	case PT::U32:
		return value == VT::U32 || value == VT::S32;
	case PT::S32:
		return value == VT::S32 || value == VT::N32;
	case PT::U64:
		return value == VT::U32 || value == VT::S32 || value == VT::U64 || value == VT::S64;
	case PT::S64:
		return value == VT::U32 || value == VT::S32 || value == VT::N32 || value == VT::S64 || value == VT::N64;
	case PT::BIG:
		return true;
	default:
		// can never happen
		return false;
	}
}

struct Param {
	ValType type;
	std::string_view val;
	constexpr Param (ValType type, std::string_view val)
		: type{type}, val{val} {}
};

struct BinOpTest {
	Param left;
	Param right;
	ExpectedResults expected;

	constexpr BinOpTest(Param left, Param right, ExpectedResults expected)
		: left(left), right(right), expected(expected) {}
};


#include "values_for_test.h"


template <class T>
struct ParamSelector {
	ParamSelector(std::function<Param(BinOpTest)> get, std::function<T(std::string_view)> convert, ParamType type)
		: get{get}, convert{convert}, type{type} {}

	std::function<Param(BinOpTest)> get;
	std::function<T(std::string_view)> convert;
	ParamType type;
};

template <class T>
struct ResultSelector {
	const std::function<ExpectedResult(ExpectedResults)> get;
	const std::function<T(std::string_view)> convert;
};

template <class O1, class O2, class R>
bool isAcceptableTest(
	const ParamSelector<O1> &left_selector,
	const ParamSelector<O2> &right_selector,
	const ResultSelector<R> &result_selector,
	const BinOpTest &test
) {
	return (is_assignable(left_selector.type, left_selector.get(test).type)) &&
		   (is_assignable(right_selector.type, right_selector.get(test).type)) &&
		   !std::holds_alternative<IgnoreTest>(result_selector.get(test.expected));
}

template <class O1, class O2, class R, class RT>
inline void testBinaryOpSingle(
	const std::function<R(O1, O2)>& operation,
	const ParamSelector<O1> &left_selector,
	const ParamSelector<O2> &right_selector,
	const ResultSelector<R> &result_selector,
	const std::function<RT(R)>& get_result_compare_value,
	const BinOpTest &test
) {
	if (!isAcceptableTest(left_selector, right_selector, result_selector, test)) {
		return; // skip
	}
	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  lhs = " << left_selector.get(test).val << std::endl;
	// std::cout << "  rhs = " << right_selector.get(test).val << std::endl;
	// std::cout << "  res = " << std::get<std::string_view>(result_selector.get(test.expected)) << std::endl;

	const auto expected_variant = result_selector.get(test.expected);
	const auto left = left_selector.convert(left_selector.get(test).val);
	const auto right = right_selector.convert(right_selector.get(test).val);
	const auto expected = result_selector.convert(std::get<std::string_view>(expected_variant));

	const auto result = operation(left, right);

	EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << left << " . " << right;

}

template <class O1, class O2, class R, class RT>
inline void testBinaryOp(
	const std::function<R(O1, O2)>& operation,
	const ParamSelector<O1> &left_selector,
	const ParamSelector<O2> &right_selector,
	const ResultSelector<R> &result_selector,
	const std::function<RT(R)>& get_result_compare_value,
	const std::vector<BinOpTest> &tests
) {
	for (const auto &test : tests) {
		testBinaryOpSingle(operation, left_selector, right_selector, result_selector, get_result_compare_value, test);
	}
}


}


// Addition
namespace {

TEST(HelloTest, TestAdd_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

}


// Subtraction
namespace {

TEST(HelloTest, TestSub_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestSub_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestSub_u64_big) {
	// testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
	// testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

}


// Multiplication
namespace {

TEST(HelloTest, TestMul_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_u64_u64) {
	testBinaryOp<uint64_t, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return BigInt{mult(a, b)}; },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<uint64_t, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return BigInt{mult(a, b)}; },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

}


// Division
namespace {

TEST(HelloTest, TestDiv_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestDiv_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestDiv_big_u32) {
	testBinaryOp<BigInt, uint32_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint32_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoul(value); }, PT::U32},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint32_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<uint32_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoul(value); }, PT::U32},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

}


// Modulo
namespace {

TEST(HelloTest, TestMod_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mod; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.mod; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestMod_big_u64) {
	testBinaryOp<BigInt, uint64_t, uint64_t, uint64_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<uint64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoull(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, uint64_t, uint64_t>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, PT::U64},
	// 	ResultSelector<uint64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoull(value); }},
	// 	[](auto res) { return res; },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestMod_big_u32) {
	testBinaryOp<BigInt, uint32_t, uint32_t, uint32_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint32_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoul(value); }, PT::U32},
		ResultSelector<uint32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoul(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint32_t, uint32_t, uint32_t>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, PT::BIG},
	// 	ParamSelector<uint32_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoul(value); }, PT::U32},
	// 	ResultSelector<uint32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoul(value); }},
	// 	[](auto res) { return res; },
	// 	ALL_TEST_VALUES
	// 	);
}

}
