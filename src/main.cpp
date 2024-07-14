#include <iostream>
#include "bigInt.h"

using namespace bigint;

int main()
{

	std::cout << "Factorial" << std::endl;
	std::cout << "=========" << std::endl;
	std::cout << std::endl;
	std::cout << "please enter a number: ";
	uint64_t base;
	std::cin >> base;
	std::cout << "calculating factorial...." << std::endl;
	std::cout << std::endl;
	std::cout << "  " << 1000 << "! = " << (factorial(base)) << std::endl;
	std::cout << std::endl;
//	for (auto i = 4; i <= 100; i += 1) {
//		std::cout << "BigInt:   " << i << "! - 7 = " << (factorial(i) - BigInt{7}) << std::endl;
//	}
	return 0;
}
