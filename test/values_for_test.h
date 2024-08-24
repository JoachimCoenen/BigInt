#ifndef VALUES_FOR_TEST_H
#define VALUES_FOR_TEST_H

#include <vector>

#include "utils_for_test.h"
#include "data_loader.h"

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

inline const std::string test_data_dir = "../../test/test_data/";

[[nodiscard]] inline std::vector<BinOpTest>
get_all_lshift_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_lshift_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_rshift_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_rshift_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_add_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_add_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_sub_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_sub_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_mul_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_mul_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_div_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_div_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_mod_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_mod_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_divmod_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_divmod_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<UnaOpTest>
get_all_sqrt_test_values() {
	return load_testdata<UnaOpTest>(test_data_dir + "values_for_sqrt_test.csv", [](const auto& x) { return to_una_op_test(x); });
}

[[nodiscard]] inline std::vector<UnaOpTest>
get_all_log2_test_values() {
	return load_testdata<UnaOpTest>(test_data_dir + "values_for_log2_test.csv", [](const auto& x) { return to_una_op_test(x); });
}

[[nodiscard]] inline std::vector<BinOpTest>
get_all_pow_test_values() {
	return load_testdata<BinOpTest>(test_data_dir + "values_for_pow_test.csv", [](const auto& x) { return to_bin_op_test(x); });
}

[[nodiscard]] inline std::vector<TriOpTest>
get_all_powmod_test_values() {
	return load_testdata<TriOpTest>(test_data_dir + "values_for_powmod_test.csv", [](const auto& x) { return to_tri_op_test(x); });
}

}

#endif // VALUES_FOR_TEST_H
