#ifndef UTILS_FOR_TEST_H
#define UTILS_FOR_TEST_H

#include "../src/bigint/bigInt.h"
#include "../src/bigint/utils.h"

#include <functional>
#include <string_view>

// required for EXPECT_EQ:
#include <gtest/gtest.h>

namespace test_utils {
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
		return bigint::utils::stoull(str_value);
	} else if constexpr (std::is_same_v<T, int64_t>) {
		return bigint::utils::stoll(str_value);
	} else if constexpr (std::is_same_v<T, uint32_t>) {
		return bigint::utils::stoul(str_value);
	} else if constexpr (std::is_same_v<T, int32_t>) {
		return bigint::utils::stol(str_value);
	} else if constexpr (std::is_same_v<T, bigint::BigInt>) {
		return bigint::BigInt(str_value);
	} else if constexpr (std::is_same_v<T, std::string>) {
		return std::string{str_value};
	} else if constexpr (std::is_same_v<T, bigint::DivModResult<typename T::_D, typename T::_R>>) {
		auto delimiter = str_value.find("|");
		auto d = str_value.substr(0, delimiter);
		auto r = str_value.substr(delimiter + 1);
		return bigint::DivModResult{str_to_int<typename T::_D>(d), str_to_int<typename T::_R>(r)};
	} else {
		static_assert(false, "unhandeled type provided");
	}
}


struct Value {
	ValType type;
	std::string val;
	Value (ValType type, std::string&& val)
		: type{type}, val{std::move(val)} {}
};

struct UnaOpTest {
	Value left;
	std::string expected;

	UnaOpTest(Value&& left, std::string&& expected)
		: left{std::move(left)}, expected{std::move(expected)} {}
};

