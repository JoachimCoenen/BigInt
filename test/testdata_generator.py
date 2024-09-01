# This Python file uses the following encoding: utf-8
from dataclasses import dataclass
import itertools as it
import math
from operator import itemgetter
from typing import Callable, ClassVar, Iterable


INDENT = '\t'


def indent_multiline(text: str, *, tabs: int, indentFirstLine: bool = True):
	s = ''

	if not tabs:
		s += text
		return s

	splitLines = text.splitlines()
	if not splitLines:
		s += text
		return s

	indent = INDENT * tabs

	iter_ = iter(splitLines)
	firstLine = next(iter_)

	if indentFirstLine:
		s += indent
	s += firstLine

	for line in iter_:
		s += '\n'
		s += indent
		s += line
	if len(text) > 0 and text[-1] == '\n':
		s += '\n'
	return s


def list_to_str(list_: list) -> str:
	nl = '\n' + INDENT
	contents = ''.join(f'{nl}{item}' for item in list_)
	return '[]' if not contents else f'[{contents}\n]'


@dataclass(unsafe_hash=True, frozen=True)
class Arg:
	index: int
	value: int


@dataclass
class Operation:
	type_name: ClassVar[str] = 'abstract'
	param_count: ClassVar[int] = 0
	name: str


@dataclass
class UnaOperation(Operation):
	type_name: ClassVar[str] = 'UnaOpTest'
	param_count: ClassVar[int] = 1
	op: Callable[[int], int]
	validForArgs: Callable[[int], bool] | None = None


@dataclass
class BinOperation(Operation):
	type_name: ClassVar[str] = 'BinOpTest'
	param_count: ClassVar[int] = 2
	op: Callable[[int, int], int]
	validForArgs: Callable[[int, int], bool] | None = None


@dataclass
class TriOperation(Operation):
	type_name: ClassVar[str] = 'TriOpTest'
	param_count: ClassVar[int] = 3
	op: Callable[[int, int, int], int]
	validForArgs: Callable[[int, int, int], bool] | None = None


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


def get_ValType(value: int) -> str:
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


# def get_ValType(value: int) -> str:
# 	if value > UINT64_MAX_00 or value < (-INT32_MAX_00 - 1) :
# 		return 'VT::BIG'
# 	elif value > UINT32_MAX_00:
# 		return 'VT::U64'
# 	else:
# 		return 'VT::U32'


def make_Param(arg: Arg) -> str:
	type_ = get_ValType(arg.value)
	return f'{type_},{arg.value}'


def make_ExpectedResult(operation: Operation, *args: Arg) -> str | None:
	values = [arg.value for arg in args]
	if operation.validForArgs is None or operation.validForArgs(*values):
		return operation.op(*values)
	else:
		return None


def make_opTest(operation: Operation, *args: Arg) -> str | None:
	assert len(args) == operation.param_count, f'param count does not match: expected { operation.param_count}, but got {len(args)}'
	expected = make_ExpectedResult(operation, *args)
	if expected is None:
		return None
	else:
		cpp_args = ';'.join(make_Param(arg) for arg in args)
		return f'{cpp_args};{expected}'


def make_ALL_TEST_VALUES(all_args: list[tuple[Arg, ...]], operation: Operation) -> list[str]:
	all_op_tests = list(filter(None, map(lambda args: make_opTest(operation, *args), all_args)))
	return all_op_tests



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
	return  len(f'{y}') - 1

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


