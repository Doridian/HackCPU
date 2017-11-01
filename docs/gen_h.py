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

instructions = []
itypes = []

INDENT = '    '

lastopid = -1
for row in range(1, number_of_rows):
	category = cv(row, 'Category')
	if category:
		category = '%s// %s' % (INDENT, category)
		instructions.append(category)
		itypes.append(category)

	opid = int(cv(row, 'OP'))
	lastopid += 1
	if opid == 0 or lastopid < opid:
		instructions.append('%sI_%s = %s,' % (INDENT, cv(row, 'Name'), opid))
	else:
		instructions.append('%sI_%s,' % (INDENT, cv(row, 'Name')))
	while lastopid < opid:
		itypes.append('%sIT_INVALID,' % INDENT)
		lastopid += 1
	itypes.append('%sIT_%s,' % (INDENT, cv(row, 'Type')))

print('''#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED

enum INSTRUCTION {
''' + '\n'.join(instructions) + '''
};

enum ITYPE {
    IT_RRVV = 0,
    IT_N,
    IT_RRVV32,
    IT_INVALID
};

static uint8_t ITYPES[] = {
''' + '\n'.join(itypes) + '''
};

#endif // OPCODES_H_INCLUDED
''')