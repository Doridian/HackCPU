file = open('Opcodes.md')

def parseMDTableLine(line):
	split = line.split('|')
	return list(map(str.strip, split))

columns = parseMDTableLine(file.readline())

colIndices = {}

def cv(row, col):
	global colIndices
	return row[colIndices[col]]

for col in range(0, len(columns)):
	colIndices[columns[col]] = col

instructions_py = []
instructions = []
itypes = []

INDENT = '	'

lastopid = -1
while True:
	row = file.readline()
	if not row:
		break

	row = parseMDTableLine(row)
	insn_name = cv(row, 'Name')
	if not insn_name or insn_name == 'Name':
		continue

	is_dashonly = True
	for c in insn_name:
		if c != '-':
			is_dashonly = False
			break

	if is_dashonly:
		continue

	category = cv(row, 'Category')
	if category:
		categoryx = '%s# %s' % (INDENT, category)
		instructions_py.append(categoryx)
		categoryx = '%s// %s' % (INDENT, category)
		instructions.append(categoryx)
		itypes.append(categoryx)

	opid = int(cv(row, 'OP'))
	instructions_py.append('%s"%s": OpCode(%d, IT_%s, "%s"),' % (INDENT, insn_name, opid, cv(row, 'Type'), insn_name))
	lastopid += 1
	if opid == 0 or lastopid < opid:
		instructions.append('%sI_%s = %s,' % (INDENT, insn_name, opid))
	else:
		instructions.append('%sI_%s,' % (INDENT, insn_name))
	while lastopid < opid:
		itypes.append('%sIT_INVALID,' % INDENT)
		lastopid += 1
	itypes.append('%sIT_%s,' % (INDENT, cv(row, 'Type')))


f = open("../asm/opcodes.py", "w")
f.write('''
IT_RRVV = 0
IT_N = 1
IT_RRVV64 = 2
IT_VIRTUAL = 3
IT_I8 = 4
IT_I8I8 = 5
IT_U8 = 6
IT_U8U8 = 7
IT_INVALID = 8

class OpCode:
	def __init__(self, i, itype, iname):
		self.i = i
		self.type = itype
		self.name = iname


OPCODES = {
''' + '\n'.join(instructions_py) + '''
	# Virtual
	"REM": OpCode(-1, IT_VIRTUAL, "REM"),
	"DB": OpCode(-1, IT_VIRTUAL, "DB"),
	"DRET": OpCode(-1, IT_VIRTUAL, "DRET"),
	"MOVARG": OpCode(-1, IT_VIRTUAL, "MOVARG"),
	"__ENABLE_ENC": OpCode(-1, IT_VIRTUAL, "__ENABLE_ENC"),
	"__DISABLE_ENC": OpCode(-1, IT_VIRTUAL, "__DISABLE_ENC"),
}
''')
f.close()

f = open("../HackCPU/opcodes.h", "w")
f.write('''
#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum INSTRUCTION {
''' + '\n'.join(instructions) + '''
	I_FIRST_INVALID
};

enum ITYPE {
	IT_RRVV = 0,
	IT_N,
	IT_RRVV64,
	IT_I8,
	IT_I8I8,
	IT_U8,
	IT_U8U8,
	IT_INVALID
};

static uint8_t ITYPES[] = {
''' + '\n'.join(itypes) + '''
};

#endif // OPCODES_H_INCLUDED
''')
f.close()
