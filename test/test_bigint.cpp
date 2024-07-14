#include "../src/bigInt.h"

#include <iostream>
#include <functional>
#include <gtest/gtest.h>

namespace {

using namespace bigint;

// usefull constants
#define UINT32_MAX_M2 "4294967294"
#define UINT32_MAX_M1 "4294967295"
#define UINT32_MAX_00 "4294967296"
#define UINT32_MAX_P1 "4294967297"
#define UINT32_MAX_P2 "4294967298"
#define UINT64_MAX_M2 "18446744073709551614"
#define UINT64_MAX_M1 "18446744073709551615"
#define UINT64_MAX_00 "18446744073709551616"
#define UINT64_MAX_P1 "18446744073709551617"
#define UINT64_MAX_P2 "18446744073709551618"


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

inline void expectSimpleInt(const BigInt &actual, const std::vector<uint64_t>& expectedData) {
	EXPECT_EQ(actual.__data_for_testing_only(), expectedData);
}


TEST(HelloTest, TestCreateEmpty) {
	expectSimpleInt(BigInt{}, {0});
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
		expectSimpleInt(BigInt{i}, {i});
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
		expectSimpleInt(BigInt{i}, {i});
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
		expectSimpleInt(BigInt{i}, {std::stoull(i)});
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
	}

	expectSimpleInt(BigInt{"61835470006151583405"}, {6495237785022928557, 3});
	EXPECT_EQ(to_string(BigInt{"61835470006151583405"}), "61835470006151583405");

	expectSimpleInt(BigInt{"340282366920938463500268095579187314689"}, {1, 2, 1});
	EXPECT_EQ(to_string(BigInt{"340282366920938463500268095579187314689"}), "340282366920938463500268095579187314689");
}


template <class O1, class O2, class R>
struct BinOpTestVal {
	O1 left;
	O2 right;
	R expected;

	BinOpTestVal(O1 left, O2 right, R expected)
		: left(left), right(right), expected(expected) {}
};

template <class O1, class O2, class R>
inline void testBinaryOp(const std::function<R(O1, O2)>& operation, const BinOpTestVal<O1, O2, R> &test) {
	auto result = operationb(test.left, test.right);
	EXPECT_EQ(result, test.expected);

}


