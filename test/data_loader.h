#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <array>
#include <iostream>
#include <fstream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "../src/bigint/utils.h"
#include "utils_for_test.h"


namespace test_utils {


[[nodiscard]] inline auto
load_lines(const std::string& path) {
	std::vector<std::string> result;
	std::ifstream rfile;
	rfile.open(path);
	if (rfile.is_open()) {

		while (std::getline(rfile, result.emplace_back())) {
		}
		result.pop_back();
		rfile.close();
	} else {
		throw std::runtime_error(std::string("Cannot open file '") + path + "'.");
	}
	return result;
}


template<size_t N>
[[nodiscard]] inline auto
split_line(const std::string& line, const std::string_view delimiter) -> std::array<std::string, N> {
	std::array<std::string, N> result;

	size_t i = 0;
	size_t lpos = 0;
	size_t pos = 0;
	while ((pos = line.find(delimiter, lpos)) != std::string::npos) {
		result.at(i) = line.substr(lpos, pos - lpos);
		++i;
		lpos = pos + 1;
	}
	result.at(i) = line.substr(lpos);
	++i;

	if (i < N) {
		throw std::length_error(std::string("Not enough substrings. Expected ") + std::to_string(N) + " but got " + std::to_string(i) + ".");
	}

	return result;
}


[[nodiscard]] inline auto
to_val_type(const std::string& line) -> ValType {
	if (line == "U32") {
		return ValType::U32;
	}
	if (line == "S32") {
		return ValType::S32;
	}
	if (line == "N32") {
		return ValType::N32;
	}
	if (line == "U64") {
		return ValType::U64;
	}
	if (line == "S64") {
		return ValType::S64;
	}
	if (line == "N64") {
		return ValType::N64;
	}
	if (line == "BIG") {
		return ValType::BIG;
	}
	throw std::runtime_error(std::string("Unknown ValType '") + line + "'.");
}


[[nodiscard]] inline auto
to_value(const std::string& line) -> Value {
	auto split = split_line<2>(line, ",");
	auto vt = to_val_type(split[0]);
	auto& val = split[1];
	return Value{vt, std::move(val)};
}


template <size_t N>
[[nodiscard]] inline auto
to_op_test(const std::string& line) -> OperationTest<N>{
	auto split = split_line<N+1>(line, ";");
	std::array<Value, N> operands{};
	for (size_t i = 0; i < N; ++i) {
		operands[i] = to_value(split[i]);
	}
	auto& result = split[N];
	return OperationTest<N>{std::move(operands), std::move(result)};
}

template<typename R>
[[nodiscard]] auto
load_testdata(const std::string& path, const std::function<R(const std::string&)> parse_test) -> std::vector<R> {
	auto lines = load_lines(path);
	return lines
		   | std::views::transform(parse_test)
		   | bigint::utils::to_vector();
}

template<size_t N>
[[nodiscard]] auto
load_testdata(const std::string& path) -> std::vector<OperationTest<N>> {
	return load_testdata<OperationTest<N>>(path, to_op_test<N>);
}



}


#endif // DATA_LOADER_H
