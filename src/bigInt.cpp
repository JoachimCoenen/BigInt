
#include "bigInt.h"
#include "utils.h"

#include <string>
#include <string_view>
#include <algorithm>
#include <math.h>
#include <iostream>

namespace bigint {
uint64_t toDigitSum(const BigInt &v) {

	// 4294967296 =>
	// 1000000000
	// 1111111110
	// 18446744073709551616 =>
	// 10000000000000000000
	static const auto basePower = 19;
	static const auto base = uint64_t(std::pow(10, basePower));
	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();

	uint64_t sum = 0ull;
	while (tempDig.d > 0) {
		tempDig = divmod(tempDig.d, base);
		auto digs = tempDig.r;
		while (digs > 00) {
			sum += digs % 10;
			digs /= 10;
		}
		tempDig.d.cleanup();
	}
	return sum;
}


BigInt factorial(uint32_t n) {
	BigInt result(1);
	for (uint32_t i = 1; i <= n; ++i) {
		result *= i;

	}
	return result;
}


template <int base = 10>
inline std::string to_string_padded_generic(uint64_t n, uint64_t len){
	std::string result(len, '0');
	for (uint64_t val = n; len-->0 && val != 0; val/=base)
	   result[len] = '0' + val%base;
	return result;
}


template <int base = 10>
std::string to_string_generic(const BigInt &v) {
	static const auto basePower = int(floor(64 / log2(base))); //  = 19;
	static const auto divisionBase = uint64_t(std::pow(base, basePower));

	DivModResult<BigInt, uint64_t> tempDig = { v, 0ull };
	tempDig.d.cleanup();

	std::string result;
	while (tempDig.d > 0) {
		tempDig = divmod(tempDig.d, divisionBase);
		auto digs = tempDig.r;
		result.insert(0, to_string_padded_generic<base>(digs, basePower));
		tempDig.d.cleanup();
	}
//       15511210043330985984000000
// 25! = 1551121  43330985984000000
	const auto index = result.find_first_of("123456789ABCDEF");
	result.erase(0, index);
	if (result.size() == 0) {
		result = "0";
	}
	return result;
}

std::string to_string_base2(const BigInt &v) {
	return to_string_generic<2>(v);
}

std::string to_string_base8(const BigInt &v) {
	return to_string_generic<8>(v);
}

std::string to_string_base10(const BigInt &v) {
	return to_string_generic<10>(v);
}

std::string to_string_base16(const BigInt &v) {
	return to_string_generic<16>(v);
}

std::string to_string(const BigInt &v) {
	return to_string_base10(v);
}


template <int base = 10>
BigInt from_string_generic(const std::string &input) {
	static const auto base_power = uint32_t(floor(64 / log2(base))); //  = 19;
	static const auto division_base = uint64_t(std::pow(base, base_power));

	BigInt result{0};

	for (size_t i = 0; i < input.size(); i += base_power) {
		auto substr = std::string_view(input).substr(i, base_power);
		auto mul = substr.size() == base_power ? division_base : uint64_t(std::pow(base, substr.size()));
		auto add = utils::stoull(substr, base);
		result *= mul;
		result += BigInt{add};
	}
//	result.cleanup();
	return result;
}

BigInt from_string_base2(const std::string &input) {
	return from_string_generic<2>(input);
}

BigInt from_string_base8(const std::string &input) {
	return from_string_generic<8>(input);
}

BigInt from_string_base10(const std::string &input) {
	return from_string_generic<10>(input);
}

BigInt from_string_base16(const std::string &input) {
	return from_string_generic<16>(input);
}

BigInt from_string(const std::string &input) {
	return from_string_base10(input);
}


}

