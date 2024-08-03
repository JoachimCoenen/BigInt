#ifndef UTILS_FOR_TEST_H
#define UTILS_FOR_TEST_H

#include <functional>
#include <string_view>
#include <variant>
// required for EXPECT_EQ:
#include <gtest/gtest.h>

namespace test_utils {

struct IgnoreTest {};
constexpr auto IGNORED = IgnoreTest{};


using ExpectedResult = std::variant<IgnoreTest, std::string_view>; // todo add exception option

struct ExpectedResults {
	ExpectedResult add;
	ExpectedResult sub;
	ExpectedResult rsub;
	ExpectedResult mul;
	ExpectedResult div;
	ExpectedResult rdiv;
	ExpectedResult mod;
	ExpectedResult rmod;
	// ExpectedResult pow;
	// ExpectedResult rpow;
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




template <class T>
struct ParamSelector {
	ParamSelector(std::function<T(std::string_view)> convert, ParamType type)
		: convert{convert}, type{type} {}

	std::function<T(std::string_view)> convert;
	ParamType type;
};

template <class T>
struct ResultSelector {
	const std::function<ExpectedResult(ExpectedResults)> get;
	const std::function<T(std::string_view)> convert;
};


bool isAcceptableTest(
	const Param &left_param,
	const Param &right_param,
	const ParamType &required_left_type,
	const ParamType &required_right_type,
	const ExpectedResult &expected_variant
) {
	return (is_assignable(required_left_type, left_param.type)) &&
		   (is_assignable(required_right_type, right_param.type)) &&
		   !std::holds_alternative<IgnoreTest>(expected_variant);
}


template <class O1, class O2, class R, class RT>
inline void testBinaryOpSingle(
	const std::function<R(O1, O2)>& operation,
	const Param &left_param,
	const Param &right_param,
	const ParamSelector<O1> &left_selector,
	const ParamSelector<O2> &right_selector,
	const ResultSelector<R> &result_selector,
	const std::function<RT(R)>& get_result_compare_value,
	const BinOpTest &test
) {
	const auto expected_variant = result_selector.get(test.expected);

	if (!isAcceptableTest(left_param, right_param, left_selector.type, right_selector.type, expected_variant)) {
		return; // skip
	}
	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  lhs = " << left_param.val << std::endl;
	// std::cout << "  rhs = " << right_param.val << std::endl;
	// std::cout << "  res = " << std::get<std::string_view>(result_selector.get(test.expected)) << std::endl;

	const auto left = left_selector.convert(left_param.val);
	const auto right = right_selector.convert(right_param.val);
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
	const ResultSelector<R> &rresult_selector, // reverse result_selector. select the result for e.g. right - left.
	const std::function<RT(R)>& get_result_compare_value,
	const std::vector<BinOpTest> &tests
) {
	for (const auto &test : tests) {
		testBinaryOpSingle(
			operation,
			test.left, test.right,
			left_selector, right_selector, result_selector,
			get_result_compare_value,
			test
		);
	}
	for (const auto &test : tests) {
		testBinaryOpSingle(
			operation,
			test.right, test.left,
			left_selector, right_selector, rresult_selector,
			get_result_compare_value,
			test
		);
	}
}


}


#endif // UTILS_FOR_TEST_H
