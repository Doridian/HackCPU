file = open('Opcodes.md', 'r')

def parseMDTableLine(line):
	return line.split('|')

columns = parseMDTableLine(file.readline())

colIndices = {}

def cv(row, col):
	global colIndices
	return row[colIndices[col]].strip()

for col in range(0, len(columns)):
	colIndices[columns[col].strip()] = col

instructions_py = []
instructions = []
itypes = []

INDENT = '	'

_b8 = 1 << 7
_b7 = 1 << 6

all_opcodes = []

while True:
	line = file.readline()
	if not line:
		break

	row = parseMDTableLine(line)
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

	opid = int(cv(row, 'OP'), 10)

	optypeid = (opid & (_b7 | _b8))
	if optypeid == 0:
		optype = 'RRVV'
	elif optypeid == _b7:
		optype = 'RRVV64'
	else:
		optype = 'Other'

	optype_doc = cv(row, 'Type')
	if optype != optype_doc and (optype_doc == 'RRVV' or optype_doc == 'RRVV64' or optype == 'RRVV' or optype == 'RRVV64'):
		raise ValueError('Opcode %s binary type would be %s, but is docoumented as %s' % (insn_name, optype, optype_doc))

	while opid >= len(all_opcodes):
		all_opcodes.append(None)

	if all_opcodes[opid]:
		raise ValueError('Duplicate OPCode: %d' % opid)

	all_opcodes[opid] = { 'Name': insn_name, 'Type': optype_doc }

file.close()

lastopid = -1
for opid in range(0, len(all_opcodes)):
	row = all_opcodes[opid]
	if not row:
		itypes.append('%sIT_INVALID,' % INDENT)
		continue

	insn_name = row['Name']
	insn_type = row['Type']

	#opid = int(cv(row, 'OP'))
	instructions_py.append('%s"%s": OpCode(%d, IT_%s, "%s"),' % (INDENT, insn_name, opid, insn_type, insn_name))
	if opid == 0 or opid != lastopid + 1:
		instructions.append('%sI_%s = %s,' % (INDENT, insn_name, opid))
	else:
		instructions.append('%sI_%s,' % (INDENT, insn_name))

	lastopid = opid

	itypes.append('%sIT_%s,' % (INDENT, insn_type))

f = open('../asm/opcodes.py', 'w')
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

f = open('../HackCPU/opcodes.h', 'w')
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
