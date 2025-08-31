# BigInt

BigInt is a header only library for working with large integer values bigger than the hardware limit.  
BigInt has no additional dependencies and is designed to be easy to use without unnecessary performance compromises.  
The numbers a represented in base 18,446,744,073,709,551,616 (= 2<sup>64</sup>) to maximize memory efficiency and speed.  
It is fully `constexpr'd` and therefore partial result and constants can be calculated at compile time.

```c++
std::cout << pow(3_big, 300) << std::endl;
// Outputs: 136891479058588375991326027382088315966463695625337436471480190078368997177499076593800206155688941388250484440597994042813512732765695774566001

std::cout << factorial(1'234) << std::endl; // over 3k decimal digits or 1.37 kB
// Outputs: 51084981466469576881306176261004598750272741624636207875758364885679783886389114119904367398214909451616865959797190085595957216060201081790863562740711392408402606162284424347926444168293770306459877429620549980121621880068812119922825565603750036793657428476498577316887890689284884464423522469162924654419945496940052746066950867784084753581540148194316888303839694860870357008235525028115281402379270279446743097868896180567901452872031734195056432576568754346528258569883526859826727735838654082246721751819658052692396270611348013013786739320229706009940781025586038809493013992111030432473321532228589636150722621360366978607484692870955691740723349227220367512994355146567475980006373400215826077949494335370591623671142026957923937669224771617167959359650439966392673073180139376563073706562200771241291710828132078928672693377605280698340976512622686207175259108984253979970269330591951400265868944014001740606398220709859461709972092316953639707607509036387468655214963966625322700932867195641466506305265122238332824677892386098873045477946570475614470735681011537762930068333229753461311175690053190276217215938122229254011663319535668562288276814566536254139944327446923749675156838399258655227114181067181300031191298489076680172983118121156086627360397334232174932132686080901569496392129263706595509472541921027039947595787992209537069031379517112985804276412719491334730247762876260753560199012424360211862466047511184797159731714330368251192307852167757615200611669009575630075581632200897019110165738489288234845801413542090086926381756642228872729319587724120647133695447658709466047131787467521648967375146176025775545958018149895570817463048968329692812003996105944812538484291689075721849889797647554854834050132592317503861422078077932841396250772305892378304960421024845815047928229669342818218960243579473180986996883486164613586224677782405363675732940386436560159992961462550218529921214223556288943276860000631422449845365510986932611414112386178573447134236164502410346254516421812825350152383907925299199371093902393126317590337340371199288380603694517035662665827287352023563128756402516081749705325705196477769315311164029733067419282135214232605607889159739038923579732630816548135472123812968829466513428484683760888731900685205308016495533252055718190142644320009683032677163609744614629730631454898167462966265387871725580083514565623719270635683662268663333999029883429331462872848995229714115709023973771126468913873648061531223428749576267079084534656923514931496743842559669386638509884709307166187205161445819828263679270112614012378542273837296427044021252077863706963514486218183806491868791174785424506337810550453063897866281127060200866754011181906809870372032953354528699094096145120997842075109057859226120844176454175393781254004382091350994101959406590175402086698874583611581937347003423449521223245166665792257252160462357733000925232292157683100179557359793926298007588370474068230320921987459976042606283566005158202572800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

const auto exponent = pow(2_big, 4'423) - 1; // 1'331 decimal digits (Mersenne prime)
const auto modulo = pow(2_big, 11'213) - 1;  // 3'375 decimal digits (Mersenne prime)
std::cout << pow_mod(factorial(1'234), exponent, modulo) << std::endl;
// Output: 955391665608243513043495538268296899246954874924245089498096340386524437251196295593632931870943445678985991483313879138959709401500844805410810607296464582774490874013709940054275218772361289955459653818656219765760709299934396995532169939354231695893970363402071593393495046800780926026819156480445145233271399911835703191946704894204303615159583761628413020404794196445954148107605863666947472398848363387245204458841242987895624650042798222996369039484448398527371843137678236664622967933301930477163587296318204309316905541442171818403082397052155855551013827631300138392617463507295598766432238244927532603702174625664029147734159484258425912211570061758932768879126012144572295044488581621803988056991279294826449920028558639440118797933935848941550647511209614891499488631967521834829107530485712631717298337067010483774272106369894655292574793880734361044761241865900834333412119496277743759055581633247342490859643484319045382768660378579351654998566716023479932910026407332178985364788489633615298168737502072009804442770861359363632045539709910565548419163417407463076976623168140769738248721270321986689511156749395378508607805152130947562806499285625004881905618210375358873777998489457014054559131103866627011466123075978538102983345673837849077207757036687457957637592199268053746844473829894440057871981383438544978392978484006395385628004779643068584935183968482815310055781917523880954346039849863598190302931902722282299036532059693635494638264133477073735137783350133064665201536985756023798551477467333813662345157623532795419097384137504217268644189055102235283694553181205616630067425206298260675904187254132486212155855858301987602899157236953296937390669368865204408477062695914837023567762525272347783705448315595074072179046435738103516864550848337675524140794138567626057282218357753954545243309714844068042699733880337112547053446837778406280211211336111503967973097707968084463498124859563539456727591580113091724097889051453268324627518667318451816177997552604242300485381903166168469449470618416783727791438237001179488872377894891984758944181964515452876760738636239675134835244285251152232278658963476375247215300067321600999453982973767873832464794888770511169237675853101079941439205020143282999989643626409469245030865125736244590917892200015483215773973579053916320004069038089777727001515750983267506299101410288699335702326547821714183964773248507139287741792157182493943945354684140644088496712120894386900415529350661809132689630657057064802147812048966595752047761439983717641033584110545921164142903570069530221986316977025163839706136772299939669576744099208511437968720874508157401410236904676853510114643576951468724956550188003126548356794033985081728928758413417856866205932936993569700024343848763900895370259730446613907280673800897748667168848129350055265851134538847766879675517923397979607864637621929071413844014498533185290321468429369586596352622169322970283058604268823252493321413972917038656250257585294055655498015145938330326710699000297428305123939774150616757370415631887435890469235495295580280856239352777316350046187332365980503114269688548872902244960178393751921687904553634554787820948879541070839447323261440251994533716942671126966405739040522452538698181669546559278652295899791966721729927482679441970927055080658249844214719024886020266707855351985199253736230878034061925508065834660089242993686181

```


