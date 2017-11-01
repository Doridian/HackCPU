from opcodes import *
from sys import argv

bpos = 0
labels = {}
instructions = []

in_f = open(argv[1], "r")
out_f = open(argv[2], "w")

class Parameter:
	def __init__(self, rval, cval):
		self.rval = rval
		self.cval = cval

	def len(self):
		return 1 + len(self.cval)

class Instruction:
	def __init__(self, opcode, params = []):
		global bpos
		self.opcode = opcode
		self.params = params
		self.bpos = bpos
		bpos += self.len()

	def len(self):
		if self.opcode.type == IT_VIRTUAL:
			return 0

		mylen = 1
		for i in range(0, len(self.params)):
			mylen += self.params[i].len()

		return mylen

	def write(self):
		global out_f
		if self.opcode.type == IT_VIRTUAL:
			return

		out_f.write(self.opcode.i)
		for i in range(0, len(self.params), 2):
			out_f.write(self.params[i].rval << 4 | self.params[i + 1].rval)
		for i in range(0, len(self.params)):
			out_f.write(self.params[i].cval)

# params can be:
# :LABEL to refer to a label
# @ANY to refer to "thing at this address in RAM" (can be const, Register or )
# $Reg to refer to a register
# So you can do @$R1 to "RAM value at address value of R1"

for line in in_f:
	line = line.strip()
	if len(line) < 1:
		continue

	insn = None

	if line[0] == ":":
		insn = Instruction(OPCODES["REM"])
		labels[line[1:]] = insn
	elif line[0] != "#":
		lsplit = line.split(' ')
		insn = Instruction(OPCODES[lsplit[0]], []) # PARAMS ARE TODO

	if insn != None:
		instructions.append(insn)

in_f.close()
out_f.close()
