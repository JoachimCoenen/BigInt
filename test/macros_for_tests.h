#pragma once

#include "../src/bigint/bigInt.h"

#include <gtest/gtest.h>


namespace {
using namespace bigint;
}

namespace test_utils {

template <size_t N, typename R, typename RT, typename... On>
inline void test_operation(
	const Operation<R, On...>& operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<OperationTest<N>>& tests
) {
	const std::function<void(const R&, const R&, const std::string&, const On&...)>
	test_func = [&](const R& result, const R& expected, const std::string& test_str, [[maybe_unused]] const On&... on) -> void {
		const auto& o1 = get<0>(on...);
		EXPECT_NE(reinterpret_cast<const void*>(&result), reinterpret_cast<const void*>(&o1)) << "return value & first operand expected to be different reference." << test_str;
		EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
	};

	testOperationBase<N, R, On...>(operation, tests, test_func);
}

template <size_t N, typename R, typename RT, typename... On>
	requires std::same_as<R, first_type_of<On...>>
			 && (!std::is_const_v<R>)
			 && (!std::is_const_v<first_type_of<On...>>)
inline void test_assignment_operation(
	const Operation<R&, On...>& operation,
	const std::function<RT(const R&)> get_result_compare_value,
	const std::vector<OperationTest<N>>& tests
) {
	const std::function<void(const R&, const R&, const std::string&, const On&...)>
		test_func = [&](const R& result, const R& expected, const std::string& test_str, [[maybe_unused]] const On&... on) -> void {
			const auto& o1 = get<0>(on...);
			EXPECT_EQ((&result), (&o1)) << "return value & first operand expected to be same reference." << test_str;
			EXPECT_EQ(get_result_compare_value(result), get_result_compare_value(expected)) << test_str;
	};

	testOperationBase<N, R&, On...>(operation, tests, test_func);
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
TEST_UNARY_OPERATOR(NAME, O1, BigInt, OP, TEST_VALUES, std::string, to_debug_string(res))



#define _TEST_BINARY_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, Test##NAME##_##O1##_##O2) {\
test_operation<2, R, RT, O1, O2>(\
[](const O1& a, const O2& b) -> R { OP; },\
[](const R& res) -> RT { return GET_RT; },\
TEST_VALUES\
);\
}


#define TEST_BINARY_OPERATOR(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
_TEST_BINARY_OPERATOR(NAME, O1, O2, R, return OP, TEST_VALUES, RT, GET_RT)

#define TEST_BINARY_OPERATOR_F(NAME, O1, O2, R, OP, TEST_VALUES, RT, GET_RT) \
_TEST_BINARY_OPERATOR(F##NAME, O1, O2, R, R res; OP; return res, TEST_VALUES, RT, GET_RT)

#define TEST_BINARY_OPERATOR_BIGINT(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR(NAME, O1, O2, BigInt, OP, TEST_VALUES, std::string, to_debug_string(res))

#define TEST_BINARY_OPERATOR_BIGINT_F(NAME, O1, O2, OP, TEST_VALUES) \
TEST_BINARY_OPERATOR_F(NAME, O1, O2, BigInt, OP, TEST_VALUES, std::string, to_debug_string(res))

#define _TEST_ASSIGN_OPERATOR(NAME, O1, O2, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, TestI##NAME##_##O1##_##O2) {\
test_assignment_operation<2, O1, RT, O1, O2>(\
[](O1& a, const O2& b) -> O1& { OP; },\
[](const O1& res) -> RT { return GET_RT; },\
TEST_VALUES\
);\
}

#define TEST_ASSIGN_OPERATOR_BIGINT(NAME, O2, OP, TEST_VALUES) \
_TEST_ASSIGN_OPERATOR(NAME, BigInt, O2, return OP, TEST_VALUES, std::string, to_debug_string(res))

#define TEST_ASSIGN_OPERATOR_BIGINT_F(NAME, O2, OP, TEST_VALUES) \
_TEST_ASSIGN_OPERATOR(F##NAME, BigInt, O2, OP; return a, TEST_VALUES, std::string, to_debug_string(res))



#define _TEST_TRINARY_OPERATOR(NAME, O1, O2, O3, R, OP, TEST_VALUES, RT, GET_RT) \
TEST(HelloTest, Test##NAME##_##O1##_##O2##_##O3) {\
test_operation<3, R, RT, O1, O2, O3>(\
[](const O1& a, const O2& b, const O3& c) -> R { OP; },\
[](const R& res) -> RT { return GET_RT; },\
TEST_VALUES\
);\
}

#define TEST_TRINARY_OPERATOR_BIGINT(NAME, O1, O2, O3, OP, TEST_VALUES) \
_TEST_TRINARY_OPERATOR(NAME, O1, O2, O3, BigInt, return OP,TEST_VALUES, std::string, to_debug_string(res))
