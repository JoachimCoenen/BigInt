#include "../src/bigInt.h"
#include "../src/utils.h"

#include <iostream>
#include <functional>
#include <variant>
#include <gtest/gtest.h>

namespace {

using namespace bigint;

// usefull constants
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
		expectSimpleInt(BigInt{i}, {utils::stoull(i)});
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



struct NotSet {};
constexpr auto NOT_SET = NotSet{};

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
	U64 = 1,
	BIG = 2,
};

using VT = ValType;

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

const static std::vector<BinOpTest> ALL_TEST_VALUES = {{
	BinOpTest{Param{VT::U32, "0"}, Param{VT::U32, "0"}, ExpectedResults{
		.add="0",
		.sub="0",
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, "1"}, Param{VT::U32, "0"}, ExpectedResults{
		.add="1",
		.sub="1",
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, "2"}, Param{VT::U32, "0"}, ExpectedResults{
		.add="2",
		.sub="2",
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, "3"}, Param{VT::U32, "0"}, ExpectedResults{
		.add="3",
		.sub="3",
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, UINT32_MAX_M1}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT32_MAX_M1,
		.sub=UINT32_MAX_M1,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT32_MAX_00,
		.sub=UINT32_MAX_00,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U64, UINT32_MAX_P1}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT32_MAX_P1,
		.sub=UINT32_MAX_P1,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_M1}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT64_MAX_M1,
		.sub=UINT64_MAX_M1,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT64_MAX_00,
		.sub=UINT64_MAX_00,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::U32, "0"}, ExpectedResults{
		.add=UINT64_MAX_P1,
		.sub=UINT64_MAX_P1,
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::U32, "0"}, ExpectedResults{
		.add="618354700061515834059999999999999999991",
		.sub="618354700061515834059999999999999999991",
		.mul="0",
		.div=IGNORED,
		.mod=IGNORED,
	}},
	BinOpTest{Param{VT::U32, "0"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="1",
		.sub=IGNORED,
		.mul="0",
		.div="0",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "1"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="2",
		.sub="0",
		.mul="1",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "2"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="3",
		.sub="1",
		.mul="2",
		.div="2",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "3"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="4",
		.sub="2",
		.mul="3",
		.div="3",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, UINT32_MAX_M1}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT32_MAX_00,
		.sub=UINT32_MAX_M2,
		.mul=UINT32_MAX_M1,
		.div=UINT32_MAX_M1,
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT32_MAX_P1,
		.sub=UINT32_MAX_M1,
		.mul=UINT32_MAX_00,
		.div=UINT32_MAX_00,
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT32_MAX_P1}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT32_MAX_P2,
		.sub=UINT32_MAX_00,
		.mul=UINT32_MAX_P1,
		.div=UINT32_MAX_P1,
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_M1}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT64_MAX_00,
		.sub=UINT64_MAX_M2,
		.mul=UINT64_MAX_M1,
		.div=UINT64_MAX_M1,
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT64_MAX_P1,
		.sub=UINT64_MAX_M1,
		.mul=UINT64_MAX_00,
		.div=UINT64_MAX_00,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::U32, "1"}, ExpectedResults{
		.add=UINT64_MAX_P2,
		.sub=UINT64_MAX_00,
		.mul=UINT64_MAX_P1,
		.div=UINT64_MAX_P1,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="618354700061515834059999999999999999992",
		.sub="618354700061515834059999999999999999990",
		.mul="618354700061515834059999999999999999991",
		.div="618354700061515834059999999999999999991",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "0"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="5",
		.sub=IGNORED,
		.mul="0",
		.div="0",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "1"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="6",
		.sub=IGNORED,
		.mul="5",
		.div="0",
		.mod="1",
	}},
	BinOpTest{Param{VT::U32, "2"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="7",
		.sub=IGNORED,
		.mul="10",
		.div="0",
		.mod="2",
	}},
	BinOpTest{Param{VT::U32, "3"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="8",
		.sub=IGNORED,
		.mul="15",
		.div="0",
		.mod="3",
	}},
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="618354700061515834059999999999999999996",
		.sub="618354700061515834059999999999999999986",
		.mul="3091773500307579170299999999999999999955",
		.div="123670940012303166811999999999999999998",
		.mod="1",
	}},
	BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
		.add="8589934590",
		.sub="0",
		.mul="18446744065119617025",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
		.add="1236709400123031668119999999799999999982",
		.sub="200000000000",
		.mul="382362535088167210234626361716426516060166448586892714986920000001800000000081",
		.div="1",
		.mod="200000000000",
	}},
	BinOpTest{Param{VT::U32, "4"}, Param{VT::U32, "1"}, ExpectedResults{
		.add="5",
		.sub="3",
		.mul="4",
		.div="4",
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT32_MAX_P2}, Param{VT::U32, "1"}, ExpectedResults{
		.add="4294967298",
		.sub=UINT32_MAX_P1,
		.mul=UINT32_MAX_P2,
		.div=UINT32_MAX_P2,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::U32, "1"}, ExpectedResults{
		.add="18446744073709551618",
		.sub=UINT64_MAX_P1,
		.mul=UINT64_MAX_P2,
		.div=UINT64_MAX_P2,
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "10"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="15",
		.sub="5",
		.mul="50",
		.div="2",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "11"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="16",
		.sub="6",
		.mul="55",
		.div="2",
		.mod="1",
	}},
	BinOpTest{Param{VT::U32, "12"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="17",
		.sub="7",
		.mul="60",
		.div="2",
		.mod="2",
	}},
	BinOpTest{Param{VT::U32, "13"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="18",
		.sub="8",
		.mul="65",
		.div="2",
		.mod="3",
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
		.add="18446744078004518910",
		.sub="18446744069414584320",
		.mul="79228162495817593515539431425",
		.div=UINT32_MAX_P2,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "1236709400123031668119999999799999999982"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
		.add="1855064100184547502179999999599999999973",
		.sub="618354700061515834059999999999999999991",
		.mul="764725070176334420469252723309182092108029730361785430013840000005400000000162",
		.div="2",
		.mod="200000000000",
	}},
	BinOpTest{Param{VT::U64, UINT32_MAX_P1}, Param{VT::U64, UINT32_MAX_P1}, ExpectedResults{
		.add="8589934592",
		.sub="0",
		.mul=UINT64_MAX_P1,
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT32_MAX_P2}, Param{VT::U64, UINT32_MAX_P2}, ExpectedResults{
		.add="8589934594",
		.sub="0",
		.mul="18446744082299486209",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
		.add="36893488147419103230",
		.sub="0",
		.mul="340282366920938463426481119284349108225",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
		.add="36893488147419103232",
		.sub="0",
		.mul="340282366920938463463374607431768211456",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
		.add="36893488147419103233",
		.sub=IGNORED,
		.mul="340282366920938463481821351505477763072",
		.div="0",
		.mod=UINT64_MAX_P1,
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
		.add="36893488147419103234",
		.sub="0",
		.mul="340282366920938463500268095579187314689",
		.div="1",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "14"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="19",
		.sub="9",
		.mul="70",
		.div="2",
		.mod="4",
	}},
	BinOpTest{Param{VT::U32, "15"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="20",
		.sub="10",
		.mul="75",
		.div="3",
		.mod="0",
	}},
	BinOpTest{Param{VT::U32, "97"}, Param{VT::U32, "5"}, ExpectedResults{
		.add="102",
		.sub="92",
		.mul="485",
		.div="19",
		.mod="2",
	}},
	BinOpTest{Param{VT::U64, "18446744065119617025"}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
		.add="18446744069414584320",
		.sub="18446744060824649730",
		.mul="79228162458924105385300197375",
		.div=UINT32_MAX_00,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::U64, UINT32_MAX_P1}, ExpectedResults{
		.add="18446744078004518912",
		.sub="18446744069414584320",
		.mul="79228162514264337593543950336",
		.div=UINT32_MAX_P1,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "18446744082299486209"}, Param{VT::U64, UINT32_MAX_P2}, ExpectedResults{
		.add="18446744086594453506",
		.sub="18446744078004518912",
		.mul="79228162569604569827557507073",
		.div=UINT32_MAX_P2,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "340282366920938463426481119284349108225"}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
		.add="340282366920938463444927863358058659840",
		.sub="340282366920938463408034375210639556610",
		.mul="6277101735386680762814942322444851025767571854389858533375",
		.div=UINT64_MAX_00,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211456"}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
		.add="340282366920938463481821351505477763072",
		.sub="340282366920938463444927863358058659840",
		.mul="6277101735386680763835789423207666416102355444464034512896",
		.div=UINT64_MAX_P1,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "340282366920938463481821351505477763072"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
		.add="340282366920938463500268095579187314689",
		.sub="340282366920938463463374607431768211455",
		.mul="6277101735386680764516354157049543343047551403401280487424",
		.div=UINT64_MAX_P1,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "340282366920938463500268095579187314689"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
		.add="340282366920938463518714839652896866306",
		.sub="340282366920938463481821351505477763072",
		.mul="6277101735386680764856636523970481806547819498980467802113",
		.div=UINT64_MAX_P2,
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "382362535088167210234626361716426516060166448586892714986920000001800000000081"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
		.add="382362535088167210234626361716426516060784803286954230820980000001600000000072",
		.sub="382362535088167210234626361716426516059548093886831199152860000002000000000090",
		.mul="236435670699204459093713935009106305322803931741435815699776293803790155888735974197346948347316579999967599999999271",
		.div="618354700061515834059999999999999999991",
		.mod="0",
	}},
	BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::BIG, "531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889"}, ExpectedResults{
		.add="85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
		.sub="85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
		.mul="45175124645189023184578764904745573466523720031595903892159066621104110513940600552455540874571215918778304649022541648601808557323245008512881159450610811088902177717844390449838411048294269360247162248387533712273",
		.div="160134",
		.mod="209823076619367981023771469794601869328016582143312911032147521717876940994108137259152513310889145399331",
	}},
	BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::U32, "9"}, ExpectedResults{
		.add="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
		.sub="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
		.mul="765481150483171217545855874755885251935997530976144059722777377966946273785288364322372765612281835884025434113",
		.div="9450384573866311327726615737726978418962932481186963700281202197122793503522078571881145254472615257827474495",
		.mod="2",
	}},
}};




