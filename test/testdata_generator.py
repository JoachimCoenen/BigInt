# This Python file uses the following encoding: utf-8
import sys
from dataclasses import dataclass, field
import itertools as it
import math
from typing import Callable, ClassVar, NewType, cast, Iterator

sys.set_int_max_str_digits(8600*4)
print(f"{sys.get_int_max_str_digits()=}")

INDENT = '\t'


INT32_MAX_00 = 2**31 - 1
INT32_MAX_M2 = INT32_MAX_00 - 2
INT32_MAX_M1 = INT32_MAX_00 - 1
INT32_MAX_P1 = INT32_MAX_00 + 1
INT32_MAX_P2 = INT32_MAX_00 + 2
INT64_MAX_00 = 2**63 - 1
INT64_MAX_M2 = INT64_MAX_00 - 2
INT64_MAX_M1 = INT64_MAX_00 - 1
INT64_MAX_P1 = INT64_MAX_00 + 1
INT64_MAX_P2 = INT64_MAX_00 + 2

UINT32_MAX_00 = 2**32 - 1
UINT32_MAX_M2 = UINT32_MAX_00 - 2
UINT32_MAX_M1 = UINT32_MAX_00 - 1
UINT32_MAX_P1 = UINT32_MAX_00 + 1
UINT32_MAX_P2 = UINT32_MAX_00 + 2
UINT64_MAX_00 = 2**64 - 1
UINT64_MAX_M2 = UINT64_MAX_00 - 2
UINT64_MAX_M1 = UINT64_MAX_00 - 1
UINT64_MAX_P1 = UINT64_MAX_00 + 1
UINT64_MAX_P2 = UINT64_MAX_00 + 2


def combine_huge(value: int) -> int:
	return value * ((value >> 3) + 7) * ((value >> 2) + 71)


ALL_UNIQUE_VALUES_NORMAL: list[int] = [
	0,
	1,
	2,
	3,
	5,
	9,
	10,
	14,
	15,
	97,
	150,
	INT32_MAX_M1 // 5000000,
	550,
	INT32_MAX_M1,
	UINT32_MAX_M1,
	UINT32_MAX_00,
	UINT32_MAX_P1,
	UINT32_MAX_P2,
	18446744065119617025,
	UINT64_MAX_M1,
	UINT64_MAX_00,
	UINT64_MAX_P1,
	UINT64_MAX_P2,  # equivalent to 11, useful for testing carry overflow
	18446744082299486209,
	340282366920938463426481119284349108225,
	340282366920938463463374607431768211455,  # eqivalent to 99, usful for testing carry overflow or underflow
	340282366920938463463374607431768211456,  # eqivalent to 100, usful for testing carry underflow
	340282366920938463481821351505477763072,
	340282366920938463500268095579187314689,
	340282366920938464926816303946059311975,  # broke division as a divisor
	618354700061515834059999999799999999991,
	618354700061515834059999999999999999991,
	1236709400123031668119999999799999999982,
	6277101735386680790718096409961805133951500192106399399936,  # broke division as a dividend
	382362535088167210234626361716426516060166448586892714986920000001800000000081,
	531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889,  # 2^107 - 1, a Mersenne prime.
	85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457,  # 3rd Generalized repunit prime for a=7
	# combine_huge(53113799281676709868958820655246862732959311772703192319944413820040355986085224273916250226522928),
	# combine_huge(33402823669209384634633746074317682114554028236692093846348182135150579680194953954163954280577066639233068267330253081977410514153169870714693030729025353732044727477763072)
]