## Contents

<!-- TOC -->
* [Requirements](#requirements)
* [Installing](#installing)
* [Overview](#overview)
  * [Features](#features)
  * [Initialization](#initialization)
  * [Arithmetic Operators](#arithmetic-operators)
  * [Maths Functions](#maths-functions)
  * [Additional Functions](#additional-functions)
  * [Exceptions](#exceptions)
  * [Implementing your own algorithms](#implementing-your-own-algorithms)
* [Examples](#examples)
  * [Factorial](#factorial)
  * [Digit Sum](#digit-sum)
* [Contributing](#contributing)
* [License](#license)
<!-- TOC -->


## Requirements
BigInt requires **C++23** or higher and a compatible GCC (or MinGW), Clang, or MSVC compiler.


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

If you experience problems, found a bug, or have suggestions, Feel free to create a new issue or make a pull request.


## Overview
For simplicity reasons, this overview will use `using bigint;`. If you prefer not to do so, you can access the BigInt class as `bigint::BigInt`.  

### Features

- BigInt literals in decimal, hexadecimal, octal, and binary using `_big`:
  ```c++
  auto a = 12300000000000000000000000000000000000000_big;
  auto b = 1'000'000'000'000'000_big;  // with digit separators
  auto c = -0xFF00'ff00'0000'0000_big;  // hexadecimal literal
  auto d = 0b10101010'11000011_big;  // binary literal
  auto e = 0777777777777777777_big;  // octal literal
  ```
* Everything can be calculated at compile time, because BigInt is fully `constexpr'd`:
  ```c++
  constexpr auto a = -1 + pow(3_big, 3000) * 5;
  constexpr auto b = gdc(12368464545159878212501232471351513542431_big, 0x1385a5347761f71414247342dda76655535_big);
  constinit auto c = 12368464545159878212501232471351513542431_big / 0; // does not compile, because division by zero
  ```
* Supports all arithmetic and bitwise binary operators (`+`, `-`, `*`, `/`, `%`,  `&`, `|`, `^`, `<<`, `>>`) as well as many useful math functions:
  ```c++
  auto a = 132456789_big - 987654321_big * 1235467890_big;
  auto b = (a % 999999_big) << 513
  auto c = b & a;
  c *= a;
  
  pow_mod(123456789_big, 987654321_big, 5555555555_big); // efficient modular exponentiation
  log(pow(3_big, 100), factorial(1000));
  log2(factorial(1000));              // log2() is computed in O(1)!
  lcm(factorial(100), pow(3, 100));   // least common multiple
  divmod(factorial(100), pow(3, 50)); // combined division and modulo
  digit_sum(factorial(100), 13);      // digit sum in base 13
  // etc ...
  ```
* All arithmetic and bitwise binary operators are also available as GMP-style methods, which can save on unnecessary allocations: 
  ```c++
  BigInt a = 1234567890 * 0x1234'5678'9abc'def0_big - factorial(100);
  
  BigInt b;
  mul(b, 0x1234'5678'9abc'def0_big, 1234567890);
  sub(b, b factorial(100));
  
  assert(a == b)
  ```
* Easy string conversion even for decimal, hexadecimal, octal, binary, and other bases:
  ```c++
  constexpr auto a = from_string("-999888777666555444333222111000");
  constexpr auto a = from_string("-1011001101", 2);
  constexpr auto a = from_string("-abc057", 16);
  constexpr auto c = from_string("123456777", 7); // does not compile, because '7' is not a valid digit in base 7
  constexpr std::string base_10 = to_string(-0xabc057_big, 10)
  constexpr std::string base_17 = to_string(-0xabc057_big, 17)
  ```
* EasyConversion from / to integral types:
  ```c++
  fits<uint16_t>(-1234_big); // false
  fits<int16_t>(-1234_big); // true
  
  int64_t a = as_i64(-1234_big);
  uint16_t b = as_integral<uint16_t>(-1234_big);
  BigInt c = BigInt{-99LL};
  ```

### Initialization
It is possible to instantiate a BigInt in multiple ways:  
```c++
BigInt A;  // will hold the value +0.
BigInt B1 = 123456789_big;     // BigInt literal
BigInt B2 = 123'456'789_big;   // BigInt literal with digit separators
BigInt B3 = 0x5abcDEF1_big;    // hexadecimal BigInt literal
BigInt B4 = 0b10101010110_big; // binary BigInt literal
BigInt B5 = 077777755555_big;  // octal BigInt literal
BigInt C{100};  // B will hold the value +100.
BigInt D{-50};  // C will hold the value -50.
BigInt E{50, Sign::NEG};  // D will hold the value -50.
constexpr BigInt F = 13 - 4561273837128312881999_big;  // E will hold the value -4561273837128312881986.
```

But *not* like this:  
```c++
BigInt G = 2;  // BigInt(uint64_t) is also explicit. prevents accidental use of expensive operations.
```

### Arithmetic Operators

All arithmetic operators can be used with mixed BigInt and integral types.
```c++
BigInt a = 1390824942691875931654_big;
std::cout << (a * 78) - 12 << std::endl;
// Output: 108484345529966322669000
```

All arithmetic operators have a function representation.
```c++
BigInt a = 1390824942691875931654_big;
mul(a, a, 78);
sub(a, a, 12);
std::cout << a << std::endl;
// Output: 108484345529966322669000
```

#### Addition (+, +=, add)
Sums two integers. The addition assignment operations are always performed in-place. 

#### Subtraction (-, -=, sub)
Subtracts `b` from `a`. The subtraction assignment operations are always performed in-place.

#### Multiplication (*, *=, mul)
Multiplies two integers. The multiplication assignment operation is only performed in-place if the second multiplicand is an integral type (like `int` or `uint64_t`).

#### Division (/, /=, div)
Divides `a` by `b`. The division assignment operation is only performed in-place if the divisor has 64 or fewer bits (e.g. `uint64_t`, `int16_t`, `BigInt` with `.size() == 1`).  
Dividing by a 32-bit integer is 1.26 times faster than dividing by a 64-bit one (`myBigInt / 7` is faster than `myBigInt / 7ull`).

#### Modulo (%, %=, mod)
Calculates the reminder of dividing `a` by `b`. The modulo assignment operation is *never* performed in-place.  
Dividing by a 32-bit integer is 1.26 times faster than dividing by a 64-bit one (`myBigInt % 7` is faster than `myBigInt % 7ull`).

#### divmod(a, b)
Calculates the dividend and reminder of dividing `a` by `b` at the same time.  
Dividing by a 32-bit integer is 1.26 times faster than dividing by a 64-bit one (`divmod(myBigInt, 17)` is faster than `divmod(myBigInt, 17ull)`).

#### Left-shift, Right-shift (<<, >>, <<=, >>=)
Shifts the given integer by n bits left or right, filling with zeros. The shift assignment operations are always performed in-place.

#### bitwise AND, OR, XOR (&, |, ^, &=, |=, ^=)
Bitwise AND, bitwise OR, and bitwise XOR operations. The bitwise assignment operations are always performed in-place.  
The signs are handled independently of the digits. A negative sign is treated as a binary `1`. 

#### Negation (-), abs
 Both operations return a view of the underlying BigInt with the sign changed accordingly.


### Maths Functions
located in `bigint/math.h`.

#### sqrt(y)
Calculates the integer square root of `y` using Newton's method.  
Throws `std::domain_error` if y < 0.
```c++
constexpr auto
sqrt(const BigInt& y) -> BigInt
```  

#### log2(y)
Calculates the integer logarithm of y for base 2.  
Throws `std::domain_error` if y <= 0.  
This is the same as counting the number of digits of a positive, non-zero integer in binary representation minus one.  
`log2(y)` is very fast with time and space complexity of just O(1).
```c++
constexpr auto
log2(const BigInt& y) -> uint64_t;
```

#### log10(y)
Calculates the integer logarithm of y for base 10. Throws `std::domain_error` if y <= 0
This is the same as counting the number of digits of a positive, non-zero integer in decimal representation minus one.
`log10(y)` is the same as `log(10, y)`.
```c++
constexpr auto
log10(const BigInt& y) -> uint64_t;
```

#### log(base, y)
Calculates the integer logarithm of y for base `base`. Throws `std::domain_error` if base <= 1 or y <= 0
```c++
constexpr auto
log(const BigInt& base, const BigInt& y) -> uint64_t;
```

#### pow(base, exp)
Raises `base` to the power of `exp`. E.g.: `pow(10, 3) == 1000`. if you need to calculate `pow(a, b) % m` use `pow_mod()` instead.  
Throws `std::domain_error` if both `base` and `exp` are equal to zero.
```c++
constexpr auto
pow(const BigInt& base, uin64_t exp) -> BigInt
```

#### pow_mod(base, exp, mod)
Raises `base` to the power of `exp` modulo `mod`. E.g.: `pow_mod(10, 3, 12) == 4`. This is *way* faster than doing `pow(base, exp) % mod`; especially for large `x` and `y`.  
Throws `std::domain_error` if both `base` and `exp` are equal to zero or if `mod` is zero
```c++
constexpr auto
pow_mod(const BigInt& base, const BigInt& exp, const BigInt& mod) -> BigInt
```

#### digit_sum(x)
Sums all digits in the given base ignoring any sign. E.g.: `digit_sum(-12955_big) == 1 + 2 + 9 + 5 + 5 == 22`.  
Supported bases are 2 - 64 (inclusive). The bases 2, 4, 8, 16, and 32 are considerable faster than any other base.
```c++
constexpr auto
digit_sum(const BigInt& x, uint_fast8_t base = 10) -> uint64_t
```

#### factorial(x), aka. x!
Calculates the factorial of `x`. `x! = 1 * 2 * 3 * ... * x`.
```c++
constexpr auto
factorial(uin32_t x) -> BigInt
```

#### perm(n, k)
The Permutation function, Pochhammer symbol, or `nPk`. Calculates the number of ways to choose `k` items from `n` items without repetition and with order. Evaluates to `n! / (n - k)!` when `k <= n` and evaluates to zero otherwise.
```c++
constexpr auto
perm(uint32_t n, uint32_t k) -> BigInt
```

#### comb(n, ki)
The Combinations function, Binomial coefficient, or `nCk`. Calculates the number of ways to choose `k` items from `n` items without repetition and without order. Evaluates to `n! / ((n - k)! * k!)` when `k <= n` and evaluates to zero otherwise.
```c++
constexpr auto
comb(uint32_t n, uint32_t k) -> BigInt
```

#### gcd(u, v)
Calculates the greatest common divisor of `u` and `v` using Lehmer’s Euclidean GCD Algorithm. The result is never negative. Adapted from Jonathan Sorenson, 1995, An Analysis of Lehmer’s Euclidean GCD Algorithm
```c++
constexpr auto
gcd(const BigInt& u, const BigInt& v) -> BigInt
```

#### lcm(u, v)
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

* Use `fits_u64(const BigInt&)` or `fits_i64(const BigInt&)` to check whether `value` would fit into a uint64_t or an int64_t respectively.
* Use `fits_u32(const BigInt&)` or `fits_i32(const BigInt&)` to check whether `value` would fit into a uint32_t or an int32_t respectively.
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



### Implementing your own algorithms
When implementing your own algorithms or function, make sure to use the concept `is_BigInt_like` for const parameters 
instead of he type `BigInt`. This helps reduce unnecessary copies:
```c++
auto sum_sqr_bad(
    const BigInt& a,
    const BigInt& b
) -> BigInt { return a*a + b*b; }

auto sum_sqr_better(
    const is_BigInt_like auto& a,
    const is_BigInt_like auto& b
) -> BigInt { return a*a + b*b; }

  sum_sqr_bad(abs(c), -d); // the digits of c & d are copied
  sum_sqr_better(abs(c), -d); // the digits of c & d are not copied.
  ```

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
digit_sum(const is_BigInt_like auto& v) -> uint64_t {
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

