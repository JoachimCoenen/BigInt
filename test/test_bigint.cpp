#include "../src/bigInt.h"
#include "../src/utils.h"

#include "values_for_test.h"

#include <gtest/gtest.h>

namespace {

using namespace bigint;
using namespace test_data;

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


// Addition
namespace {

TEST(HelloTest, TestAdd_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
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
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rsub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestSub_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rsub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestSub_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rsub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

}


// Multiplication
namespace {

TEST(HelloTest, TestMul_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMul_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMul_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMul_u64_u64) {
	testBinaryOp<uint64_t, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return BigInt{mult(a, b)}; },
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
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
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rdiv; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestDiv_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rdiv; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestDiv_big_s64) {
	testBinaryOp<BigInt, int64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<int64_t>{[](auto value) { return utils::stoll(value); }, PT::S64},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rdiv; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestDiv_big_u32) {
	testBinaryOp<BigInt, uint32_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint32_t>{[](auto value) { return utils::stoul(value); }, PT::U32},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rdiv; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestDiv_big_s32) {
	testBinaryOp<BigInt, int32_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<int32_t>{[](auto value) { return utils::stol(value); }, PT::S32},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rdiv; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

}


// Modulo
namespace {

TEST(HelloTest, TestMod_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mod; }, [](auto value) { return BigInt(value); }},
		ResultSelector<BigInt>{[](auto res) { return res.rmod; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMod_big_u64) {
	testBinaryOp<BigInt, uint64_t, uint64_t, uint64_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint64_t>{[](auto value) { return utils::stoull(value); }, PT::U64},
		ResultSelector<uint64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoull(value); }},
		ResultSelector<uint64_t>{[](auto res) { return res.rmod; }, [](auto value) { return utils::stoull(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMod_big_s64) {
	testBinaryOp<BigInt, int64_t, int64_t, int64_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<int64_t>{[](auto value) { return utils::stoll(value); }, PT::S64},
		ResultSelector<int64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoll(value); }},
		ResultSelector<int64_t>{[](auto res) { return res.rmod; }, [](auto value) { return utils::stoll(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMod_big_u32) {
	testBinaryOp<BigInt, uint32_t, uint32_t, uint32_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<uint32_t>{[](auto value) { return utils::stoul(value); }, PT::U32},
		ResultSelector<uint32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoul(value); }},
		ResultSelector<uint32_t>{[](auto res) { return res.rmod; }, [](auto value) { return utils::stoul(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestMod_big_s32) {
	testBinaryOp<BigInt, int32_t, int32_t, int32_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto value) { return BigInt(value); }, PT::BIG},
		ParamSelector<int32_t>{[](auto value) { return utils::stol(value); }, PT::S32},
		ResultSelector<int32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stol(value); }},
		ResultSelector<int32_t>{[](auto res) { return res.rmod; }, [](auto value) { return utils::stol(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
	);
}

}
