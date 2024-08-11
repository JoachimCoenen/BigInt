#ifndef UTILS_FOR_TEST_H
#define UTILS_FOR_TEST_H

#include "../src/bigint/bigInt.h"
#include "../src/bigint/utils.h"

#include <functional>
#include <string_view>
#include <variant>
// required for EXPECT_EQ:
#include <gtest/gtest.h>

namespace test_utils {

struct IgnoreTest {};
constexpr auto IGNORED = IgnoreTest{};


using ExpectedResult = std::variant<IgnoreTest, std::string>; // todo add exception option

struct ExpectedResults {
	ExpectedResult lshift;
	ExpectedResult rlshift;
	ExpectedResult rshift;
	ExpectedResult rrshift;
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

CONSTEXPR_AUTO
is_assignable(const ParamType param, const ValType value) {
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
template <typename T>
CONSTEVAL_AUTO
param_type() {
	if constexpr (std::is_same_v<uint64_t, T>) {
		return PT::U64;
	} else if constexpr (std::is_same_v<int64_t, T>) {
		return PT::S64;
	} else if constexpr (std::is_same_v<uint32_t, T>) {
		return PT::U32;
	} else if constexpr (std::is_same_v<int32_t, T>) {
		return PT::S32;
	} else if constexpr (std::is_same_v<bigint::BigInt, T>) {
		return PT::BIG;
	} else {
		static_assert(false, "unhandeled type provided");
	}
}

template <typename T>
CONSTEXPR_AUTO
str_to_int(const std::string_view str_value) -> T {
	if constexpr (std::is_same_v<T, uint64_t>) {
		return utils::stoull(str_value);
	} else if constexpr (std::is_same_v<T, int64_t>) {
		return utils::stoll(str_value);
	} else if constexpr (std::is_same_v<T, uint32_t>) {
		return utils::stoul(str_value);
	} else if constexpr (std::is_same_v<T, int32_t>) {
		return utils::stol(str_value);
	} else if constexpr (std::is_same_v<T, bigint::BigInt>) {
		return bigint::BigInt(str_value);
	} else if constexpr (std::is_same_v<T, bigint::DivModResult<typename T::_D, typename T::_R>>) {
		auto delimiter = str_value.find("|");
		auto d = str_value.substr(0, delimiter);
		auto r = str_value.substr(delimiter + 1);
		return bigint::DivModResult{str_to_int<typename T::_D>(d), str_to_int<typename T::_R>(r)};
	} else {
		static_assert(false, "unhandeled type provided");
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


using ResultSelector = std::function<ExpectedResult(const ExpectedResults&)>;


bool isAcceptableTest(
	ValType left_arg_type,
	ValType right_arg_type,
	ParamType required_left_type,
	ParamType required_right_type,
	const ExpectedResult &expected_variant
) {
	return (is_assignable(required_left_type, left_arg_type)) &&
		   (is_assignable(required_right_type, right_arg_type)) &&
		   !std::holds_alternative<IgnoreTest>(expected_variant);
}


template <class O1, class O2, class R>
void testBinaryOpSingle1(
	const std::function<R(O1&, const O2&)> operation,
	const Param& left_param,
	const Param& right_param,
	const ExpectedResult expected_variant,
	const std::function<void(const O1&, const O2&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test
) {
	if (!isAcceptableTest(left_param.type, right_param.type, param_type<O1>(), param_type<O2>(), expected_variant)) {
		return; // skip
	}

	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  lhs = " << left_param.val << std::endl;
	// std::cout << "  rhs = " << right_param.val << std::endl;
	// std::cout << "  res = " << std::get<std::string_view>(get_result_str(test.expected)) << std::endl;

	O1 left = str_to_int<O1>(left_param.val);
	const O2 right = str_to_int<O2>(right_param.val);
	const auto expected = str_to_int<std::remove_cvref_t<R>>(std::get<std::string>(expected_variant));

	const auto& result = operation(left, right);
	const std::string test_str = std::string{left_param.val} + " . " + std::string{right_param.val};
	test(left, right, result, expected, test_str);
}

template <class O1, class O2, class R>
inline void testBinaryOpBase(
	const std::function<R(O1&, const O2&)> operation,
	const ResultSelector result_selector,
	const ResultSelector rresult_selector, // reverse result_selector. select the result for e.g. right - left.
	const std::vector<BinOpTest> &tests,
	const std::function<void(const O1&, const O2&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test_func
) {
	for (const auto &test : tests) {
		testBinaryOpSingle1(
			operation,
			test.left, test.right,
			result_selector(test.expected),
			test_func
		);
	}
	for (const auto &test : tests) {
		testBinaryOpSingle1(
			operation,
			test.right, test.left,
			rresult_selector(test.expected),
			test_func
		);
	}
}

template <class O1, class O2, class R, class RT>
inline void testBinaryOp(
	const std::function<R(O1&, const O2&)> operation,
	const ResultSelector result_selector,
	const ResultSelector rresult_selector, // reverse result_selector. select the result for e.g. right - left.
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<BinOpTest> &tests
) {
	testBinaryOpBase<O1, O2, R>(
		operation,
		result_selector,
		rresult_selector,
		tests,
		[&]([[maybe_unused]]const O1& l, [[maybe_unused]]const O2& r, const R& result, const R& expected, const std::string_view test_str) {
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
	);
}


template <class O1, class O2, class RT>
inline void testIOp(
	const std::function<O1&(O1&, const O2&)> operation,
	const ResultSelector result_selector,
	const ResultSelector rresult_selector, // reverse result_selector. select the result for e.g. right - left.
	const std::function<RT(const O1&)> get_result_compare_value,
	const std::vector<BinOpTest> &tests
) {
	testBinaryOpBase<O1, O2, O1&>(
		operation,
		result_selector,
		rresult_selector,
		tests,
		[&](const O1& left, [[maybe_unused]]const O2& r, const O1& result, const O1& expected, const std::string_view test_str) {
			EXPECT_EQ(static_cast<const void*>(&result), static_cast<const void*>(&left)) << "returned same reference? " << test_str;
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
	);
}

}


#endif // UTILS_FOR_TEST_H
