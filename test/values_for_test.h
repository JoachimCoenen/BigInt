#ifndef VALUES_FOR_TEST_H
#define VALUES_FOR_TEST_H

#include "utils_for_test.h"
#include <vector>

namespace test_data {

using namespace test_utils;

// usefull constants
#define INT32_MAX_00 "2147483647"
#define INT32_MAX_M2 "2147483645"
#define INT32_MAX_M1 "2147483646"
#define INT32_MAX_P1 "2147483648"
#define INT32_MAX_P2 "2147483649"
#define INT64_MAX_00 "9223372036854775807"
#define INT64_MAX_M2 "9223372036854775805"
#define INT64_MAX_M1 "9223372036854775806"
#define INT64_MAX_P1 "9223372036854775808"
#define INT64_MAX_P2 "9223372036854775809"
#define UINT32_MAX_00 "4294967295"
#define UINT32_MAX_M2 "4294967293"
#define UINT32_MAX_M1 "4294967294"
#define UINT32_MAX_P1 "4294967296"
#define UINT32_MAX_P2 "4294967297"
#define UINT64_MAX_00 "18446744073709551615"
#define UINT64_MAX_M2 "18446744073709551613"
#define UINT64_MAX_M1 "18446744073709551614"
#define UINT64_MAX_P1 "18446744073709551616"
#define UINT64_MAX_P2 "18446744073709551617"

}