TEST(HelloTest, TestAdd) {
	const std::vector<BinOpTestVal<BigInt, BigInt, BigInt>> tests = {{
		BinOpTestVal{BigInt("0"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("0"), BigInt("1")},
		BinOpTestVal{BigInt("2"), BigInt("0"), BigInt("2")},
		BinOpTestVal{BigInt("3"), BigInt("0"), BigInt("3")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("0"), BigInt(UINT32_MAX_M1)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("0"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("0"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("0"), BigInt(UINT64_MAX_M1)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("0"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("0"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("0"), BigInt("618354700061515834059999999999999999991")},
		BinOpTestVal{BigInt("0"), BigInt("1"), BigInt("1")},
		BinOpTestVal{BigInt("1"), BigInt("1"), BigInt("2")},
		BinOpTestVal{BigInt("2"), BigInt("1"), BigInt("3")},
		BinOpTestVal{BigInt("3"), BigInt("1"), BigInt("4")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("1"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("1"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("1"), BigInt(UINT32_MAX_P2)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("1"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("1"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("1"), BigInt(UINT64_MAX_P2)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("1"), BigInt("618354700061515834059999999999999999992")},
		BinOpTestVal{BigInt("0"), BigInt("5"), BigInt("5")},
		BinOpTestVal{BigInt("1"), BigInt("5"), BigInt("6")},
		BinOpTestVal{BigInt("2"), BigInt("5"), BigInt("7")},
		BinOpTestVal{BigInt("3"), BigInt("5"), BigInt("8")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("5"), BigInt("618354700061515834059999999999999999996")},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt(UINT32_MAX_00), BigInt("8589934592")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("618354700061515834059999999799999999991"), BigInt("1236709400123031668119999999799999999982")},
	}};

	for (const auto &test : tests) {
		EXPECT_EQ((test.left + test.right).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}

	for (const auto &test : tests) {
		EXPECT_EQ((test.right + test.left).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}
}


TEST(HelloTest, TestSub) {
	const std::vector<BinOpTestVal<BigInt, BigInt, BigInt>> tests = {{
		BinOpTestVal{BigInt("0"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("0"), BigInt("1")},
		BinOpTestVal{BigInt("2"), BigInt("0"), BigInt("2")},
		BinOpTestVal{BigInt("3"), BigInt("0"), BigInt("3")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("0"), BigInt(UINT32_MAX_M1)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("0"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("0"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("0"), BigInt(UINT64_MAX_M1)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("0"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("0"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("0"), BigInt("618354700061515834059999999999999999991")},
		BinOpTestVal{BigInt("1"), BigInt("1"), BigInt("0")},
		BinOpTestVal{BigInt("2"), BigInt("1"), BigInt("1")},
		BinOpTestVal{BigInt("3"), BigInt("1"), BigInt("2")},
		BinOpTestVal{BigInt("4"), BigInt("1"), BigInt("3")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("1"), BigInt(UINT32_MAX_M2)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("1"), BigInt(UINT32_MAX_M1)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("1"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P2), BigInt("1"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("1"), BigInt(UINT64_MAX_M2)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("1"), BigInt(UINT64_MAX_M1)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("1"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_P2), BigInt("1"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("1"), BigInt("618354700061515834059999999999999999990")},
		BinOpTestVal{BigInt("10"), BigInt("5"), BigInt("5")},
		BinOpTestVal{BigInt("11"), BigInt("5"), BigInt("6")},
		BinOpTestVal{BigInt("12"), BigInt("5"), BigInt("7")},
		BinOpTestVal{BigInt("13"), BigInt("5"), BigInt("8")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("5"), BigInt("618354700061515834059999999999999999986")},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt(UINT32_MAX_00), BigInt("0")},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt(UINT32_MAX_00), BigInt("18446744069414584320")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("618354700061515834059999999799999999991"), BigInt("200000000000")},
		BinOpTestVal{BigInt("1236709400123031668119999999799999999982"), BigInt("618354700061515834059999999799999999991"), BigInt("618354700061515834059999999999999999991")},
	}};

	for (const auto &test : tests) {
		EXPECT_EQ((test.left - test.right).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}

	for (const auto &test : tests) {
		EXPECT_EQ((test.left - test.expected).__data_for_testing_only(), test.right.__data_for_testing_only());
	}
}


TEST(HelloTest, TestMul) {
	const std::vector<BinOpTestVal<BigInt, BigInt, BigInt>> tests = {{
		BinOpTestVal{BigInt("0"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("2"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("3"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("0"), BigInt("1"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("1"), BigInt("1")},
		BinOpTestVal{BigInt("2"), BigInt("1"), BigInt("2")},
		BinOpTestVal{BigInt("3"), BigInt("1"), BigInt("3")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("1"), BigInt(UINT32_MAX_M1)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("1"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("1"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("1"), BigInt(UINT64_MAX_M1)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("1"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("1"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("1"), BigInt("618354700061515834059999999999999999991")},
		BinOpTestVal{BigInt("0"), BigInt("5"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("5"), BigInt("5")},
		BinOpTestVal{BigInt("2"), BigInt("5"), BigInt("10")},
		BinOpTestVal{BigInt("3"), BigInt("5"), BigInt("15")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("5"), BigInt("3091773500307579170299999999999999999955")},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt(UINT32_MAX_00), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt(UINT32_MAX_P1), BigInt("18446744082299486209")},
		BinOpTestVal{BigInt(UINT32_MAX_P2), BigInt(UINT32_MAX_P2), BigInt("18446744090889420804")},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt(UINT64_MAX_00), BigInt("340282366920938463463374607431768211456")},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt(UINT64_MAX_P1), BigInt("340282366920938463500268095579187314689")},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt(UINT64_MAX_P2), BigInt("340282366920938463518714839652896866306")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("618354700061515834059999999799999999991"), BigInt("382362535088167210234626361716426516060166448586892714986920000001800000000081")},
	}};

	EXPECT_EQ((BigInt("618354700061515834059999999999999999991") * BigInt("618354700061515834059999999799999999991")).__data_for_testing_only(), BigInt("382362535088167210234626361716426516060166448586892714986920000001800000000081").__data_for_testing_only());

	for (const auto &test : tests) {
		EXPECT_EQ((test.left * test.right).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}

	for (const auto &test : tests) {
		EXPECT_EQ((test.right * test.left).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}
}


TEST(HelloTest, TestDiv) {
	const std::vector<BinOpTestVal<BigInt, BigInt, BigInt>> tests = {{
//		BinOpTestVal{BigInt("0"), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt("1"), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt("2"), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt("3"), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("0"), BigInt("0")},
//		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("0"), BigInt("0")},
		BinOpTestVal{BigInt("0"), BigInt("1"), BigInt("0")},
		BinOpTestVal{BigInt("1"), BigInt("1"), BigInt("1")},
		BinOpTestVal{BigInt("2"), BigInt("1"), BigInt("2")},
		BinOpTestVal{BigInt("3"), BigInt("1"), BigInt("3")},
		BinOpTestVal{BigInt(UINT32_MAX_M1), BigInt("1"), BigInt(UINT32_MAX_M1)},
		BinOpTestVal{BigInt(UINT32_MAX_00), BigInt("1"), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt(UINT32_MAX_P1), BigInt("1"), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt(UINT64_MAX_M1), BigInt("1"), BigInt(UINT64_MAX_M1)},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt("1"), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt(UINT64_MAX_P1), BigInt("1"), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("1"), BigInt("618354700061515834059999999999999999991")},
		BinOpTestVal{BigInt("10"), BigInt("5"), BigInt("2")},
		BinOpTestVal{BigInt("11"), BigInt("5"), BigInt("2")},
		BinOpTestVal{BigInt("12"), BigInt("5"), BigInt("2")},
		BinOpTestVal{BigInt("14"), BigInt("5"), BigInt("2")},
		BinOpTestVal{BigInt("15"), BigInt("5"), BigInt("3")},
		BinOpTestVal{BigInt("97"), BigInt("5"), BigInt("19")},
		BinOpTestVal{BigInt("618354700061515834059999999999999999991"), BigInt("5"), BigInt("123670940012303166811999999999999999998")},
		BinOpTestVal{BigInt(UINT64_MAX_00), BigInt(UINT32_MAX_00), BigInt(UINT32_MAX_00)},
		BinOpTestVal{BigInt("18446744082299486209"), BigInt(UINT32_MAX_P1), BigInt(UINT32_MAX_P1)},
		BinOpTestVal{BigInt("18446744090889420804"), BigInt(UINT32_MAX_P2), BigInt(UINT32_MAX_P2)},
		BinOpTestVal{BigInt("340282366920938463463374607431768211456"), BigInt(UINT64_MAX_00), BigInt(UINT64_MAX_00)},
		BinOpTestVal{BigInt("340282366920938463500268095579187314689"), BigInt(UINT64_MAX_P1), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("340282366920938463518714839652896866306"), BigInt(UINT64_MAX_P2), BigInt(UINT64_MAX_P1)},
		BinOpTestVal{BigInt("382362535088167210234626361716426516060166448586892714986920000001800000000081"), BigInt("618354700061515834059999999799999999991"), BigInt("618354700061515834059999999999999999991")},
	}};

	EXPECT_EQ((BigInt("618354700061515834059999999999999999991") * BigInt("618354700061515834059999999799999999991")).__data_for_testing_only(), BigInt("382362535088167210234626361716426516060166448586892714986920000001800000000081").__data_for_testing_only());

	for (const auto &test : tests) {
		EXPECT_EQ((test.left / test.right).__data_for_testing_only(), test.expected.__data_for_testing_only());
	}
}

}