template <class T>
struct ParamSelector {
	ParamSelector(std::function<Param(BinOpTest)> get, std::function<T(std::string_view)> convert, ValType type)
		: get{get}, convert{convert}, type{type} {}

	std::function<Param(BinOpTest)> get;
	std::function<T(std::string_view)> convert;
	ValType type;
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
	return (left_selector.get(test).type <= left_selector.type) &&
		   (right_selector.get(test).type<= right_selector.type) &&
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



// Addition
TEST(HelloTest, TestAdd_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}

TEST(HelloTest, TestAdd_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a + b; },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.add; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
	);
}


// Subtraction
TEST(HelloTest, TestSub_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestSub_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a - b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestSub_u64_big) {
	// testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
	// testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a - b; },
	// 	ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.sub; }, [](auto value) { return BigInt(value); }},
	// [](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}


// Multiplication
TEST(HelloTest, TestMul_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_u64_big) {
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<uint64_t, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a * b; },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}

TEST(HelloTest, TestMul_u64_u64) {
	testBinaryOp<uint64_t, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return mult(a, b); },
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	testBinaryOp<uint64_t, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return mult(a, b); },
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.mul; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
}


// Division
TEST(HelloTest, TestDiv_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestDiv_big_u64) {
	testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestDiv_big_u32) {
	testBinaryOp<BigInt, uint32_t, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a / b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint32_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoul(value); }, VT::U32},
		ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint32_t, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a / b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<uint32_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoul(value); }, VT::U32},
	// 	ResultSelector<BigInt>{[](auto res) { return res.div; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}


