#ifndef UTILS_FOR_TEST_H
#define UTILS_FOR_TEST_H

#include "../src/bigint/bigInt.h"
#include "../src/bigint/utils.h"

#include <functional>
#include <ranges>
#include <string_view>
#include <vector>

#ifndef DO_PRINT_TEST_INFO
#  define DO_PRINT_TEST_INFO 0
#endif

#if DO_PRINT_TEST_INFO
#  include <iostream>
#endif

// General purpose tooling
namespace test_utils {

struct to_vector_adapter { /* just a marker */ };

/**
 * @brief creates a range adaptor that converts the range to a vector.
 * @return A range adaptor that converts the range to a vector.
 */
CONSTEXPR_AUTO
to_vector() {
	return to_vector_adapter{};
}

/**
 * @brief A range pipe that results in a vector.
 */
template<std::ranges::range R>
CONSTEXPR_AUTO
operator|(R&& r, [[maybe_unused]] const to_vector_adapter a) {
	std::vector<std::ranges::range_value_t<R>> v;

	// if we can get a size, reserve that much
	if constexpr (requires { std::ranges::size(r); }) {
		v.reserve(std::ranges::size(r));
	}

	v.insert(v.begin(), r.begin(), r.end());

	return v;
}

struct join_str_adapter {
	std::string separator;
};

/**
 * @brief creates a range adaptor that converts the range to a vector.
 * @return A range adaptor that converts the range to a vector.
 */
CONSTEXPR_AUTO
join_str(const std::string& separator) {
	return join_str_adapter{separator};
}

/**
 * @brief A range pipe that results in a string.
 */
template<std::ranges::range R>
	requires std::same_as<std::ranges::range_value_t<R>, std::string>
CONSTEXPR_AUTO
operator|(R&& r, [[maybe_unused]] const join_str_adapter& a) {
	auto iter = r.begin();

	if (iter == r.end()) {
		return std::string{};
	}

	std::string str = *iter;
	++iter;
	for (; iter != r.end(); ++iter) {
		str += a.separator;
		str += *iter;
	}

	return str;
}


namespace _private {

template <class T, T... Vs, class Op>
CONSTEXPR_VOID
consteval_for (std::integer_sequence<T, Vs...> const &, const Op& op) {
	using unused = int[]; // just a trick to provide an environment where to unpack the Vs... values
	(void)unused { 0, (op.template operator()<Vs>(), 0)... };
}

}

template <size_t N, class Op>
CONSTEXPR_VOID
consteval_for (const Op& op) {
	_private::consteval_for(std::make_index_sequence<N>{}, op);
}


/**
 * Returns the Nth type in the given template parameter pack.
 */
template<int N, typename... Ts>
using nth_type_of = std::tuple_element_t<N, std::tuple<Ts...>>;

/**
 * returns the first type in the given template parameter pack.
 */
template<typename... Ts>
using first_type_of = std::tuple_element_t<0, std::tuple<Ts...>>;


/**
 * @brief Extracts the Ith element from the tuple. I must be an integer value in [​0​, sizeof...(Ts)).
 * @param ts the parameter pack.
 */
template <int I, class... Ts>
const auto& get(const Ts&... ts) {
	return std::get<I>(std::tie(ts...));
}


}


// Test related tooling
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
str_to_int(const std::string_view str_value) {
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

	Value ()
		: type{ValType::BIG}, val{""} {}
};


template <size_t N>
struct OperationTest {
	std::array<Value, N> operands;
	std::string expected;
};


template<typename R, typename... On>
struct Operation_ { };

template<typename R, typename O1, typename... On>
struct Operation_<R, O1, On...> { using type = std::function<R(O1&, const On&...)>; };

template<typename R, typename O1>
struct Operation_<R, O1> { using type = std::function<R(O1&)>; };

template<typename R, typename... On>
using Operation = Operation_<R, On...>::type;


template <size_t N, class... On>
CONSTEXPR_AUTO
is_valid_test_for_op(const std::array<Value, N>& operands) -> bool {
	bool is_valid = true;
	consteval_for<N>(
		[&]<size_t I>() { is_valid &= is_assignable(param_type<nth_type_of<I, On...>>(), operands[I].type); }
	);
	return is_valid;
}

template <size_t N, class... On>
CONSTEXPR_AUTO
transform_to_tuple(const std::array<Value, N>& operands) -> std::tuple<On...> {
	std::tuple<On...> result{};
	consteval_for<N>(
		[&]<size_t I>() -> void { std::get<I>(result) = str_to_int<nth_type_of<I, On...>>(operands[I].val); }
	);
	return result;
}

template <size_t N>
void
print_test_info([[maybe_unused]]const OperationTest<N>& test) {
#if DO_PRINT_TEST_INFO
	std::cout << "retrieving values..." << std::endl;
	for (int i = 0; i < test.operands.size(); ++i) {
		std::cout << "  op[" << i << "] = " << test.operands[i].val << std::endl;
	}
	std::cout << "  res = " << test.expected << std::endl;
#endif
}

template <size_t N>
CONSTEXPR_AUTO
build_test_str(const std::array<Value, N> operands) -> std::string {
	return operands
		   | std::views::transform([](const auto& v) { return v.val; })
		   | join_str(" . ");
}

template <size_t N, class R, class... On>
void testOperationSingle(
	const Operation<R, On...> operation,
	const OperationTest<N>& test,
	//                      (actual result,                 expected result,             failure message (test_str), Operands
	const std::function<void(const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string&, const On&...)> test_func
) {
	if (!is_valid_test_for_op<N, On...>(test.operands)) {
		return; // skip
	}

	print_test_info(test);

	std::tuple<On...> operands = transform_to_tuple<N, On...>(test.operands);
	std::tuple<On&...> operand_refs = std::apply(std::tie<On...>, operands);

	const auto expected_result = str_to_int<std::remove_cvref_t<R>>(test.expected);

	const auto& actual_result = std::apply(operation, operand_refs);

	const auto test_str = build_test_str(test.operands);

	const auto test_tpl = std::tuple_cat(
		std::tie(actual_result, expected_result, test_str),
		operand_refs
	);

	std::apply(test_func, test_tpl);
}

template <size_t N, typename R, typename... On>
inline void testOperationBase(
	const Operation<R, On...> operation,
	const std::vector<OperationTest<N>>& tests,
	//                      (actual result,                 expected result,               test_str,               Operands
	const std::function<void(const std::remove_cvref_t<R>&, const std::remove_cvref_t<R>&, const std::string&, const On&...)> test_func
) {
	for (const auto &test : tests) {
		testOperationSingle<N, R, On...>(operation, test, test_func);
	}
}

}


#endif // UTILS_FOR_TEST_H
