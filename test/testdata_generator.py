# This Python file uses the following encoding: utf-8
import operator
from typing import Callable


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


Operation = tuple[str, Callable[[int, int], int]]


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

CONSTS = {
	INT32_MAX_00: 'INT32_MAX_00',
	INT32_MAX_M2: 'INT32_MAX_M2',
	INT32_MAX_M1: 'INT32_MAX_M1',
	INT32_MAX_P1: 'INT32_MAX_P1',
	INT32_MAX_P2: 'INT32_MAX_P2',
	INT64_MAX_00: 'INT64_MAX_00',
	INT64_MAX_M2: 'INT64_MAX_M2',
	INT64_MAX_M1: 'INT64_MAX_M1',
	INT64_MAX_P1: 'INT64_MAX_P1',
	INT64_MAX_P2: 'INT64_MAX_P2',

	UINT32_MAX_00: 'UINT32_MAX_00',
	UINT32_MAX_M2: 'UINT32_MAX_M2',
	UINT32_MAX_M1: 'UINT32_MAX_M1',
	UINT32_MAX_P1: 'UINT32_MAX_P1',
	UINT32_MAX_P2: 'UINT32_MAX_P2',
	UINT64_MAX_00: 'UINT64_MAX_00',
	UINT64_MAX_M2: 'UINT64_MAX_M2',
	UINT64_MAX_M1: 'UINT64_MAX_M1',
	UINT64_MAX_P1: 'UINT64_MAX_P1',
	UINT64_MAX_P2: 'UINT64_MAX_P2',
}

CONSTS2 = {value: key for key, value in CONSTS.items()}
assert len(CONSTS) == len(CONSTS2)


def to_str(value: int) -> str:
	return CONSTS.get(value, f'"{value}"')


def get_ValType(value: int) -> str:
	if value > UINT64_MAX_00:
		return 'VT::BIG'
	elif value > INT64_MAX_00:
		return 'VT::U64'
	elif value > UINT32_MAX_00:
		return 'VT::S64'
	elif value > INT32_MAX_00:
		return 'VT::U32'
	elif value >= 0:
		return 'VT::S32'
	elif value >= -INT32_MAX_00 - 1:
		return 'VT::N32'
	elif value >= -INT64_MAX_00 - 1:
		return 'VT::N64'
	else:
		return 'VT::BIG'


# def get_ValType(value: int) -> str:
# 	if value > UINT64_MAX_00 or value < (-INT32_MAX_00 - 1) :
# 		return 'VT::BIG'
# 	elif value > UINT32_MAX_00:
# 		return 'VT::U64'
# 	else:
# 		return 'VT::U32'


def make_Param(value: int) -> str:
	type_ = get_ValType(value)
	return f'Param{{{type_}, {to_str(value)}}}'


def make_ExpectedResult(left: int, right: int, operation: Callable[[int, int], int]) -> str:
	try:
		result = operation(left, right)
	except ZeroDivisionError:
		return 'IGNORED'
	else:
		return f'{to_str(result)}'


def make_ExpectedResults(left: int, right: int, operations: list[Operation]) -> str:
	results = '\n'.join([
		f'.{operation[0]}={make_ExpectedResult(left, right, operation[1])},'
		for operation in operations
	])
	return f'ExpectedResults{{\n{indent_multiline(results, tabs=2)}\n}}'


def make_BinOpTest(left: int, right: int, operations: list[Operation]) -> str:
	lparam = make_Param(left)
	rparam = make_Param(right)
	expected = make_ExpectedResults(left, right, operations)

	return f'BinOpTest{{{lparam}, {rparam}, {expected}}}'


def make_BinOpTest_combinations(left: int, right: int, operations: list[Operation]) -> list[str]:
	binOpTests = []
	binOpTests.append(f'{make_BinOpTest(left, right, operations)}')
	if left != 0:
		binOpTests.append(f'\n{make_BinOpTest(-left, right, operations)}')
	if right != 0:
		binOpTests.append(f'\n{make_BinOpTest(left, -right, operations)}')
	if left != 0 and right != 0:
		binOpTests.append(f'\n{make_BinOpTest(-left, -right, operations)}')
	return binOpTests


def make_ALL_TEST_VALUES(var_name: str, all_params: list[tuple[int, int]], operations: list[Operation]) -> list[str]:
	all_BinOpTests = [
		binOpTest
		for left, right in all_params
		for binOpTest in make_BinOpTest_combinations(left, right, operations)
	]
	return [
		'[[nodiscard]] std::vector<BinOpTest>',
		f'get_{var_name}() {{',
		f'{INDENT}std::vector<BinOpTest> {var_name};',
		'',
		*[indent_multiline(f'{var_name}.push_back({binOpTest});', tabs=1) for binOpTest in all_BinOpTests],
		'',
		f'{INDENT}return {var_name};',
		'};'
	]