// Modulo
TEST(HelloTest, TestMod_big_big) {
	testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ResultSelector<BigInt>{[](auto res) { return res.mod; }, [](auto value) { return BigInt(value); }},
		[](auto res) { return res.__data_for_testing_only(); },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, BigInt, BigInt, std::vector<uint64_t>>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ResultSelector<BigInt>{[](auto res) { return res.mod; }, [](auto value) { return BigInt(value); }},
	// 	[](auto res) { return res.__data_for_testing_only(); },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestMod_big_u64) {
	testBinaryOp<BigInt, uint64_t, uint64_t, uint64_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint64_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoull(value); }, VT::U64},
		ResultSelector<uint64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoull(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint64_t, uint64_t, uint64_t>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<uint64_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoull(value); }, VT::U64},
	// 	ResultSelector<uint64_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoull(value); }},
	// 	[](auto res) { return res; },
	// 	ALL_TEST_VALUES
	// 	);
}

TEST(HelloTest, TestMod_big_u32) {
	testBinaryOp<BigInt, uint32_t, uint32_t, uint32_t>(
		[](auto a, auto b) -> auto { return a % b; },
		ParamSelector<BigInt>{[](auto test) { return test.left; }, [](auto value) { return BigInt(value); }, VT::BIG},
		ParamSelector<uint32_t>{[](auto test) { return test.right; }, [](auto value) { return utils::stoul(value); }, VT::U32},
		ResultSelector<uint32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoul(value); }},
		[](auto res) { return res; },
		ALL_TEST_VALUES
		);
	// testBinaryOp<BigInt, uint32_t, uint32_t, uint32_t>(
	// 	[](auto a, auto b) -> auto { return a % b; },
	// 	ParamSelector<BigInt>{[](auto test) { return test.right; }, [](auto value) { return BigInt(value); }, VT::BIG},
	// 	ParamSelector<uint32_t>{[](auto test) { return test.left; }, [](auto value) { return utils::stoul(value); }, VT::U32},
	// 	ResultSelector<uint32_t>{[](auto res) { return res.mod; }, [](auto value) { return utils::stoul(value); }},
	// 	[](auto res) { return res; },
	// 	ALL_TEST_VALUES
	// 	);
}

}