ALL_UNIQUE_VALUES_2: list[int] = [
	123670940012303166811909400123031668119999999999997999999999821236709400123031668119999999799999999982,
	531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889,  # 2^107 - 1, a Mersenne prime.
	85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457,  # 3rd Generalized repunit prime for a=7
	3823625350881672102346263850534611647968019617164265160601664485868927149869200000018000000009869200000018000081,
	6183547000615158340599999997999999999913402823669209384634633746074317986920000001808505346116479680190068211455,
	340282366920938463500268095518446744082299094001230316681198505346116479680199999948620979187314685053461164796801989,
	3402823669209384634633746074340282366920938463500268095518446744082299486209791873146893176818446744065119617025211456,  # eqivalent to 100, usful for testing carry underflow
	6277101735386680790718096409961805112367094001230316681199991844674406511961702599979999999998233951500192106399399936,  # broke division as a dividend
	3402823669209123670940012303166811999999972114554028236692093846348182135150579680194953954163959999999998238463463374607431768211455,
	3402823669209384634264811192843491082251236709400123031668119999999792114554028236692093846348182135150579680194953954163959999999982,
	340282366920938464926816303531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889946059311975,  # broke division as a divisor
	6183547000615184467440822994862091583405997968019495395416395428057706663923306826733025308197741051415316987071469303072902535373204472799999999999999991,
	33402823669209384634633746074317682114554028236692093846348182135150579680194953954163954280577066639233068267330253081977410514153169870714693030729025353732044727477763072,
]
ALL_UNIQUE_VALUES_BIG = [combine_huge(combine_huge(combine_huge(value))) for value in ALL_UNIQUE_VALUES_2]
ALL_UNIQUE_VALUES_HUGE = [combine_huge(combine_huge(combine_huge(combine_huge(value)))) for value in ALL_UNIQUE_VALUES_2]
ALL_UNIQUE_VALUES_MIX = [0, 1] + ALL_UNIQUE_VALUES_NORMAL[6:14] + ALL_UNIQUE_VALUES_NORMAL[29:] + ALL_UNIQUE_VALUES_BIG[::2]


class TestdataSet:
	@staticmethod
	def with_negative_numbers(unique_values: list[int]) -> list[int]:
		return list(dict.fromkeys(value * sig for value in unique_values for sig in (1, -1)))

	NORMAL_UNSIGNED = ALL_UNIQUE_VALUES_NORMAL
	BIG_UNSIGNED = ALL_UNIQUE_VALUES_BIG
	HUGE_UNSIGNED = ALL_UNIQUE_VALUES_HUGE
	MIX_UNSIGNED = ALL_UNIQUE_VALUES_MIX

	NORMAL = with_negative_numbers(NORMAL_UNSIGNED)
	BIG = with_negative_numbers(BIG_UNSIGNED)
	HUGE = with_negative_numbers(HUGE_UNSIGNED)
	MIX = with_negative_numbers(MIX_UNSIGNED)


def list_to_str(list_: list) -> str:
	nl = '\n' + INDENT
	contents = ''.join(f'{nl}{item}' for item in list_)
	return '[]' if not contents else f'[{contents}\n]'


FilteredArgs = NewType('FilteredArgs', tuple[int, ...])


@dataclass
class Operation:
	type_name: ClassVar[str] = 'abstract'
	param_count: ClassVar[int] = 0
	name: str
	op: Callable[[int, ...], int | str]
	valid_for_args: Callable[[int, ...], bool] | None = None
	modify_args: Callable[[int, ...], tuple[int, ...]] | None = field(default=None, kw_only=True)
	testdata: list[int] | tuple[list[int], ...] = field(default_factory=lambda: TestdataSet.NORMAL, kw_only=True)


@dataclass
class UnaOperation(Operation):
	type_name: ClassVar[str] = 'UnaOpTest'
	param_count: ClassVar[int] = 1
	op: Callable[[int], int | str]
	valid_for_args: Callable[[int], bool] | None = None
	modify_args: Callable[[int], tuple[int]] | None = field(default=None, kw_only=True)
	testdata: list[int] | tuple[list[int]] = field(default_factory=lambda: TestdataSet.NORMAL, kw_only=True)


@dataclass
class BinOperation(Operation):
	type_name: ClassVar[str] = 'BinOpTest'
	param_count: ClassVar[int] = 2
	op: Callable[[int, int], int | str]
	valid_for_args: Callable[[int, int], bool] | None = None
	modify_args: Callable[[int, int], tuple[int, int]] | None = field(default=None, kw_only=True)
	testdata: list[int] | tuple[list[int], list[int]] = field(default_factory=lambda: TestdataSet.NORMAL, kw_only=True)


@dataclass
class TriOperation(Operation):
	type_name: ClassVar[str] = 'TriOpTest'
	param_count: ClassVar[int] = 3
	op: Callable[[int, int, int], int | str]
	valid_for_args: Callable[[int, int, int], bool] | None = None
	modify_args: Callable[[int, int, int], tuple[int, int, int]] | None = field(default=None, kw_only=True)
	testdata: list[int] | tuple[list[int], list[int], list[int]] = field(default_factory=lambda: TestdataSet.NORMAL, kw_only=True)