def make_define(var_name: str, value: str) -> str:
	return f'#define {var_name} {value}'


def make_usefull_constants(constants: dict[str, int]) -> list[str]:
	return [
		make_define(name, f'"{value}"')
		for name, value in constants.items()
	]



BINARY_ARITHMETIC_OPERATIONS = [
	('add',  lambda a, b: a + b),
	('sub',  lambda a, b: a - b),
	('rsub', lambda a, b: b - a),
	('mul',  lambda a, b: a * b),
	('div',  lambda a, b: a // b),
	('rdiv', lambda a, b: b // a),
	('mod',  lambda a, b: a % b),
	('rmod', lambda a, b: b % a),
	# ('pow',  lambda a, b: a ** b),
	# ('rpow', lambda a, b: b ** a),
]


ALL_PARAMS = [
	(0, 0),
	(1, 0),
	(2, 0),
	(3, 0),
	(UINT32_MAX_M1, 0),
	(UINT32_MAX_00, 0),
	(UINT32_MAX_P1, 0),
	(UINT64_MAX_M1, 0),
	(UINT64_MAX_00, 0),
	(UINT64_MAX_P1, 0),
	(618354700061515834059999999999999999991, 0),
	(0, 1),
	(1, 1),
	(2, 1),
	(3, 1),
	(UINT32_MAX_M1, 1),
	(UINT32_MAX_00, 1),
	(UINT32_MAX_P1, 1),
	(UINT64_MAX_M1, 1),
	(UINT64_MAX_00, 1),
	(UINT64_MAX_P1, 1),
	(618354700061515834059999999999999999991, 1),
	(0, 5),
	(1, 5),
	(2, 5),
	(3, 5),
	(618354700061515834059999999999999999991, 5),
	(UINT32_MAX_00, UINT32_MAX_00),
	(618354700061515834059999999999999999991, 618354700061515834059999999799999999991),
	(4, 1),
	(UINT32_MAX_P2, 1),
	(UINT64_MAX_P2, 1),
	(10, 5),
	(11, 5),
	(12, 5),
	(13, 5),
	(UINT64_MAX_00, UINT32_MAX_00),
	(1236709400123031668119999999799999999982, 618354700061515834059999999799999999991),
	(UINT32_MAX_P1, UINT32_MAX_P1),
	(UINT32_MAX_P2, UINT32_MAX_P2),
	(UINT64_MAX_00, UINT64_MAX_00),
	(UINT64_MAX_P1, UINT64_MAX_P1),
	(UINT64_MAX_P1, UINT64_MAX_P2),
	(UINT64_MAX_P2, UINT64_MAX_P2),
	(14, 5),
	(15, 5),
	(97, 5),
	(18446744065119617025, UINT32_MAX_00),
	(UINT64_MAX_P1, UINT32_MAX_P1),
	(18446744082299486209, UINT32_MAX_P2),
	(340282366920938463426481119284349108225, UINT64_MAX_00),
	(340282366920938463463374607431768211456, UINT64_MAX_P1),
	(340282366920938463481821351505477763072, UINT64_MAX_P2),
	(340282366920938463500268095579187314689, UINT64_MAX_P2),
	(340282366920938463463374607431768211455, UINT64_MAX_P2), # eqivalent to 99, 11. Tests carry overflow
	(340282366920938463463374607431768211456, 340282366920938463463374607431768211455), # eqivalent to 100, 99. Tests carry underflow
	(382362535088167210234626361716426516060166448586892714986920000001800000000081, 618354700061515834059999999799999999991),
	(85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457  # 3rd Generalized repunit prime for a=7
	, 531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889),  # 2^107 - 1, a Mersenne prime.
	(85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457  # 3rd Generalized repunit prime for a=7
	, 9),
]


def make_values_for_test_h() -> str:
	lines = [
		'#ifndef VALUES_FOR_TEST_H',
		'#define VALUES_FOR_TEST_H',
		'',
		'#include "utils_for_test.h"',
		'#include <vector>',
		'',
		'namespace test_data {',
		'',
		'using namespace test_utils;',
		'',
		'// usefull constants',
		*make_usefull_constants(CONSTS2),
		'',
		'}',
		'',
		'',
		'namespace test_data {',
		'',
		*make_ALL_TEST_VALUES(
			'all_test_values',
			ALL_PARAMS,
			BINARY_ARITHMETIC_OPERATIONS
		),
		'',
		'}',
		'',
		'#endif // VALUES_FOR_TEST_H',
		'',
	]
	return '\n'.join(lines)


if __name__ == "__main__":
	values_for_test_h = make_values_for_test_h()
	# print(values_for_test_h)

	with open('./values_for_test.h', 'w') as file:
		file.write(values_for_test_h)





















