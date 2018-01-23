from xlrd import open_workbook

wb = open_workbook('CPU.xlsx')
sheet = wb.sheet_by_name('Opcodes')

number_of_rows = sheet.nrows
number_of_columns = sheet.ncols

colIndices = {}

def cv(row, col):
	global colIndices
	return sheet.cell(row, colIndices[col]).value

for col in range(0, number_of_columns):
	colIndices[sheet.cell(0, col).value] = col

instructions_py = []
instructions = []
itypes = []

INDENT = '	'

lastopid = -1
for row in range(1, number_of_rows):
	category = cv(row, 'Category')
	if category:
		categoryx = '%s# %s' % (INDENT, category)
		instructions_py.append(categoryx)
		categoryx = '%s// %s' % (INDENT, category)
		instructions.append(categoryx)
		itypes.append(categoryx)

	opid = int(cv(row, 'OP'))
	instructions_py.append('%s"%s": OpCode(%d, IT_%s, "%s"),' % (INDENT, cv(row, 'Name'), opid, cv(row, 'Type'), cv(row, 'Name')))
	lastopid += 1
	if opid == 0 or lastopid < opid:
		instructions.append('%sI_%s = %s,' % (INDENT, cv(row, 'Name'), opid))
	else:
		instructions.append('%sI_%s,' % (INDENT, cv(row, 'Name')))
	while lastopid < opid:
		itypes.append('%sIT_INVALID,' % INDENT)
		lastopid += 1
	itypes.append('%sIT_%s,' % (INDENT, cv(row, 'Type')))



f = open("../asm/opcodes.py", "w")
f.write('''
IT_RRVV = 0
IT_N = 1
IT_RRVV64 = 2
IT_INVALID = 3
IT_VIRTUAL = 4
IT_V8 = 5
IT_V8V8 = 6

class OpCode:
	def __init__(self, i, itype, iname):
		self.i = i
		self.type = itype
		self.name = iname


OPCODES = {
''' + '\n'.join(instructions_py) + '''
	"REM": OpCode(-1, IT_VIRTUAL, "REM"),
	"DB": OpCode(-1, IT_VIRTUAL, "DB"),
	"__ENABLE_ENC": OpCode(-1, IT_VIRTUAL, "__ENABLE_ENC"),
	"__DISABLE_ENC": OpCode(-1, IT_VIRTUAL, "__DISABLE_ENC"),
}
''')
f.close()

f = open("../opcodes.h", "w")
f.write('''
#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum INSTRUCTION {
''' + '\n'.join(instructions) + '''
};

#define II_MAX (''' + str(lastopid) + ''')

enum ITYPE {
	IT_RRVV = 0,
	IT_N,
	IT_RRVV64,
	IT_V8,
	IT_V8V8,
	IT_INVALID
};

static uint8_t ITYPES[] = {
''' + '\n'.join(itypes) + '''
};

#endif // OPCODES_H_INCLUDED
''')
f.close()