template <class O1, class R>
void testUnaryOpSingle(
	const std::function<R(const O1&)> operation,
	const Value& left_param,
	const std::string expected_str,
	const std::function<void(const O1&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test
) {
	if (!(is_assignable( param_type<O1>(), left_param.type))) {
		return; // skip
	}

	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  lhs = " << left_param.val << std::endl;
	// std::cout << "  res = " << expected_str << std::endl;

	const O1 left = str_to_int<O1>(left_param.val);
	const auto expected = str_to_int<std::remove_cvref_t<R>>(expected_str);

	const auto& result = operation(left);
	const std::string test_str = std::string{left_param.val};
	test(left, result, expected, test_str);
}

template <class O1, class R>
inline void testUnaryOpBase(
	const std::function<R(const O1&)> operation,
	const std::vector<UnaOpTest>& tests,
	const std::function<void(const O1&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test_func
) {
	for (const auto &test : tests) {
		testUnaryOpSingle(
			operation,
			test.left,
			test.expected,
			test_func
		);
	}
}

template <class O1, class R, class RT>
inline void testUnaryOp(
	const std::function<R(const O1&)> operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<UnaOpTest>& tests
) {
	testUnaryOpBase<O1, R>(
		operation,
		tests,
		[&]([[maybe_unused]]const O1& l, const R& result, const R& expected, const std::string_view test_str) {
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
	);
}

struct BinOpTest {
	Value left;
	Value right;
	std::string expected;

	BinOpTest(Value&& left, Value right, std::string&& expected)
		: left{std::move(left)}, right{std::move(right)}, expected{std::move(expected)} {}
};

template <class O1, class O2, class R>
void testBinaryOpSingle1(
	const std::function<R(O1&, const O2&)> operation,
	const Value& left_param,
	const Value& right_param,
	const std::string expected_str,
	const std::function<void(const O1&, const O2&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test
) {
	if (!(is_assignable( param_type<O1>(), left_param.type) && is_assignable(param_type<O2>(), right_param.type))) {
		return; // skip
	}

	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  lhs = " << left_param.val << std::endl;
	// std::cout << "  rhs = " << right_param.val << std::endl;
	// std::cout << "  res = " << expected_str << std::endl;

	O1 left = str_to_int<O1>(left_param.val);
	const O2 right = str_to_int<O2>(right_param.val);
	const auto expected = str_to_int<std::remove_cvref_t<R>>(expected_str);

	const auto& result = operation(left, right);
	const std::string test_str = std::string{left_param.val} + " . " + std::string{right_param.val};
	test(left, right, result, expected, test_str);
}

template <class O1, class O2, class R>
inline void testBinaryOpBase(
	const std::function<R(O1&, const O2&)> operation,
	const std::vector<BinOpTest>& tests,
	const std::function<void(const O1&, const O2&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test_func
) {
	for (const auto &test : tests) {
		testBinaryOpSingle1(
			operation,
			test.left,
			test.right,
			test.expected,
			test_func
		);
	}
}

template <class O1, class O2, class R, class RT>
inline void testBinaryOp(
	const std::function<R(O1&, const O2&)> operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<BinOpTest>& tests
) {
	testBinaryOpBase<O1, O2, R>(
		operation,
		tests,
		[&]([[maybe_unused]]const O1& l, [[maybe_unused]]const O2& r, const R& result, const R& expected, const std::string_view test_str) {
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
	);
}


template <class O1, class O2, class RT>
inline void testIOp(
	const std::function<O1&(O1&, const O2&)> operation,
	const std::function<RT(const O1&)> get_result_compare_value,
	const std::vector<BinOpTest>& tests
) {
	testBinaryOpBase<O1, O2, O1&>(
		operation,
		tests,
		[&](const O1& left, [[maybe_unused]]const O2& r, const O1& result, const O1& expected, const std::string_view test_str) {
			EXPECT_EQ(static_cast<const void*>(&result), static_cast<const void*>(&left)) << "returned same reference? " << test_str;
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
	);
}

struct TriOpTest {
	Value o1;
	Value o2;
	Value o3;
	std::string expected;

	TriOpTest(Value&& o1, Value&& o2, Value&& o3, std::string&& expected)
		: o1{std::move(o1)}, o2{std::move(o2)}, o3{std::move(o3)}, expected{std::move(expected)} {}
};

template <class O1, class O2, class O3, class R>
void testTrinaryOpSingle(
	const std::function<R(const O1&, const O2&, const O3&)> operation,
	const TriOpTest& test_vals,
	const std::function<void(const O1&, const O2&, const O3&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test
	) {
	if (!(is_assignable(param_type<O1>(), test_vals.o1.type) && is_assignable(param_type<O2>(), test_vals.o2.type) && is_assignable(param_type<O3>(), test_vals.o3.type))) {
		return; // skip
	}

	// std::cout << "retrieving values..." << std::endl;
	// std::cout << "  o1 = " << test_vals.o1.val << std::endl;
	// std::cout << "  o2 = " << test_vals.o2.val << std::endl;
	// std::cout << "  o3 = " << test_vals.o3.val << std::endl;
	// std::cout << "  res = " << test_vals.expected << std::endl;

	const O1 o1 = str_to_int<O1>(test_vals.o1.val);
	const O2 o2 = str_to_int<O2>(test_vals.o2.val);
	const O3 o3 = str_to_int<O3>(test_vals.o3.val);
	const auto expected = str_to_int<std::remove_cvref_t<R>>(test_vals.expected);

	const auto& result = operation(o1, o2, o3);
	const std::string test_str = std::string{test_vals.o1.val} + " . " + std::string{test_vals.o2.val} + " . " + std::string{test_vals.o3.val};
	test(o1, o2, o3, result, expected, test_str);
}

template <class O1, class O2, class O3, class R>
inline void testTrinaryOpBase(
	const std::function<R(const O1&, const O2&, const O3&)> operation,
	const std::vector<TriOpTest>& tests,
	const std::function<void(const O1&, const O2&, const O3&, const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string_view)> test_func
	) {
	for (const auto &test : tests) {
		testTrinaryOpSingle(
			operation,
			test,
			test_func
			);
	}
}

template <class O1, class O2, class O3, class R, class RT>
inline void testTrinaryOp(
	const std::function<R(const O1&, const O2&, const O3&)> operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<TriOpTest>& tests
	) {
	testTrinaryOpBase<O1, O2, O3, R>(
		operation,
		tests,
		[&]([[maybe_unused]]const O1&, [[maybe_unused]]const O2&, [[maybe_unused]]const O3& l, const R& result, const R& expected, const std::string_view test_str) {
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
		}
		);
}


}


#endif // UTILS_FOR_TEST_H
