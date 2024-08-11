#include <iostream>
#include "bigint/bigInt.h"

using namespace bigint;

int main()
{
	std::cout << "Factorial" << std::endl;
	std::cout << "=========" << std::endl;
	std::cout << std::endl;
	std::cout << "please enter a number: ";
	uint32_t base;
	std::cin >> base;
	std::cout << "calculating factorial...." << std::endl;
	std::cout << std::endl;
	std::cout << "  " << base << "! = " << (factorial(base)) << std::endl;
	std::cout << std::endl;
	return 0;
}