def get_val_type(value: int) -> str:
	if value > UINT64_MAX_00:
		return 'BIG'
	elif value > INT64_MAX_00:
		return 'U64'
	elif value > UINT32_MAX_00:
		return 'S64'
	elif value > INT32_MAX_00:
		return 'U32'
	elif value >= 0:
		return 'S32'
	elif value >= -INT32_MAX_00 - 1:
		return 'N32'
	elif value >= -INT64_MAX_00 - 1:
		return 'N64'
	else:
		return 'BIG'


def make_param(arg: int) -> str:
	type_ = get_val_type(arg)
	return f'{type_},{arg}'


def make_expected_result(operation: Operation, args: FilteredArgs) -> str:
	return operation.op(*args)


def make_operation_test(operation: Operation, args: FilteredArgs) -> str:
	assert len(args) == operation.param_count, f'param count does not match: expected {operation.param_count}, but got {len(args)}'
	expected = make_expected_result(operation, args)
	cpp_args = ';'.join(make_param(arg) for arg in args)
	return f'{cpp_args};{expected}'


def make_all_operation_tests(filtered_args: Iterator[FilteredArgs], operation: Operation) -> list[str]:
	return [make_operation_test(operation, args) for args in filtered_args]


def sign(a: int) -> int:
	return -1 if a < 0 else (1 if a > 0 else 0)


def digit_sum_10(a: int) -> int:
	return sum(bytes(str(abs(a)), encoding='utf-8')) - len(str(abs(a)))*ord('0')


def digit_sum_16(a: int) -> int:
	mapp = {
		ord('0'): 0,
		ord('1'): 1,
		ord('2'): 2,
		ord('3'): 3,
		ord('4'): 4,
		ord('5'): 5,
		ord('6'): 6,
		ord('7'): 7,
		ord('8'): 8,
		ord('9'): 9,
		ord('a'): 10,
		ord('b'): 11,
		ord('c'): 12,
		ord('d'): 13,
		ord('e'): 14,
		ord('f'): 15,
	}
	return sum(map(mapp.__getitem__, bytes(f'{abs(a):x}', encoding='utf-8')))


def log2(y: int) -> int:
	return y.bit_length() - 1


def log10(y: int) -> int:
	return len(f'{y}') - 1


def logg(base: int, y: int) -> int:
	exp = log2(y) // log2(base)
	i = 0
	while base ** exp > y:
		i += 1
		exp -= 1
	return exp


def log_checked(base: int, y: int) -> int:
	exp = logg(base, y)
	assert base ** exp <= y, f"got {base ** exp} but expected <= {y}; log({base}, {y}) = {exp}"
	assert base ** (exp+1) >= y, f"got {base ** (exp+1)} but expected >= {y}; log({base}, {y}) = {exp}"
	return exp


def digits_of_pow_result_approx(a: int, b: int) -> int:
	if a == 0 or a == 1 or b == 0:
		return 1
	return a.bit_length() * b * 10000000000 // 33219280949
	# a.bit_length() * b / math.log2(10)
	# math.log10(abs(a)) * b


