# BigInt

Integers without a size limit.
```c++
std::cout << -1 + pow(3_big, 300) << std::endl;
// Outputs: 136891479058588375991326027382088315966463695625337436471480190078368997177499076593800206155688941388250484440597994042813512732765695774566000

std::cout << (1390824942691875931654000_big * 78) << std::endl;
// Output: 108484345529966322669012000
```

If you experience problems, found a bug, or have suggestions, Feel free to create a new issue or make a pull request.


## Contents

<!-- TOC -->
* [Requirements](#requirements)
* [Installing](#installing)
* [Overview](#overview)
  * [Features](#features)
  * [Instantiation](#instantiation)
  * [Basic Operators](#basic-operators)
  * [Basic Math Functions](#basic-math-functions)
  * [Additional Functions](#additional-functions)
  * [Exceptions](#exceptions)
  * [Internals](#internals)
* [Examples](#examples)
  * [Factorial](#factorial-1)
  * [Digit Sum](#digit-sum)
* [Contributing](#contributing)
* [License](#license)
<!-- TOC -->

## Summary
BigInt is a header only library for working with integers values bigger than the hardware limit.  
BigInt has no additional dependencies and is designed to be easy to use without unnecessary performance compromises.  
The numbers a represented in base 18446744073709551616 = 2<sup>64</sup> to maximize memory efficiency.  
It is fully `constexpr'd` and therefore partial result and constants can be calculated at compile time.  


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
depending on where you put the folder.  


## Overview
For simplicity reasons, this overview will use `using bigint;`. If you prefer not to do so, you can access the BigInt class as `bigint::BigInt`.  

### Features

* numbers a represented in base 18446744073709551616 = 2<sup>64</sup> to maximize memory efficiency (64 bit per digit).
* BigInt provides support for literals using `_big`:  
  ```c++
  auto a = 100_big;
  auto b = 123'456'789_big;  // with digit separators
  auto c = -0x5abcDEF1_big;  // hexadecimal literal
  auto d = 0b10101010110_big;  // binaryliteral
  auto e = 0777777_big;  // octal literal
  ```
* BigInt is fully `constexpr'd` and therefore partial results and constants can be calculated at compile time:  
  ```c++
  constexpr auto a = -1 + pow(3_big, 300) * 5;
  constexpr auto b = gdc(12368464545159878212501232471351513542431_big, 0x1385a5347761f71414247342dda76655535_big);
  constexpr auto c = 12368464545159878212501232471351513542431_big / 0; // compile time error division by zero
  ```
* Conversion from / to different bases:  
  ```c++
  constexpr auto a = from_string_base16("-0xabc057");
  constexpr std::string b = to_string<17>(-0xabc057_big)
  constexpr auto c = from_string<7>("123456777") // compile time error because '7' is not a valid digit in base 7
  ```
* EasyConversion from / to integral types:  
  ```c++
  fits_u32(-1234_big); // false
  fits<int16_t>(-1234_big); // true
  
  int64_t a = as_i64(-1234_big);
  uint16_t b = as_integral<uint16_t>(-1234_big);
  BigInt c = BigInt{-99LL);
  ```
* many math functions:  
  ```c++
  pow_mod(123456789_big, 987654321_big, 5555555555_big); // efficient modular exponentiation
  log(factorial(100), pow(3, 100));
  lcm(factorial(100), pow(3, 100)); // least commin multiple
  divmod(factorial(100), pow(3, 50)); // combined division and modulo
  digit_sum<13>(factorial(100)); // digit sum in base 13
  ...
  ```

### Instantiation
It is possible to instantiate a BigInt in multiple ways:  
```c++
BigInt A;  // will hold the value +0.
BigInt B1 = 100_big;  // BigInt literal
BigInt B2 = 123'456'789_big;  // BigInt literal with digit separators
BigInt B3 = 0x5abcDEF1_big;  // hexadecimal BigInt literal
BigInt B4 = 0b10101010110_big;  // binary BigInt literal
BigInt B5 = 0777777_big;  // octal BigInt literal
BigInt C{100};  // B will hold the value +100.
BigInt D{-50};  // C will hold the value -50.
BigInt E{50, Sign::NEG};  // D will hold the value -50.
constexpr BigInt F = 13 - 4561273837128312881999_big;  // E will hold the value -4561273837128312881986.
```

But *not* like this:  
```c++
BigInt G = 2;  // BigInt(uint64_t) is also explicit. prevents accidental use of expensive operations.
```

### Basic Operators

#### addition
Sums two integers. The addition assignment operations are performed in-place. 
```c++
constexpr auto
operator+(const BigInt& a, const BigInt& b) -> BigInt;
constexpr auto
operator+(const BigInt& a, std::integral auto b) -> BigInt;
constexpr auto
operator+(std::integral auto a, const BigInt& b) -> BigInt;

constexpr auto
operator+=(BigInt& a, const BigInt& b) -> BigInt&;
constexpr auto
operator+=(BigInt& a, std::integral auto b) -> BigInt&;
```

#### subtraction
Subtracts `b` from `a`. The subtraction assignment operations are performed in-place.
```c++
constexpr auto
operator-(const BigInt& a, const BigInt& b) -> BigInt;
constexpr auto
operator-(const BigInt& a, std::integral auto b) -> BigInt;
constexpr auto
operator-(std::integral auto a, const BigInt& b) -> BigInt;

constexpr auto
operator-=(BigInt& a, const BigInt& b) -> BigInt&;
constexpr auto
operator-=(BigInt& a, std::integral auto b) -> BigInt&;
```

#### multiplication
Multiplies two integers. The multiplication assignment operation is only performed in-place if the second multiplicand is an integral type (like `int` or `uint64_t`).
```c++
constexpr auto
operator*(const BigInt& a, const BigInt& b) -> BigInt;
constexpr auto
operator*(const BigInt& a, std::integral auto b) -> BigInt;
constexpr auto
operator*(std::integral auto a, const BigInt& b) -> BigInt;

constexpr auto
operator*=(BigInt& a, const BigInt& b) -> BigInt&;
constexpr auto
operator*=(BigInt& a, std::integral auto b) -> BigInt&;

constexpr auto
mult(uint64_t a, uint64_t b) -> /* Special stack allocated BigInt-like type */
```

#### division
Divides `a` by `b`. The division assignment operation is only performed in-place if the divisor is a 32-bit integer.  
Dividing by a 32-bit integer is considerably faster than dividing ba a 64-bit one: `myBigInt / 7` is noticeably faster than `myBigInt / 7ull`.  
```c++
constexpr auto
operator/(const BigInt& a, const BigInt& b) -> BigInt;
constexpr auto
operator/(const BigInt& a, std::integral auto b) -> BigInt;

constexpr auto
operator/=(BigInt& a, const BigInt& b) -> BigInt&;
constexpr auto
operator/=(BigInt& a, std::integral auto b) -> BigInt&;
```

#### modulo
calculates the reminder of dividing `a` by `b`. The modulo assignment operation is *never* performed in-place.  
Dividing by a 32-bit integer is considerably faster than dividing ba a 64-bit one: `myBigInt % 17` is noticeably faster than `myBigInt % 17ull`.  
```c++
constexpr auto
operator%(const BigInt& a, const BigInt& b) -> BigInt;
constexpr auto
operator%(const BigInt& a, std::integral auto b) -> /*simple integral type */;

constexpr auto
operator%=(BigInt& a, const BigInt& b) -> BigInt&;
```

#### divmod
Calculates the dividend and reminder of dividing `a` by `b` at the same time.  
Dividing by a 32-bit integer is considerably faster than dividing ba a 64-bit one: `divmod1(myBigInt, 17)` is noticeably faster than `divmod(myBigInt, 17ull)`.  
```c++
constexpr auto
divmod(const BigInt& a, const BigInt& b) -> DivModResult<BigInt, BigInt>
constexpr auto
divmod(const BigInt& a, std::integral auto b) -> DivModResult<BigInt, /*simple integral type */>

constexpr auto
divmod1(const BigInt& a, uint32_t auto b) -> DivModResult<BigInt, uint32_t> // aslo aviable for int32_t.
```

#### left-shift, right-shift
Shifts the given integer by n bits left or right, filling with zeros. The shift assignment operations are always performed in-place.  
Dividing by a 32-bit integer is considerably faster than dividing ba a 64-bit one: `divmod1(myBigInt, 17)` is noticeably faster than `divmod(myBigInt, 17ull)`.  
```c++
constexpr auto
operator<<(const BigInt& a, uint64_t n) -> BigInt;
constexpr auto
operator>>(const BigInt& a, uint64_t n) -> BigInt;

constexpr auto
operator<<=(BigInt& a, uint64_t n) -> BigInt&;
constexpr auto
operator>>=(BigInt& a, uint64_t n) -> BigInt&;
```

#### negation (-), abs
 Both operations return a view of the underlying BigInt with the sign changed accordingly.
```c++
constexpr auto
operator-(const BigInt& a) -> /* negated BigInt view */;
constexpr auto
operator-(BigInt&& a) -> /* negated BigInt view */;

constexpr auto
abs(const BigInt& a) -> /* BigInt view with negative sign removed */;
constexpr auto
abs(BigInt&& a) -> /* BigInt view with negative sign removed */;
```

Following operators are provided: 
* Basic arithmetic operators: `+`, `-`, `*`, `/`, `%`.
* Bitwise shift operators: `<<`, `>>`.
* Inline operators: `+=`, `-=`, `*=`, `/=`.
	* `+=`, `-=`, `<<=`, `>>=` are always performed inline.
	* `*=` is only performed inline if the second argument is an integral type (e.g. int32_t, or uint64_t).
	* `/=` is never performed inline.
* Negation `-`, `abs(const BigInt&)`: Both operations return a view of the underlying BigInt with the sign changed.
* `mult(uint64_t, uint64_t)`: multiplies both numbers and returns an entirely stack-allocated BigInt-like object.
* `divmod(...)`, `divmod1(...)`: to get both the quotient and the reminder with only one calculation.

All arithmetic operators can be used with mixed BigInt and integral types.
```c++
BigInt a{"1390824942691875931654"};
std::cout << (a * 78) << std::endl;
// Output: 108484345529966322669012
```


### Basic Math Functions
#### sqrt
Calculates the integer square root of `y` using Newton's method.  
Throws `std::domain_error` if y < 0.
```c++
constexpr auto
sqrt(const BigInt& y) -> BigInt
```  

#### log2
Calculates the integer logarithm of y for base 2.  
Throws `std::domain_error` if y <= 0.  
This is the same as counting the number of digits of a positive, non-zero integer in binary representation minus one.  
`log2(y)` is very fast with time and space complexity of just O(1).
```c++
constexpr auto
log2(const BigInt& y) -> uint64_t;
```

#### log10
Calculates the integer logarithm of y for base 10. Throws `std::domain_error` if y <= 0
This is the same as counting the number of digits of a positive, non-zero integer in decimal representation minus one.
`log10(y)` is the same as `log(10, y)`.
```c++
constexpr auto
log10(const BigInt& y) -> uint64_t;
```

#### log
Calculates the integer logarithm of y for base `base`. Throws `std::domain_error` if base <= 1 or y <= 0
```c++
constexpr auto
log(const BigInt& base, const BigInt& y) -> uint64_t;
```

#### pow
Raises `base` to the power of `exp`. E.g.: `pow(10, 3) == 1000`. if you need to calculate `pow(a, b) % m` use `pow_mod()` instead.  
Throws `std::domain_error` if both `base` and `exp` are equal to zero.
```c++
constexpr auto
pow(const BigInt& base, uin64_t exp) -> BigInt
```

#### pow_mod
Raises `base` to the power of `exp` modulo `mod`. E.g.: `pow_mod(10, 3, 12) == 4`. This is *way* faster than doing `pow(base, exp) % mod`; especially for large `x` and `y`.  
Throws `std::domain_error` if both `base` and `exp` are equal to zero or if `mod` is zero
```c++
constexpr auto
pow_mod(const BigInt& base, const BigInt& exp, const BigInt& mod) -> BigInt
```

#### digit_sum
Sums all digits in the given base ignoring any sign. E.g.: `digit_sum<10>(-12955) == 1 + 2 + 9 + 5 + 5 == 22`.  
Supported bases are 2 - 64 (inclusive). The bases 2, 4, 8, 16, and 32 are considerable faster than any other base.
```c++
template <int base = 10>
constexpr auto
digit_sum(const BigInt& x) -> uint64_t
```

#### factorial
Calculates the factorial of `x`. `x! = 1 * 2 * 3 * ... * x`.
```c++
constexpr auto
factorial(uin32_t x) -> BigInt
```

#### perm
The Permutation function, Pochhammer symbol, or `nPk`. Calculates the number of ways to choose `k` items from `n` items without repetition and with order. Evaluates to `n! / (n - k)!` when `k <= n` and evaluates to zero otherwise.
```c++
constexpr auto
perm(uint32_t n, uint32_t k) -> BigInt
```

#### comb
The Combinations function, Binomial coefficient, or `nCk`. Calculates the number of ways to choose `k` items from `n` items without repetition and without order. Evaluates to `n! / ((n - k)! * k!)` when `k <= n` and evaluates to zero otherwise.
```c++
constexpr auto
comb(uint32_t n, uint32_t k) -> BigInt
```

#### gcd
Calculates the greatest common divisor of `u` and `v` using Lehmer’s Euclidean GCD Algorithm. The result is never negative. Adapted from Jonathan Sorenson, 1995, An Analysis of Lehmer’s Euclidean GCD Algorithm
```c++
constexpr auto
gcd(const BigInt& u, const BigInt& v) -> BigInt
```

#### lcm
Calculates the least common multiple of `u` and `v` using Lehmer’s Euclidean GCD Algorithm. The result is never negative. 
```c++
constexpr auto
lcm(const BigInt& u, const BigInt& v) -> BigInt
```



### Additional Functions
* A BigInt instance is printable, overloading `<<` for `std::ostream`.
* Use `to_string(const BigInt&)` or `to_string_base10(const BigInt&)` to convert BigInt into a decimal std::string.
* Use `to_string_base2(const BigInt&)`, `to_string_base8(const BigInt&)`, `to_string_base8(const BigInt&)`, `to_string_base16(const BigInt&)` to convert BigInt into a base2, base8, or base16 std::string.
* Use `from_string(std::string_view)` or `from_string_base10(std::string_view)` to convert a decimal string to a BigInt.
* Use `from_string_base2(std::string_view)`, `from_string_base8(std::string_view)`, `from_string_base8(std::string_view)`, `from_string_base16(std::string_view)` to convert a base2, base8, or base16 string into a BigInt.

* Use `is_neg(const BigInt&)`, `is_zero(const BigInt&)`, `is_pos(const BigInt&)` to check whether a BigInt is smaller than, equal to, or greater than zero respectively.
* Use `.size()` to get the number of digits in base 2<sup>64</sup>.

* Use `fits_u64(const BigInt&)` or `fits_i64(const BigInt&)` to check whether `value` would fit into a uint64_t or a int64_t respectively.
* Use `fits_u32(const BigInt&)` or `fits_i32(const BigInt&)` to check whether `value` would fit into a uint32_t or a int32_t respectively.
* Use `as_u64(const BigInt&)` or `as_i64(const BigInt&)` to convert the given `value` to uint64_t or int64_t respectively, assuming it fits.
* Use `as_u32(const BigInt&)` or `as_i32(const BigInt&)` to convert the given `value` to uint32_t or int32_t respectively, assuming it fits.



### Exceptions
A few methods can throw exceptions:
* `std::domain_error` will be thrown by some functions when inputs are outside the domain on which an operation is defined.  
  E.g. division by zero or square root of a negative number.
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