BINARY_ARITHMETIC_OPERATIONS: list[Operation] = [
	BinOperation('lshift', lambda a, b:    a << b,                   lambda a, b:    0 <= b <= 1000),
	BinOperation('rshift', lambda a, b:    (abs(a) >> b) * sign(a),  lambda a, b:    0 <= b <= 1000),
	BinOperation('add',    lambda a, b:    a + b),
	BinOperation('sub',    lambda a, b:    a - b),
	BinOperation('mul',    lambda a, b:    a * b),
	BinOperation('div',    lambda a, b:    a // b,                   lambda a, b:    b != 0),
	BinOperation('mod',    lambda a, b:    a % b,                    lambda a, b:    b != 0),
	BinOperation('divmod', lambda a, b:    f'{a // b}|{a % b}',      lambda a, b:    b != 0),
	UnaOperation('sqrt',   lambda a:       math.isqrt(a),            lambda a:       a >= 0),
	BinOperation('pow',    lambda a, b:    a ** b,                   lambda a, b:    (a != 0 or b != 0) and (b >= 0) and (a == 0 or math.log10(abs(a))*b <= 2000) ),
	TriOperation('powmod', lambda a, b, c: pow(a, b, c),             lambda a, b, c: True and
																			(a != 0 or b != 0) and (b > 0) and c != 0 and  # mathematical feasibility
																			(abs(a) >= 97 or (b < 3 and abs(c) < 5)) and (abs(c) <= 97 or (abs(a) >= UINT64_MAX_P2 or b < UINT32_MAX_00))  # reduces the amount af test cases
	),
	UnaOperation('log2',   lambda a:       log2(a),                  lambda a:       a > 0),
	UnaOperation('log10',  lambda a:       log10(a),                 lambda a:       a > 0),
	BinOperation('log',    lambda a, b:    log_checked(a, b),        lambda base, y: y > 0 and base > 1),

	BinOperation('perm',    lambda a, b:    math.perm(a, b),         lambda a, b: 0 <= a <= INT32_MAX_M1 and 0 <= b <= INT32_MAX_M1),
	BinOperation('comb',    lambda a, b:    math.comb(a, b),         lambda a, b: 0 <= a <= UINT32_MAX_P2 and 0 <= b <= UINT32_MAX_P2),

	BinOperation('gcd',    lambda a, b:    math.gcd(a, b)),
	BinOperation('lcm',    lambda a, b:    math.lcm(a, b)),

	UnaOperation('digit_sum_10', lambda a: digit_sum_10(a)),
	UnaOperation('digit_sum_16', lambda a: digit_sum_16(a)),
	UnaOperation('to_string_10', lambda a: a),
	UnaOperation('to_string_16', lambda a: ('-' if a < 0 else '') + f'{abs(a):x}'),
]

ALL_UNIQUE_VALUES: list[int] = [
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
		UINT32_MAX_M1,
		UINT32_MAX_00,
		UINT32_MAX_P1,
		UINT32_MAX_P2,
		18446744065119617025,
		UINT64_MAX_M1,
		UINT64_MAX_00,
		UINT64_MAX_P1,
		UINT64_MAX_P2,  # eqivalent to 11, usful for testing carry overflow
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
]

ALL_UNIQUE_VALUES_SIGNED: list[int] = list(dict.fromkeys(value * sign for value in ALL_UNIQUE_VALUES for sign in (1, -1)))
ALL_UNIQUE_ARGUMENTS: list[Arg] = [Arg(i, arg) for i, arg in enumerate(ALL_UNIQUE_VALUES_SIGNED)]


ALL_UNA_ARGUMENTS: list[tuple[Arg]] = list(it.product(ALL_UNIQUE_ARGUMENTS))
ALL_BIN_ARGUMENTS: list[tuple[Arg, Arg]] = list(it.product(ALL_UNIQUE_ARGUMENTS, ALL_UNIQUE_ARGUMENTS))
ALL_TRI_ARGUMENTS: list[tuple[Arg, Arg, Arg]] = list(it.product(ALL_UNIQUE_ARGUMENTS, ALL_UNIQUE_ARGUMENTS, ALL_UNIQUE_ARGUMENTS))


def select_all_arguments(operation: Operation) -> list[tuple[Arg, ...]]:
	match operation.param_count:
		case 1: return ALL_UNA_ARGUMENTS
		case 2: return ALL_BIN_ARGUMENTS
		case 3: return ALL_TRI_ARGUMENTS
		case _: raise ValueError(f"unhandeled param_count: {param_count!r}")


def make_values_for_test_csv(operation: Operation) -> tuple[str, str]:
	lines = make_ALL_TEST_VALUES(
			select_all_arguments(operation),
			operation
		)
	return (
		f'values_for_{operation.name}_test.csv',
		'\n'.join(lines)
	)


def make_values_for_test_csvs() -> list[tuple[str, str]]:
	files = [
		*map(make_values_for_test_csv, BINARY_ARITHMETIC_OPERATIONS),
	]

	return files;


if __name__ == "__main__":
	files = make_values_for_test_csvs()
	# print(values_for_test_h)
	for name, content in files:
		with open(f'./test_data/{name}', 'w') as file:
			file.write(content)





















