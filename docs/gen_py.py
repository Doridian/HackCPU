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

INDENT = '    '

lastopid = -1
for row in range(1, number_of_rows):
	category = cv(row, 'Category')
	if category:
		category = '%s# %s' % (INDENT, category)
		instructions.append(category)

	opid = int(cv(row, 'OP'))
	instructions.append('%s"%s": {"i": %d, "type": IT_%s},' % (INDENT, cv(row, 'Name'), opid, cv(row, 'Type')))

print('''
IT_RRVV = 0
IT_N = 1
IT_RRVV32 = 2
IT_INVALID = 3

OPCODES = {
''' + '\n'.join(instructions) + '''
}
''')