BINARY_ARITHMETIC_OPERATIONS: list[Operation] = [
	BinOperation('lshift', lambda a, b:    a << b,                   testdata=(TestdataSet.NORMAL, [*range(0, 9), *range(61, 66), 127, 128, 129, 191, 192, 193, 550, 1100, 2595])),
	BinOperation('rshift', lambda a, b:    (abs(a) >> b) * sign(a),  testdata=(TestdataSet.NORMAL, [*range(0, 9), *range(61, 66), 127, 128, 129, 191, 192, 193, 550, 1100, 2595])),
	BinOperation('add',    lambda a, b:    a + b),
	BinOperation('sub',    lambda a, b:    a - b),
	BinOperation('mul',    lambda a, b:    a * b),
	BinOperation('mul_karatsuba', lambda a, b: a * b,                lambda a, b: max(a.bit_length(), b.bit_length()) > 64 * 16, testdata=TestdataSet.MIX),
	UnaOperation('sqr',    lambda a:       a * a,                    testdata=TestdataSet.HUGE),
	BinOperation('div',    lambda a, b:    a // b,                   lambda a, b:    b != 0),
	BinOperation('mod',    lambda a, b:    a % b,                    lambda a, b:    b != 0),
	BinOperation('divmod', lambda a, b:    f'{a // b}|{a % b}',      lambda a, b:    b != 0),
	UnaOperation('sqrt',   lambda a:       math.isqrt(a),            lambda a:       a >= 0),
	BinOperation('pow',    lambda a, b:    a ** b,                   lambda a, b:    (a != 0 or b != 0) and (b >= 0) and (a == 0 or digits_of_pow_result_approx(a, b) < sys.get_int_max_str_digits())),
	TriOperation('powmod', lambda a, b, c: pow(a, b, c),             lambda a, b, c: True and
																			(a != 0 or b != 0) and (b > 0) and c != 0 and  # mathematical feasibility
																			(abs(a) >= 97 or (b < 3 and abs(c) < 5)) and (abs(c) <= 97 or (abs(a) >= UINT64_MAX_P2 or b < UINT32_MAX_00)) and  # reduces the amount af test cases
																			(len(list(filter(lambda x: x % 2 == 0, [a, b, c]))) in {0, 1, 2, 3})  # reduces the amount af test cases even more
	),
	UnaOperation('log2',   lambda a:       log2(a),                  lambda a:       a > 0),
	UnaOperation('log10',  lambda a:       log10(a),                 lambda a:       a > 0),
	BinOperation('log',    lambda a, b:    log_checked(a, b),        lambda base, y: y > 0 and base > 1),

	BinOperation('perm',   lambda a, b:    math.perm(a, b),         lambda a, b: 0 <= a < INT32_MAX_M1 and 0 <= b < INT32_MAX_M1),
	BinOperation('comb',   lambda a, b:    math.comb(a, b),         lambda a, b: 0 <= a < INT32_MAX_M1 and 0 <= b < INT32_MAX_M1),

	BinOperation('gcd',    lambda a, b:    math.gcd(a, b)),
	BinOperation('lcm',    lambda a, b:    math.lcm(a, b)),

	# UnaOperation('bitwise_not', lambda a:    ~abs(a) * (-1 if not a < 0 else +1)),
	BinOperation('bitwise_and', lambda a, b: (abs(a) & abs(b)) * (-1 if (a < 0) and (b < 0) else +1)),
	BinOperation('bitwise_or',  lambda a, b: (abs(a) | abs(b)) * (-1 if (a < 0) or (b < 0) else +1)),
	BinOperation('bitwise_xor', lambda a, b: (abs(a) ^ abs(b)) * (-1 if (a < 0) != (b < 0) else +1)),


	UnaOperation('digit_sum_10', lambda a: digit_sum_10(a)),
	UnaOperation('digit_sum_16', lambda a: digit_sum_16(a)),
	UnaOperation('to_string_10', lambda a: a),
	UnaOperation('to_string_16', lambda a: ('-' if a < 0 else '') + f'{abs(a):x}'),

	BinOperation('huge',   lambda a, b: 0, testdata=TestdataSet.HUGE),  # can be used for crude performance test
	BinOperation('mix',    lambda a, b: 0, testdata=TestdataSet.MIX),   # can be used for crude performance test
]


def build_all_arguments(testdata: list[int] | tuple[list[int], ...], param_count: int) -> Iterator[tuple[int, ...]]:
	if isinstance(testdata, tuple):
		if param_count != len(testdata):
			raise ValueError(f"expected testdata for {param_count} arguments, but found testdata for {len(testdata)} arguments.")
		return it.product(*testdata)
	return it.product(*([testdata] * param_count))


def filter_all_arguments(operation: Operation, test_data: Iterator[tuple[int, ...]]) -> Iterator[FilteredArgs]:
	if (modify_args := operation.modify_args) is not None:
		test_data = [modify_args(*args) for args in test_data]
	if (valid_for_args := operation.valid_for_args) is not None:
		test_data = [args for args in test_data if valid_for_args(*args)]
	return cast(Iterator[FilteredArgs], iter(test_data))


def make_values_for_test_csv(operation: Operation) -> tuple[str, str]:
	all_arguments = build_all_arguments(operation.testdata, operation.param_count)
	lines = make_all_operation_tests(filter_all_arguments(operation, all_arguments), operation)
	print(f"{operation.name} finished!")
	return f'values_for_{operation.name}_test.csv', '\n'.join(lines)


def make_values_for_test_csvs() -> list[tuple[str, str]]:
	files = [
		*map(make_values_for_test_csv, BINARY_ARITHMETIC_OPERATIONS),
	]

	return files


if __name__ == "__main__":
	files = make_values_for_test_csvs()
	# print(values_for_test_h)
	for name, content in files:
		with open(f'./test_data/{name}', 'w') as file:
			file.write(content)





















