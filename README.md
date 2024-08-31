# BigInt

Integers without a size limit.
```c++
std::cout << "3^100 = "<< pow(BigInt{3}, 100) << std::endl;
// Outputs: 3^100 = 515377520732011331036461129765621272702107522001
```

If you experience problems, found a bug, or have suggestions, Feel free to create a new issue or make a pull requset.


## Contents

* [Contents](#Contents)
* [Overview](#Overview)
* [Installing](#Installing)
* [Examples](#Examples)
* [Contributing](#Contributing)
* [License](#License)


## Overview
BigInt is a header only library for working with integers values bigger than the hardware limit.  
BigInt has no additional dependencies and is as simple as possible to use without unneccesary performance compromises.  
It is fully `constexpr'd` and therfore 

## Requirements
BigInt requires **C++20** or higher and a compatible GCC (or MinGW), Clang, or MSVC compiler.


## Installing
Installation is very simple, just download and copy the [bigint folder](src/bigint) to your project.  
Then, you can simply include in other files:
```c++
#include "bigint/bigInt.h"
```
or
```c++
#include "<some_folder>/bigint/bigInt.h"
```
depending where you put the folder.  


## Overview
For simplicity reasons, this overview will use `using bigint;`. If you prefer not to do so, you can access the BigInt class as `bigint::BigInt`.  

### Instantiation
It is possible to instantiate a BigInt in multiple ways:  
```c++
BigInt A;  // will hold the value +0.
BigInt B{100};  // B will hold the value +100.
BigInt C{-50};  // !! Does not work currently !!
BigInt D{50, Sign::NEG};  // C will hold the value -50.
BigInt E{"-4561273837128312881"};  // D will hold the value -4561273837128312881.
```

But *not* like this:  
```c++
BigInt F{-50};  // !! Does not work currently !!
BigInt G = "4561273837";  // BigInt(std::string_view) is an explicit contructor
BigInt H = 2;  // BigInt(uint64_t) is also explicit
```

### Operators and Basic Math Functions 
Following operators are provided: 
* Basic arithmetic operators: `+`, `-`, `*`, `/`, `%`.
* Bitwise shift operators: `<<`, `>>`.
* Inline operators: `+=`, `-=`, `*=`, `/=`.
	* `+=`, `-=`, `<<=`, `>>=` are always performed inline.
	* `*=` is only performed inline if the second argument is an integral type (e.g. int32_t, or uint64_t).
	* `/=` is never performed inline.
* Negation, absolute value `-`, `abs(const BigInt&)`: Both operations return a view of the underlying BigInt with the sign changed.
* `mult(uint64_t, uint64_t)`: multilpies both numbers and returns an entirely stack-allocated BigInt-like object.
* `divmod(...)`, `divmod1(...)`: to get both the quotient and the reminder with only one calculation.
* `sqrt(const BigInt& x)`: calculates floor(sqrt(x)).
* `log2(const BigInt& x)`: calculates floor(ln(x) / ln(2)). This is the same as counting the number of digits of a positive, non-zero integer in binary representation.
* `log10(const BigInt& x)`: calculates floor(ln(x) / ln(10)). This is the same as counting the number of digits of a positive, non-zero integer in decimal representation.
* `log(const BigInt& base, const BigInt& y)`: calculates floor(ln(y) / ln(base)).
* `pow(const BigInt& base, uin64_t exp)`: calculates base<sup>exp</sup>.
* `pow_mod(const BigInt& base, const BigInt& exp, const BigInt& mod)`: calculates base<sup>exp</sup> % mod. This is <i>way</i> faster than doing `pow(base, exp) % mod`; especially for large `x` and `y`.
* `factorial(uin32_t x)`: calculates `x! = 1 * 2 * 3 * ... * x`.
* `digit_sum(const BigInt& x)`: sums all digits in base 10. E.g.: `digit_sum(12955) == 1 + 2 + 9 + 5 + 5`.
* `perm(uint32_t n, uint32_t k)`: Calculates the number of ways to choose `k` items from `n` items without repetition and with order. Evaluates to `n! / (n - k)!` when `k <= n` and evaluates to zero otherwise.
* `comb(uint32_t n, uint32_t k)`: Calculates the number of ways to choose `k` items from `n` items without repetition and without order. Evaluates to `n! / ((n - k)! * k!)` when `k <= n` and evaluates to zero otherwise.

All arithmetic operators can be used with mixed BigInt and integral types.
```c++
BigInt a{"1390824942691875931654"};
std::cout << (a * 78) << std::endl;
// Output: 108484345529966322669012
```

### Additional Functions
* A BigInt instance is printable, overloading `<<` for `std::ostream`.
* Use `to_string(const BigInt&)` or `to_string_base10(const BigInt&)` to convert BigInt into a decimal std::string.
* Use `to_string_base2(const BigInt&)`, `to_string_base8(const BigInt&)`, `to_string_base8(const BigInt&)`, `to_string_base16(const BigInt&)` to convert BigInt into a base2, base8, or base16 std::string.
* Use `from_string(std::string_view)` or `from_string_base10(std::string_view)` to convert a decimal string to a BigInt.
* Use `from_string_base2(std::string_view)`, `from_string_base8(std::string_view)`, `from_string_base8(std::string_view)`, `from_string_base16(std::string_view)` to convert a base2, base8, or base16 tring into a BigInt.

* Use `is_neg(const BigInt&)`, `is_zero(const BigInt&)`, `is_pos(const BigInt&)` to ccheck whether a BigInt is smaller than, equal to, or greater than zero respectively.
* Use `.size()` to get the number of digits in base 2<sup>64</sup>.


### Exceptions
A few methods can throw exceptions:
* `std::domain_error` will be thrown by some functions when inputs are outside of the domain on which an operation is defined.  
  E.g. division by zero or sqare root of a negative number.
  * `/`, `%`, `/=`, `divmod(a, b)`, etc.: when the divisor is zero.
  * `sqrt(x)`: when `x` is negative.
  * `log2(x)`: when `x` is zero or negative.
  * `pow(base, exp)`: when `base` and `exp` are both zero. (`exp` is an unsigned integer, so negative values cannot be passed.) 
  * `pow_mod(base, exp, mod)`: when `base` and `exp` are both zero or when `exp` is negative or when `mod` is zero.

* `std::invalid_argument` will be thrown by some functions when some (non - mathematical) assumptions are not fulfilled.
  * `BigInt(std::string_view)`, `from_string(std::string_view)`, `from_string_baseXY(std::string_view)`: when a non-number string is provided.
  * `BigInt.set(index, digit)`: when `index` is greater than `BigInt.size() - 1`.

### Internals

BigInt internally represents numbers in base 2<sup>64</sup> and uses only a fraction of the memory of a typical base 10 approach. That makes it (potentially) very fast.

## Examples

### Factorial
```c++
[[nodiscard]] constexpr auto
factorial(uint32_t n) -> BigInt {
	BigInt result{1};
	for (uint64_t i = 1; i <= n; ++i) {
		result *= i;
	}
	return result;
}

std::cout << "100! = " << factorial(100) << std::endl;
// Outputs 100! = 93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864000000000000000000000000
```


### Digit Sum

```c++
[[nodiscard]] constexpr auto
digit_sum(const BigInt &v) -> uint64_t {
	constexpr auto base = 19; // 19 is the larges value for n such that 10^n fits into 64 bits
	DivModResult<BigInt, uint64_t> tempDig{ abs(v), 0ull };
	
	uint64_t sum = 0ull;
	while (tempDig.d > 0) {
		tempDig = divmod(tempDig.d, base);
		while (tempDig.r > 0) {
			sum += tempDig.r % 10;
			tempDig.r /= 10;
		}
	}
	return sum;
}

std::cout << "digit_sum(100!) = " << digit_sum(factorial(100)) << std::endl;
// Outputs digit_sum(100!) = 648
```

## Contributing
Contributions are welcome, fork this repo, change it, open a pull request or an issue.  
Make sure to add test for new functionality and that no tests are failing.  

## License
All code is licensed under [MIT](LICENSE).  
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.*

