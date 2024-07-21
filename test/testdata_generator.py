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


def to_str(value: int) -> str:
	return CONSTS.get(value, f'"{value}"')




def get_ValType(value: int) -> str:
	if value > UINT64_MAX_00:
		return 'VT::BIG'
	elif value > UINT32_MAX_00:
		return 'VT::U64'
	else:
		return 'VT::U32'


def make_Param(value: int) -> str:
	type_ = get_ValType(value)
	return f'Param{{{type_}, {to_str(value)}}}'


def make_ExpectedResult(left: int, right: int, operation: Callable[[int, int], int]) -> str:
	try:
		result = operation(left, right)
	except ZeroDivisionError:
		return 'IGNORED'
	else:
		if result < 0:
			return 'IGNORED'
		return f'{to_str(result)}'


def make_ExpectedResults(left: int, right: int, operations: list[Operation]) -> str:
	results = '\n'.join([
		f'.{operation[0]}={make_ExpectedResult(left, right, operation[1])},'
		for operation in operations
	])
	return f'ExpectedResults{{\n{indent_multiline(results, tabs=1)}\n}}'


def make_BinOpTest(left: int, right: int, operations: list[Operation]) -> str:
	lparam = make_Param(left)
	rparam = make_Param(right)
	expected = make_ExpectedResults(left, right, operations)

	return f'BinOpTest{{{lparam}, {rparam}, {expected}}}'


def make_ALL_TEST_VALUES(var_name: str, all_params: list[tuple[int, int]], operations: list[Operation]) -> str:
	all_BinOpTests = '\n'.join([
		f'{make_BinOpTest(left, right, operations)},'
		for left, right in all_params
	])
	all_BinOpTests_indented = indent_multiline(all_BinOpTests, tabs=1)
	return f'const static std::vector<BinOpTest> {var_name} = {{{{\n{all_BinOpTests_indented}\n}}}}'




BINARY_ARITHMETIC_OPERATIONS = [
	('add', operator.add),
	('sub', operator.sub),
	('mul', operator.mul),
	('div', operator.floordiv),
	('mod', operator.mod),
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
	(382362535088167210234626361716426516060166448586892714986920000001800000000081, 618354700061515834059999999799999999991),
	(85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457  # 3rd Generalized repunit prime for a=7
	, 531137992816767098689588206552468627329593117727031923199444138200403559860852242739162502265229285668889),  # 2^107 - 1, a Mersenne prime.
	(85053461164796801949539541639542805770666392330682673302530819774105141531698707146930307290253537320447270457  # 3rd Generalized repunit prime for a=7
	, 9),
]


if __name__ == "__main__":
	all_test_values = make_ALL_TEST_VALUES(
		'ALL_TEST_VALUES',
		ALL_PARAMS,
		BINARY_ARITHMETIC_OPERATIONS
	)
	print(all_test_values + ';')