namespace test_data {

[[nodiscard]] std::vector<BinOpTest>
get_all_test_values() {
	std::vector<BinOpTest> all_test_values;

	all_test_values.push_back(BinOpTest{Param{VT::S32, "0"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="0",
			.sub="0",
			.rsub="0",
			.mul="0",
			.div=IGNORED,
			.rdiv=IGNORED,
			.mod=IGNORED,
			.rmod=IGNORED,
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "1"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="1",
			.sub="1",
			.rsub="-1",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-1"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-1",
			.sub="-1",
			.rsub="1",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "2"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="2",
			.sub="2",
			.rsub="-2",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-2"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-2",
			.sub="-2",
			.rsub="2",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "3"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="3",
			.sub="3",
			.rsub="-3",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-3"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-3",
			.sub="-3",
			.rsub="3",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U32, UINT32_MAX_M1}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT32_MAX_M1,
			.sub=UINT32_MAX_M1,
			.rsub="-4294967294",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967294"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-4294967294",
			.sub="-4294967294",
			.rsub=UINT32_MAX_M1,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT32_MAX_00,
			.sub=UINT32_MAX_00,
			.rsub="-4294967295",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967295"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-4294967295",
			.sub="-4294967295",
			.rsub=UINT32_MAX_00,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S64, UINT32_MAX_P1}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT32_MAX_P1,
			.sub=UINT32_MAX_P1,
			.rsub="-4294967296",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967296"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-4294967296",
			.sub="-4294967296",
			.rsub=UINT32_MAX_P1,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_M1}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT64_MAX_M1,
			.sub=UINT64_MAX_M1,
			.rsub="-18446744073709551614",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551614"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-18446744073709551614",
			.sub="-18446744073709551614",
			.rsub=UINT64_MAX_M1,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT64_MAX_00,
			.sub=UINT64_MAX_00,
			.rsub="-18446744073709551615",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-18446744073709551615",
			.sub="-18446744073709551615",
			.rsub=UINT64_MAX_00,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::S32, "0"}, ExpectedResults{
			.add=UINT64_MAX_P1,
			.sub=UINT64_MAX_P1,
			.rsub="-18446744073709551616",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-18446744073709551616",
			.sub="-18446744073709551616",
			.rsub=UINT64_MAX_P1,
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="618354700061515834059999999999999999991",
			.sub="618354700061515834059999999999999999991",
			.rsub="-618354700061515834059999999999999999991",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::S32, "0"}, ExpectedResults{
			.add="-618354700061515834059999999999999999991",
			.sub="-618354700061515834059999999999999999991",
			.rsub="618354700061515834059999999999999999991",
			.mul="0",
			.div=IGNORED,
			.rdiv="0",
			.mod=IGNORED,
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "0"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="1",
			.sub="-1",
			.rsub="1",
			.mul="0",
			.div="0",
			.rdiv=IGNORED,
			.mod="0",
			.rmod=IGNORED,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "0"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-1",
			.sub="1",
			.rsub="-1",
			.mul="0",
			.div="0",
			.rdiv=IGNORED,
			.mod="0",
			.rmod=IGNORED,
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "1"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="2",
			.sub="0",
			.rsub="0",
			.mul="1",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-1"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="0",
			.sub="-2",
			.rsub="2",
			.mul="-1",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "1"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="0",
			.sub="2",
			.rsub="-2",
			.mul="-1",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-1"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-2",
			.sub="0",
			.rsub="0",
			.mul="1",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "2"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="3",
			.sub="1",
			.rsub="-1",
			.mul="2",
			.div="2",
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-2"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-1",
			.sub="-3",
			.rsub="3",
			.mul="-2",
			.div="-2",
			.rdiv="-1",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "2"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="1",
			.sub="3",
			.rsub="-3",
			.mul="-2",
			.div="-2",
			.rdiv="-1",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-2"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-3",
			.sub="-1",
			.rsub="1",
			.mul="2",
			.div="2",
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "3"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="4",
			.sub="2",
			.rsub="-2",
			.mul="3",
			.div="3",
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-3"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-2",
			.sub="-4",
			.rsub="4",
			.mul="-3",
			.div="-3",
			.rdiv="-1",
			.mod="0",
			.rmod="-2",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "3"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="2",
			.sub="4",
			.rsub="-4",
			.mul="-3",
			.div="-3",
			.rdiv="-1",
			.mod="0",
			.rmod="2",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-3"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-4",
			.sub="-2",
			.rsub="2",
			.mul="3",
			.div="3",
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U32, UINT32_MAX_M1}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT32_MAX_00,
			.sub=UINT32_MAX_M2,
			.rsub="-4294967293",
			.mul=UINT32_MAX_M1,
			.div=UINT32_MAX_M1,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967294"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-4294967293",
			.sub="-4294967295",
			.rsub=UINT32_MAX_00,
			.mul="-4294967294",
			.div="-4294967294",
			.rdiv="-1",
			.mod="0",
			.rmod="-4294967293",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U32, UINT32_MAX_M1}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT32_MAX_M2,
			.sub=UINT32_MAX_00,
			.rsub="-4294967295",
			.mul="-4294967294",
			.div="-4294967294",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT32_MAX_M2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967294"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-4294967295",
			.sub="-4294967293",
			.rsub=UINT32_MAX_M2,
			.mul=UINT32_MAX_M1,
			.div=UINT32_MAX_M1,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT32_MAX_P1,
			.sub=UINT32_MAX_M1,
			.rsub="-4294967294",
			.mul=UINT32_MAX_00,
			.div=UINT32_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967295"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-4294967294",
			.sub="-4294967296",
			.rsub=UINT32_MAX_P1,
			.mul="-4294967295",
			.div="-4294967295",
			.rdiv="-1",
			.mod="0",
			.rmod="-4294967294",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT32_MAX_M1,
			.sub=UINT32_MAX_P1,
			.rsub="-4294967296",
			.mul="-4294967295",
			.div="-4294967295",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT32_MAX_M1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967295"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-4294967296",
			.sub="-4294967294",
			.rsub=UINT32_MAX_M1,
			.mul=UINT32_MAX_00,
			.div=UINT32_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S64, UINT32_MAX_P1}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT32_MAX_P2,
			.sub=UINT32_MAX_00,
			.rsub="-4294967295",
			.mul=UINT32_MAX_P1,
			.div=UINT32_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967296"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-4294967295",
			.sub="-4294967297",
			.rsub=UINT32_MAX_P2,
			.mul="-4294967296",
			.div="-4294967296",
			.rdiv="-1",
			.mod="0",
			.rmod="-4294967295",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S64, UINT32_MAX_P1}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT32_MAX_00,
			.sub=UINT32_MAX_P2,
			.rsub="-4294967297",
			.mul="-4294967296",
			.div="-4294967296",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT32_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967296"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-4294967297",
			.sub="-4294967295",
			.rsub=UINT32_MAX_00,
			.mul=UINT32_MAX_P1,
			.div=UINT32_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_M1}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT64_MAX_00,
			.sub=UINT64_MAX_M2,
			.rsub="-18446744073709551613",
			.mul=UINT64_MAX_M1,
			.div=UINT64_MAX_M1,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551614"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-18446744073709551613",
			.sub="-18446744073709551615",
			.rsub=UINT64_MAX_00,
			.mul="-18446744073709551614",
			.div="-18446744073709551614",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744073709551613",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U64, UINT64_MAX_M1}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT64_MAX_M2,
			.sub=UINT64_MAX_00,
			.rsub="-18446744073709551615",
			.mul="-18446744073709551614",
			.div="-18446744073709551614",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT64_MAX_M2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551614"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-18446744073709551615",
			.sub="-18446744073709551613",
			.rsub=UINT64_MAX_M2,
			.mul=UINT64_MAX_M1,
			.div=UINT64_MAX_M1,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT64_MAX_P1,
			.sub=UINT64_MAX_M1,
			.rsub="-18446744073709551614",
			.mul=UINT64_MAX_00,
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-18446744073709551614",
			.sub="-18446744073709551616",
			.rsub=UINT64_MAX_P1,
			.mul="-18446744073709551615",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744073709551614",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT64_MAX_M1,
			.sub=UINT64_MAX_P1,
			.rsub="-18446744073709551616",
			.mul="-18446744073709551615",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT64_MAX_M1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-18446744073709551616",
			.sub="-18446744073709551614",
			.rsub=UINT64_MAX_M1,
			.mul=UINT64_MAX_00,
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::S32, "1"}, ExpectedResults{
			.add=UINT64_MAX_P2,
			.sub=UINT64_MAX_00,
			.rsub="-18446744073709551615",
			.mul=UINT64_MAX_P1,
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-18446744073709551615",
			.sub="-18446744073709551617",
			.rsub=UINT64_MAX_P2,
			.mul="-18446744073709551616",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744073709551615",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT64_MAX_00,
			.sub=UINT64_MAX_P2,
			.rsub="-18446744073709551617",
			.mul="-18446744073709551616",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT64_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-18446744073709551617",
			.sub="-18446744073709551615",
			.rsub=UINT64_MAX_00,
			.mul=UINT64_MAX_P1,
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="618354700061515834059999999999999999992",
			.sub="618354700061515834059999999999999999990",
			.rsub="-618354700061515834059999999999999999990",
			.mul="618354700061515834059999999999999999991",
			.div="618354700061515834059999999999999999991",
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-618354700061515834059999999999999999990",
			.sub="-618354700061515834059999999999999999992",
			.rsub="618354700061515834059999999999999999992",
			.mul="-618354700061515834059999999999999999991",
			.div="-618354700061515834059999999999999999991",
			.rdiv="-1",
			.mod="0",
			.rmod="-618354700061515834059999999999999999990",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="618354700061515834059999999999999999990",
			.sub="618354700061515834059999999999999999992",
			.rsub="-618354700061515834059999999999999999992",
			.mul="-618354700061515834059999999999999999991",
			.div="-618354700061515834059999999999999999991",
			.rdiv="-1",
			.mod="0",
			.rmod="618354700061515834059999999999999999990",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-618354700061515834059999999999999999992",
			.sub="-618354700061515834059999999999999999990",
			.rsub="618354700061515834059999999999999999990",
			.mul="618354700061515834059999999999999999991",
			.div="618354700061515834059999999999999999991",
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "0"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="5",
			.sub="-5",
			.rsub="5",
			.mul="0",
			.div="0",
			.rdiv=IGNORED,
			.mod="0",
			.rmod=IGNORED,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "0"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-5",
			.sub="5",
			.rsub="-5",
			.mul="0",
			.div="0",
			.rdiv=IGNORED,
			.mod="0",
			.rmod=IGNORED,
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "1"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="6",
			.sub="-4",
			.rsub="4",
			.mul="5",
			.div="0",
			.rdiv="5",
			.mod="1",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-1"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="4",
			.sub="-6",
			.rsub="6",
			.mul="-5",
			.div="-1",
			.rdiv="-5",
			.mod="4",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "1"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-4",
			.sub="6",
			.rsub="-6",
			.mul="-5",
			.div="-1",
			.rdiv="-5",
			.mod="-4",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-1"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-6",
			.sub="4",
			.rsub="-4",
			.mul="5",
			.div="0",
			.rdiv="5",
			.mod="-1",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "2"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="7",
			.sub="-3",
			.rsub="3",
			.mul="10",
			.div="0",
			.rdiv="2",
			.mod="2",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-2"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="3",
			.sub="-7",
			.rsub="7",
			.mul="-10",
			.div="-1",
			.rdiv="-3",
			.mod="3",
			.rmod="-1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "2"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-3",
			.sub="7",
			.rsub="-7",
			.mul="-10",
			.div="-1",
			.rdiv="-3",
			.mod="-3",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-2"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-7",
			.sub="3",
			.rsub="-3",
			.mul="10",
			.div="0",
			.rdiv="2",
			.mod="-2",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "3"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="8",
			.sub="-2",
			.rsub="2",
			.mul="15",
			.div="0",
			.rdiv="1",
			.mod="3",
			.rmod="2",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-3"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="2",
			.sub="-8",
			.rsub="8",
			.mul="-15",
			.div="-1",
			.rdiv="-2",
			.mod="2",
			.rmod="-1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "3"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-2",
			.sub="8",
			.rsub="-8",
			.mul="-15",
			.div="-1",
			.rdiv="-2",
			.mod="-2",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-3"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-8",
			.sub="2",
			.rsub="-2",
			.mul="15",
			.div="0",
			.rdiv="1",
			.mod="-3",
			.rmod="-2",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="618354700061515834059999999999999999996",
			.sub="618354700061515834059999999999999999986",
			.rsub="-618354700061515834059999999999999999986",
			.mul="3091773500307579170299999999999999999955",
			.div="123670940012303166811999999999999999998",
			.rdiv="0",
			.mod="1",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-618354700061515834059999999999999999986",
			.sub="-618354700061515834059999999999999999996",
			.rsub="618354700061515834059999999999999999996",
			.mul="-3091773500307579170299999999999999999955",
			.div="-123670940012303166811999999999999999999",
			.rdiv="-1",
			.mod="4",
			.rmod="-618354700061515834059999999999999999986",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="618354700061515834059999999999999999986",
			.sub="618354700061515834059999999999999999996",
			.rsub="-618354700061515834059999999999999999996",
			.mul="-3091773500307579170299999999999999999955",
			.div="-123670940012303166811999999999999999999",
			.rdiv="-1",
			.mod="-4",
			.rmod="618354700061515834059999999999999999986",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-618354700061515834059999999999999999996",
			.sub="-618354700061515834059999999999999999986",
			.rsub="618354700061515834059999999999999999986",
			.mul="3091773500307579170299999999999999999955",
			.div="123670940012303166811999999999999999998",
			.rdiv="0",
			.mod="-1",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="8589934590",
			.sub="0",
			.rsub="0",
			.mul="18446744065119617025",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967295"}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="0",
			.sub="-8589934590",
			.rsub="8589934590",
			.mul="-18446744065119617025",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U32, UINT32_MAX_00}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="0",
			.sub="8589934590",
			.rsub="-8589934590",
			.mul="-18446744065119617025",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967295"}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="-8589934590",
			.sub="0",
			.rsub="0",
			.mul="18446744065119617025",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="1236709400123031668119999999799999999982",
			.sub="200000000000",
			.rsub="-200000000000",
			.mul="382362535088167210234626361716426516060166448586892714986920000001800000000081",
			.div="1",
			.rdiv="0",
			.mod="200000000000",
			.rmod="618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-200000000000",
			.sub="-1236709400123031668119999999799999999982",
			.rsub="1236709400123031668119999999799999999982",
			.mul="-382362535088167210234626361716426516060166448586892714986920000001800000000081",
			.div="-2",
			.rdiv="-1",
			.mod="618354700061515834059999999599999999991",
			.rmod="-200000000000",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "618354700061515834059999999999999999991"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="200000000000",
			.sub="1236709400123031668119999999799999999982",
			.rsub="-1236709400123031668119999999799999999982",
			.mul="-382362535088167210234626361716426516060166448586892714986920000001800000000081",
			.div="-2",
			.rdiv="-1",
			.mod="-618354700061515834059999999599999999991",
			.rmod="200000000000",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-618354700061515834059999999999999999991"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-1236709400123031668119999999799999999982",
			.sub="-200000000000",
			.rsub="200000000000",
			.mul="382362535088167210234626361716426516060166448586892714986920000001800000000081",
			.div="1",
			.rdiv="0",
			.mod="-200000000000",
			.rmod="-618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "4"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="5",
			.sub="3",
			.rsub="-3",
			.mul="4",
			.div="4",
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-4"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-3",
			.sub="-5",
			.rsub="5",
			.mul="-4",
			.div="-4",
			.rdiv="-1",
			.mod="0",
			.rmod="-3",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "4"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="3",
			.sub="5",
			.rsub="-5",
			.mul="-4",
			.div="-4",
			.rdiv="-1",
			.mod="0",
			.rmod="3",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-4"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-5",
			.sub="-3",
			.rsub="3",
			.mul="4",
			.div="4",
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S64, UINT32_MAX_P2}, Param{VT::S32, "1"}, ExpectedResults{
			.add="4294967298",
			.sub=UINT32_MAX_P1,
			.rsub="-4294967296",
			.mul=UINT32_MAX_P2,
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967297"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-4294967296",
			.sub="-4294967298",
			.rsub="4294967298",
			.mul="-4294967297",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod="-4294967296",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S64, UINT32_MAX_P2}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT32_MAX_P1,
			.sub="4294967298",
			.rsub="-4294967298",
			.mul="-4294967297",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT32_MAX_P1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967297"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-4294967298",
			.sub="-4294967296",
			.rsub=UINT32_MAX_P1,
			.mul=UINT32_MAX_P2,
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::S32, "1"}, ExpectedResults{
			.add="18446744073709551618",
			.sub=UINT64_MAX_P1,
			.rsub="-18446744073709551616",
			.mul=UINT64_MAX_P2,
			.div=UINT64_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551617"}, Param{VT::S32, "1"}, ExpectedResults{
			.add="-18446744073709551616",
			.sub="-18446744073709551618",
			.rsub="18446744073709551618",
			.mul="-18446744073709551617",
			.div="-18446744073709551617",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744073709551616",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::N32, "-1"}, ExpectedResults{
			.add=UINT64_MAX_P1,
			.sub="18446744073709551618",
			.rsub="-18446744073709551618",
			.mul="-18446744073709551617",
			.div="-18446744073709551617",
			.rdiv="-1",
			.mod="0",
			.rmod=UINT64_MAX_P1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551617"}, Param{VT::N32, "-1"}, ExpectedResults{
			.add="-18446744073709551618",
			.sub="-18446744073709551616",
			.rsub=UINT64_MAX_P1,
			.mul=UINT64_MAX_P2,
			.div=UINT64_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "10"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="15",
			.sub="5",
			.rsub="-5",
			.mul="50",
			.div="2",
			.rdiv="0",
			.mod="0",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-10"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-5",
			.sub="-15",
			.rsub="15",
			.mul="-50",
			.div="-2",
			.rdiv="-1",
			.mod="0",
			.rmod="-5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "10"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="5",
			.sub="15",
			.rsub="-15",
			.mul="-50",
			.div="-2",
			.rdiv="-1",
			.mod="0",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-10"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-15",
			.sub="-5",
			.rsub="5",
			.mul="50",
			.div="2",
			.rdiv="0",
			.mod="0",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "11"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="16",
			.sub="6",
			.rsub="-6",
			.mul="55",
			.div="2",
			.rdiv="0",
			.mod="1",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-11"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-6",
			.sub="-16",
			.rsub="16",
			.mul="-55",
			.div="-3",
			.rdiv="-1",
			.mod="4",
			.rmod="-6",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "11"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="6",
			.sub="16",
			.rsub="-16",
			.mul="-55",
			.div="-3",
			.rdiv="-1",
			.mod="-4",
			.rmod="6",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-11"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-16",
			.sub="-6",
			.rsub="6",
			.mul="55",
			.div="2",
			.rdiv="0",
			.mod="-1",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "12"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="17",
			.sub="7",
			.rsub="-7",
			.mul="60",
			.div="2",
			.rdiv="0",
			.mod="2",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-12"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-7",
			.sub="-17",
			.rsub="17",
			.mul="-60",
			.div="-3",
			.rdiv="-1",
			.mod="3",
			.rmod="-7",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "12"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="7",
			.sub="17",
			.rsub="-17",
			.mul="-60",
			.div="-3",
			.rdiv="-1",
			.mod="-3",
			.rmod="7",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-12"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-17",
			.sub="-7",
			.rsub="7",
			.mul="60",
			.div="2",
			.rdiv="0",
			.mod="-2",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "13"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="18",
			.sub="8",
			.rsub="-8",
			.mul="65",
			.div="2",
			.rdiv="0",
			.mod="3",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-13"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-8",
			.sub="-18",
			.rsub="18",
			.mul="-65",
			.div="-3",
			.rdiv="-1",
			.mod="2",
			.rmod="-8",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "13"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="8",
			.sub="18",
			.rsub="-18",
			.mul="-65",
			.div="-3",
			.rdiv="-1",
			.mod="-2",
			.rmod="8",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-13"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-18",
			.sub="-8",
			.rsub="8",
			.mul="65",
			.div="2",
			.rdiv="0",
			.mod="-3",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="18446744078004518910",
			.sub="18446744069414584320",
			.rsub="-18446744069414584320",
			.mul="79228162495817593515539431425",
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod=UINT32_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="-18446744069414584320",
			.sub="-18446744078004518910",
			.rsub="18446744078004518910",
			.mul="-79228162495817593515539431425",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744069414584320",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="18446744069414584320",
			.sub="18446744078004518910",
			.rsub="-18446744078004518910",
			.mul="-79228162495817593515539431425",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod="18446744069414584320",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="-18446744078004518910",
			.sub="-18446744069414584320",
			.rsub="18446744069414584320",
			.mul="79228162495817593515539431425",
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="-4294967295",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "1236709400123031668119999999799999999982"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="1855064100184547502179999999599999999973",
			.sub="618354700061515834059999999999999999991",
			.rsub="-618354700061515834059999999999999999991",
			.mul="764725070176334420469252723309182092108029730361785430013840000005400000000162",
			.div="2",
			.rdiv="0",
			.mod="200000000000",
			.rmod="618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-1236709400123031668119999999799999999982"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-618354700061515834059999999999999999991",
			.sub="-1855064100184547502179999999599999999973",
			.rsub="1855064100184547502179999999599999999973",
			.mul="-764725070176334420469252723309182092108029730361785430013840000005400000000162",
			.div="-3",
			.rdiv="-1",
			.mod="618354700061515834059999999599999999991",
			.rmod="-618354700061515834059999999999999999991",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "1236709400123031668119999999799999999982"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="618354700061515834059999999999999999991",
			.sub="1855064100184547502179999999599999999973",
			.rsub="-1855064100184547502179999999599999999973",
			.mul="-764725070176334420469252723309182092108029730361785430013840000005400000000162",
			.div="-3",
			.rdiv="-1",
			.mod="-618354700061515834059999999599999999991",
			.rmod="618354700061515834059999999999999999991",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-1236709400123031668119999999799999999982"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-1855064100184547502179999999599999999973",
			.sub="-618354700061515834059999999999999999991",
			.rsub="618354700061515834059999999999999999991",
			.mul="764725070176334420469252723309182092108029730361785430013840000005400000000162",
			.div="2",
			.rdiv="0",
			.mod="-200000000000",
			.rmod="-618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S64, UINT32_MAX_P1}, Param{VT::S64, UINT32_MAX_P1}, ExpectedResults{
			.add="8589934592",
			.sub="0",
			.rsub="0",
			.mul=UINT64_MAX_P1,
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967296"}, Param{VT::S64, UINT32_MAX_P1}, ExpectedResults{
			.add="0",
			.sub="-8589934592",
			.rsub="8589934592",
			.mul="-18446744073709551616",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S64, UINT32_MAX_P1}, Param{VT::N64, "-4294967296"}, ExpectedResults{
			.add="0",
			.sub="8589934592",
			.rsub="-8589934592",
			.mul="-18446744073709551616",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967296"}, Param{VT::N64, "-4294967296"}, ExpectedResults{
			.add="-8589934592",
			.sub="0",
			.rsub="0",
			.mul=UINT64_MAX_P1,
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S64, UINT32_MAX_P2}, Param{VT::S64, UINT32_MAX_P2}, ExpectedResults{
			.add="8589934594",
			.sub="0",
			.rsub="0",
			.mul="18446744082299486209",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967297"}, Param{VT::S64, UINT32_MAX_P2}, ExpectedResults{
			.add="0",
			.sub="-8589934594",
			.rsub="8589934594",
			.mul="-18446744082299486209",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S64, UINT32_MAX_P2}, Param{VT::N64, "-4294967297"}, ExpectedResults{
			.add="0",
			.sub="8589934594",
			.rsub="-8589934594",
			.mul="-18446744082299486209",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N64, "-4294967297"}, Param{VT::N64, "-4294967297"}, ExpectedResults{
			.add="-8589934594",
			.sub="0",
			.rsub="0",
			.mul="18446744082299486209",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
			.add="36893488147419103230",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463426481119284349108225",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
			.add="0",
			.sub="-36893488147419103230",
			.rsub="36893488147419103230",
			.mul="-340282366920938463426481119284349108225",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U64, UINT64_MAX_00}, Param{VT::BIG, "-18446744073709551615"}, ExpectedResults{
			.add="0",
			.sub="36893488147419103230",
			.rsub="-36893488147419103230",
			.mul="-340282366920938463426481119284349108225",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551615"}, Param{VT::BIG, "-18446744073709551615"}, ExpectedResults{
			.add="-36893488147419103230",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463426481119284349108225",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
			.add="36893488147419103232",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463463374607431768211456",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
			.add="0",
			.sub="-36893488147419103232",
			.rsub="36893488147419103232",
			.mul="-340282366920938463463374607431768211456",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, "-18446744073709551616"}, ExpectedResults{
			.add="0",
			.sub="36893488147419103232",
			.rsub="-36893488147419103232",
			.mul="-340282366920938463463374607431768211456",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::BIG, "-18446744073709551616"}, ExpectedResults{
			.add="-36893488147419103232",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463463374607431768211456",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="36893488147419103233",
			.sub="-1",
			.rsub="1",
			.mul="340282366920938463481821351505477763072",
			.div="0",
			.rdiv="1",
			.mod=UINT64_MAX_P1,
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="1",
			.sub="-36893488147419103233",
			.rsub="36893488147419103233",
			.mul="-340282366920938463481821351505477763072",
			.div="-1",
			.rdiv="-2",
			.mod="1",
			.rmod="-18446744073709551615",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-1",
			.sub="36893488147419103233",
			.rsub="-36893488147419103233",
			.mul="-340282366920938463481821351505477763072",
			.div="-1",
			.rdiv="-2",
			.mod="-1",
			.rmod=UINT64_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-36893488147419103233",
			.sub="1",
			.rsub="-1",
			.mul="340282366920938463481821351505477763072",
			.div="0",
			.rdiv="1",
			.mod="-18446744073709551616",
			.rmod="-1",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="36893488147419103234",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463500268095579187314689",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551617"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="0",
			.sub="-36893488147419103234",
			.rsub="36893488147419103234",
			.mul="-340282366920938463500268095579187314689",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P2}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="0",
			.sub="36893488147419103234",
			.rsub="-36893488147419103234",
			.mul="-340282366920938463500268095579187314689",
			.div="-1",
			.rdiv="-1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551617"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-36893488147419103234",
			.sub="0",
			.rsub="0",
			.mul="340282366920938463500268095579187314689",
			.div="1",
			.rdiv="1",
			.mod="0",
			.rmod="0",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "14"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="19",
			.sub="9",
			.rsub="-9",
			.mul="70",
			.div="2",
			.rdiv="0",
			.mod="4",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-14"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-9",
			.sub="-19",
			.rsub="19",
			.mul="-70",
			.div="-3",
			.rdiv="-1",
			.mod="1",
			.rmod="-9",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "14"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="9",
			.sub="19",
			.rsub="-19",
			.mul="-70",
			.div="-3",
			.rdiv="-1",
			.mod="-1",
			.rmod="9",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-14"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-19",
			.sub="-9",
			.rsub="9",
			.mul="70",
			.div="2",
			.rdiv="0",
			.mod="-4",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "15"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="20",
			.sub="10",
			.rsub="-10",
			.mul="75",
			.div="3",
			.rdiv="0",
			.mod="0",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-15"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-10",
			.sub="-20",
			.rsub="20",
			.mul="-75",
			.div="-3",
			.rdiv="-1",
			.mod="0",
			.rmod="-10",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "15"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="10",
			.sub="20",
			.rsub="-20",
			.mul="-75",
			.div="-3",
			.rdiv="-1",
			.mod="0",
			.rmod="10",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-15"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-20",
			.sub="-10",
			.rsub="10",
			.mul="75",
			.div="3",
			.rdiv="0",
			.mod="0",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::S32, "97"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="102",
			.sub="92",
			.rsub="-92",
			.mul="485",
			.div="19",
			.rdiv="0",
			.mod="2",
			.rmod="5",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-97"}, Param{VT::S32, "5"}, ExpectedResults{
			.add="-92",
			.sub="-102",
			.rsub="102",
			.mul="-485",
			.div="-20",
			.rdiv="-1",
			.mod="3",
			.rmod="-92",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::S32, "97"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="92",
			.sub="102",
			.rsub="-102",
			.mul="-485",
			.div="-20",
			.rdiv="-1",
			.mod="-3",
			.rmod="92",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::N32, "-97"}, Param{VT::N32, "-5"}, ExpectedResults{
			.add="-102",
			.sub="-92",
			.rsub="92",
			.mul="485",
			.div="19",
			.rdiv="0",
			.mod="-2",
			.rmod="-5",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::U64, "18446744065119617025"}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="18446744069414584320",
			.sub="18446744060824649730",
			.rsub="-18446744060824649730",
			.mul="79228162458924105385300197375",
			.div=UINT32_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod=UINT32_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744065119617025"}, Param{VT::U32, UINT32_MAX_00}, ExpectedResults{
			.add="-18446744060824649730",
			.sub="-18446744069414584320",
			.rsub="18446744069414584320",
			.mul="-79228162458924105385300197375",
			.div="-4294967295",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744060824649730",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::U64, "18446744065119617025"}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="18446744060824649730",
			.sub="18446744069414584320",
			.rsub="-18446744069414584320",
			.mul="-79228162458924105385300197375",
			.div="-4294967295",
			.rdiv="-1",
			.mod="0",
			.rmod="18446744060824649730",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744065119617025"}, Param{VT::N64, "-4294967295"}, ExpectedResults{
			.add="-18446744069414584320",
			.sub="-18446744060824649730",
			.rsub="18446744060824649730",
			.mul="79228162458924105385300197375",
			.div=UINT32_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="-4294967295",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::S64, UINT32_MAX_P1}, ExpectedResults{
			.add="18446744078004518912",
			.sub="18446744069414584320",
			.rsub="-18446744069414584320",
			.mul="79228162514264337593543950336",
			.div=UINT32_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod=UINT32_MAX_P1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::S64, UINT32_MAX_P1}, ExpectedResults{
			.add="-18446744069414584320",
			.sub="-18446744078004518912",
			.rsub="18446744078004518912",
			.mul="-79228162514264337593543950336",
			.div="-4294967296",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744069414584320",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, UINT64_MAX_P1}, Param{VT::N64, "-4294967296"}, ExpectedResults{
			.add="18446744069414584320",
			.sub="18446744078004518912",
			.rsub="-18446744078004518912",
			.mul="-79228162514264337593543950336",
			.div="-4294967296",
			.rdiv="-1",
			.mod="0",
			.rmod="18446744069414584320",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744073709551616"}, Param{VT::N64, "-4294967296"}, ExpectedResults{
			.add="-18446744078004518912",
			.sub="-18446744069414584320",
			.rsub="18446744069414584320",
			.mul="79228162514264337593543950336",
			.div=UINT32_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="-4294967296",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "18446744082299486209"}, Param{VT::S64, UINT32_MAX_P2}, ExpectedResults{
			.add="18446744086594453506",
			.sub="18446744078004518912",
			.rsub="-18446744078004518912",
			.mul="79228162569604569827557507073",
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod=UINT32_MAX_P2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744082299486209"}, Param{VT::S64, UINT32_MAX_P2}, ExpectedResults{
			.add="-18446744078004518912",
			.sub="-18446744086594453506",
			.rsub="18446744086594453506",
			.mul="-79228162569604569827557507073",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod="-18446744078004518912",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "18446744082299486209"}, Param{VT::N64, "-4294967297"}, ExpectedResults{
			.add="18446744078004518912",
			.sub="18446744086594453506",
			.rsub="-18446744086594453506",
			.mul="-79228162569604569827557507073",
			.div="-4294967297",
			.rdiv="-1",
			.mod="0",
			.rmod="18446744078004518912",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-18446744082299486209"}, Param{VT::N64, "-4294967297"}, ExpectedResults{
			.add="-18446744086594453506",
			.sub="-18446744078004518912",
			.rsub="18446744078004518912",
			.mul="79228162569604569827557507073",
			.div=UINT32_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="-4294967297",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463426481119284349108225"}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
			.add="340282366920938463444927863358058659840",
			.sub="340282366920938463408034375210639556610",
			.rsub="-340282366920938463408034375210639556610",
			.mul="6277101735386680762814942322444851025767571854389858533375",
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod=UINT64_MAX_00,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463426481119284349108225"}, Param{VT::U64, UINT64_MAX_00}, ExpectedResults{
			.add="-340282366920938463408034375210639556610",
			.sub="-340282366920938463444927863358058659840",
			.rsub="340282366920938463444927863358058659840",
			.mul="-6277101735386680762814942322444851025767571854389858533375",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod="-340282366920938463408034375210639556610",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463426481119284349108225"}, Param{VT::BIG, "-18446744073709551615"}, ExpectedResults{
			.add="340282366920938463408034375210639556610",
			.sub="340282366920938463444927863358058659840",
			.rsub="-340282366920938463444927863358058659840",
			.mul="-6277101735386680762814942322444851025767571854389858533375",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod="340282366920938463408034375210639556610",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463426481119284349108225"}, Param{VT::BIG, "-18446744073709551615"}, ExpectedResults{
			.add="-340282366920938463444927863358058659840",
			.sub="-340282366920938463408034375210639556610",
			.rsub="340282366920938463408034375210639556610",
			.mul="6277101735386680762814942322444851025767571854389858533375",
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="-18446744073709551615",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211456"}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
			.add="340282366920938463481821351505477763072",
			.sub="340282366920938463444927863358058659840",
			.rsub="-340282366920938463444927863358058659840",
			.mul="6277101735386680763835789423207666416102355444464034512896",
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod=UINT64_MAX_P1,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211456"}, Param{VT::BIG, UINT64_MAX_P1}, ExpectedResults{
			.add="-340282366920938463444927863358058659840",
			.sub="-340282366920938463481821351505477763072",
			.rsub="340282366920938463481821351505477763072",
			.mul="-6277101735386680763835789423207666416102355444464034512896",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod="-340282366920938463444927863358058659840",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211456"}, Param{VT::BIG, "-18446744073709551616"}, ExpectedResults{
			.add="340282366920938463444927863358058659840",
			.sub="340282366920938463481821351505477763072",
			.rsub="-340282366920938463481821351505477763072",
			.mul="-6277101735386680763835789423207666416102355444464034512896",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod="340282366920938463444927863358058659840",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211456"}, Param{VT::BIG, "-18446744073709551616"}, ExpectedResults{
			.add="-340282366920938463481821351505477763072",
			.sub="-340282366920938463444927863358058659840",
			.rsub="340282366920938463444927863358058659840",
			.mul="6277101735386680763835789423207666416102355444464034512896",
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="-18446744073709551616",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463481821351505477763072"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="340282366920938463500268095579187314689",
			.sub="340282366920938463463374607431768211455",
			.rsub="-340282366920938463463374607431768211455",
			.mul="6277101735386680764516354157049543343047551403401280487424",
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod=UINT64_MAX_P2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463481821351505477763072"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="-340282366920938463463374607431768211455",
			.sub="-340282366920938463500268095579187314689",
			.rsub="340282366920938463500268095579187314689",
			.mul="-6277101735386680764516354157049543343047551403401280487424",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod="-340282366920938463463374607431768211455",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463481821351505477763072"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="340282366920938463463374607431768211455",
			.sub="340282366920938463500268095579187314689",
			.rsub="-340282366920938463500268095579187314689",
			.mul="-6277101735386680764516354157049543343047551403401280487424",
			.div="-18446744073709551616",
			.rdiv="-1",
			.mod="0",
			.rmod="340282366920938463463374607431768211455",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463481821351505477763072"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-340282366920938463500268095579187314689",
			.sub="-340282366920938463463374607431768211455",
			.rsub="340282366920938463463374607431768211455",
			.mul="6277101735386680764516354157049543343047551403401280487424",
			.div=UINT64_MAX_P1,
			.rdiv="0",
			.mod="0",
			.rmod="-18446744073709551617",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463500268095579187314689"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="340282366920938463518714839652896866306",
			.sub="340282366920938463481821351505477763072",
			.rsub="-340282366920938463481821351505477763072",
			.mul="6277101735386680764856636523970481806547819498980467802113",
			.div=UINT64_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod=UINT64_MAX_P2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463500268095579187314689"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="-340282366920938463481821351505477763072",
			.sub="-340282366920938463518714839652896866306",
			.rsub="340282366920938463518714839652896866306",
			.mul="-6277101735386680764856636523970481806547819498980467802113",
			.div="-18446744073709551617",
			.rdiv="-1",
			.mod="0",
			.rmod="-340282366920938463481821351505477763072",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463500268095579187314689"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="340282366920938463481821351505477763072",
			.sub="340282366920938463518714839652896866306",
			.rsub="-340282366920938463518714839652896866306",
			.mul="-6277101735386680764856636523970481806547819498980467802113",
			.div="-18446744073709551617",
			.rdiv="-1",
			.mod="0",
			.rmod="340282366920938463481821351505477763072",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463500268095579187314689"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-340282366920938463518714839652896866306",
			.sub="-340282366920938463481821351505477763072",
			.rsub="340282366920938463481821351505477763072",
			.mul="6277101735386680764856636523970481806547819498980467802113",
			.div=UINT64_MAX_P2,
			.rdiv="0",
			.mod="0",
			.rmod="-18446744073709551617",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211455"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="340282366920938463481821351505477763072",
			.sub="340282366920938463444927863358058659838",
			.rsub="-340282366920938463444927863358058659838",
			.mul="6277101735386680764176071790128604879547283307822093172735",
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod=UINT64_MAX_P2,
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211455"}, Param{VT::BIG, UINT64_MAX_P2}, ExpectedResults{
			.add="-340282366920938463444927863358058659838",
			.sub="-340282366920938463481821351505477763072",
			.rsub="340282366920938463481821351505477763072",
			.mul="-6277101735386680764176071790128604879547283307822093172735",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod="-340282366920938463444927863358058659838",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211455"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="340282366920938463444927863358058659838",
			.sub="340282366920938463481821351505477763072",
			.rsub="-340282366920938463481821351505477763072",
			.mul="-6277101735386680764176071790128604879547283307822093172735",
			.div="-18446744073709551615",
			.rdiv="-1",
			.mod="0",
			.rmod="340282366920938463444927863358058659838",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211455"}, Param{VT::BIG, "-18446744073709551617"}, ExpectedResults{
			.add="-340282366920938463481821351505477763072",
			.sub="-340282366920938463444927863358058659838",
			.rsub="340282366920938463444927863358058659838",
			.mul="6277101735386680764176071790128604879547283307822093172735",
			.div=UINT64_MAX_00,
			.rdiv="0",
			.mod="0",
			.rmod="-18446744073709551617",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211456"}, Param{VT::BIG, "340282366920938463463374607431768211455"}, ExpectedResults{
			.add="680564733841876926926749214863536422911",
			.sub="1",
			.rsub="-1",
			.mul="115792089237316195423570985008687907852929702298719625575994209400481361428480",
			.div="1",
			.rdiv="0",
			.mod="1",
			.rmod="340282366920938463463374607431768211455",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211456"}, Param{VT::BIG, "340282366920938463463374607431768211455"}, ExpectedResults{
			.add="-1",
			.sub="-680564733841876926926749214863536422911",
			.rsub="680564733841876926926749214863536422911",
			.mul="-115792089237316195423570985008687907852929702298719625575994209400481361428480",
			.div="-2",
			.rdiv="-1",
			.mod="340282366920938463463374607431768211454",
			.rmod="-1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "340282366920938463463374607431768211456"}, Param{VT::BIG, "-340282366920938463463374607431768211455"}, ExpectedResults{
			.add="1",
			.sub="680564733841876926926749214863536422911",
			.rsub="-680564733841876926926749214863536422911",
			.mul="-115792089237316195423570985008687907852929702298719625575994209400481361428480",
			.div="-2",
			.rdiv="-1",
			.mod="-340282366920938463463374607431768211454",
			.rmod="1",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-340282366920938463463374607431768211456"}, Param{VT::BIG, "-340282366920938463463374607431768211455"}, ExpectedResults{
			.add="-680564733841876926926749214863536422911",
			.sub="-1",
			.rsub="1",
			.mul="115792089237316195423570985008687907852929702298719625575994209400481361428480",
			.div="1",
			.rdiv="0",
			.mod="-1",
			.rmod="-340282366920938463463374607431768211455",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "382362535088167210234626361716426516060166448586892714986920000001800000000081"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.sub="382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.rsub="-382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.mul="236435670699204459093713935009106305322803931741435815699776293803790155888735974197346948347316579999967599999999271",
			.div="618354700061515834059999999999999999991",
			.rdiv="0",
			.mod="0",
			.rmod="618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-382362535088167210234626361716426516060166448586892714986920000001800000000081"}, Param{VT::BIG, "618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.sub="-382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.rsub="382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.mul="-236435670699204459093713935009106305322803931741435815699776293803790155888735974197346948347316579999967599999999271",
			.div="-618354700061515834059999999999999999991",
			.rdiv="-1",
			.mod="0",
			.rmod="-382362535088167210234626361716426516059548093886831199152860000002000000000090",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "382362535088167210234626361716426516060166448586892714986920000001800000000081"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.sub="382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.rsub="-382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.mul="-236435670699204459093713935009106305322803931741435815699776293803790155888735974197346948347316579999967599999999271",
			.div="-618354700061515834059999999999999999991",
			.rdiv="-1",
			.mod="0",
			.rmod="382362535088167210234626361716426516059548093886831199152860000002000000000090",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-382362535088167210234626361716426516060166448586892714986920000001800000000081"}, Param{VT::BIG, "-618354700061515834059999999799999999991"}, ExpectedResults{
			.add="-382362535088167210234626361716426516060784803286954230820980000001600000000072",
			.sub="-382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.rsub="382362535088167210234626361716426516059548093886831199152860000002000000000090",
			.mul="236435670699204459093713935009106305322803931741435815699776293803790155888735974197346948347316579999967599999999271",
			.div="618354700061515834059999999999999999991",
			.rdiv="0",
			.mod="0",
			.rmod="-618354700061515834059999999799999999991",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::BIG, "531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889"}, ExpectedResults{
			.add="85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.sub="85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.rsub="-85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.mul="45175124645189023184578764904745573466523720031595903892159066621104110513940600552455540874571215918778304649022541648601808557323245008512881159450610811088902177717844390449838411048294269360247162248387533712273",
			.div="160134",
			.rdiv="0",
			.mod="209823076619367981023771469794601869328016582143312911032147521717876940994108137259152513310889145399331",
			.rmod="531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::BIG, "531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889"}, ExpectedResults{
			.add="-85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.sub="-85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.rsub="85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.mul="-45175124645189023184578764904745573466523720031595903892159066621104110513940600552455540874571215918778304649022541648601808557323245008512881159450610811088902177717844390449838411048294269360247162248387533712273",
			.div="-160135",
			.rdiv="-1",
			.mod="321314916197399117665816736757866758001576535583719012167296616482526618866744105480009988954340140269558",
			.rmod="-85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::BIG, "-531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889"}, ExpectedResults{
			.add="85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.sub="85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.rsub="-85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.mul="-45175124645189023184578764904745573466523720031595903892159066621104110513940600552455540874571215918778304649022541648601808557323245008512881159450610811088902177717844390449838411048294269360247162248387533712273",
			.div="-160135",
			.rdiv="-1",
			.mod="-321314916197399117665816736757866758001576535583719012167296616482526618866744105480009988954340140269558",
			.rmod="85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::BIG, "-531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889"}, ExpectedResults{
			.add="-85053992302789618716638231227749358239293721923800400334454019218243341935258567999173046452755802549732939346",
			.sub="-85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.rsub="85052930026803985182440852051336253302039062737564946270607620329966941128138846294687568127751272091161601568",
			.mul="45175124645189023184578764904745573466523720031595903892159066621104110513940600552455540874571215918778304649022541648601808557323245008512881159450610811088902177717844390449838411048294269360247162248387533712273",
			.div="160134",
			.rdiv="0",
			.mod="-209823076619367981023771469794601869328016582143312911032147521717876940994108137259152513310889145399331",
			.rmod="-531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889",
	}});
	all_test_values.push_back(BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::S32, "9"}, ExpectedResults{
			.add="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.sub="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.rsub="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.mul="765481150483171217545855874755885251935997530976144059722777377966946273785288364322372765612281835884025434113",
			.div="9450384573866311327726615737726978418962932481186963700281202197122793503522078571881145254472615257827474495",
			.rdiv="0",
			.mod="2",
			.rmod="9",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::S32, "9"}, ExpectedResults{
			.add="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.sub="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.rsub="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.mul="-765481150483171217545855874755885251935997530976144059722777377966946273785288364322372765612281835884025434113",
			.div="-9450384573866311327726615737726978418962932481186963700281202197122793503522078571881145254472615257827474496",
			.rdiv="-1",
			.mod="7",
			.rmod="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::N32, "-9"}, ExpectedResults{
			.add="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.sub="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.rsub="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.mul="-765481150483171217545855874755885251935997530976144059722777377966946273785288364322372765612281835884025434113",
			.div="-9450384573866311327726615737726978418962932481186963700281202197122793503522078571881145254472615257827474496",
			.rdiv="-1",
			.mod="-7",
			.rmod="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
	}});
	all_test_values.push_back(
	BinOpTest{Param{VT::BIG, "-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457"}, Param{VT::N32, "-9"}, ExpectedResults{
			.add="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270466",
			.sub="-85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.rsub="85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270448",
			.mul="765481150483171217545855874755885251935997530976144059722777377966946273785288364322372765612281835884025434113",
			.div="9450384573866311327726615737726978418962932481186963700281202197122793503522078571881145254472615257827474495",
			.rdiv="0",
			.mod="-2",
			.rmod="-9",
	}});

	return all_test_values;
};

}

#endif // VALUES_FOR_TEST_H
