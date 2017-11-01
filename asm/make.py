from opcodes import *
from sys import argv

REGISTERS = {
	"R1":      0,
	"R2":      1,
	"R3":      2,
	"R4":      3,
	"SP":      4,
	"PC":      5,
	"ENCREG1": 6,
	"ENCREG2": 7,
	"FLAGR":   8,
	"IHBASE":  9,

	"CREG":    13,
	"MREG":    14,
	"MREGC":   15,

	"R12":     0,
	"R34":     2,
	"ENCREG":  6,
}

baseaddr = len(argv) > 3 and int(argv[3]) or -1

bpos = 0
labels = {}
instructions = []

in_f = open(argv[1], "r")
out_f = open(argv[2], "w")

class Parameter:
	def __init__(self, src, b32 = False):
		if src[0] == "@":
			if src[1] == ":":
				self.rval = REGISTERS["MREGC"]
				self.cval = src[2:]
			elif src[1] == "$":
				self.rval = REGISTERS["MREG"]
				self.cval = REGISTERS[src[2:]]
			else:
				self.rval = REGISTERS["MREGC"]
				# Convert to binary
				self.cval = int(src)
		elif src[0] == ":":
			self.rval = REGISTERS["CREG"]
			self.cval = src[1:]
		elif src[0] == "$":
			# Convert to binary
			self.rval = REGISTERS[src[1:]]
			self.cval = []
		else:
			self.rval = REGISTERS["CREG"]
			# Check if 32-bit mode and convert as number to binary
			self.cval = int(src)

	def len(self):
		return 1 + len(self.cval)

	def getcval(self):
		if type(self.cval) == "string":
			# Convert to binary
			return labels[self.cval].bpos + baseaddr
		return self.cval

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

		#out_f.write(self.opcode.i)
		for i in range(0, len(self.params), 2):
			#out_f.write(self.params[i].rval << 4 | self.params[i + 1].rval)
			continue
		for i in range(0, len(self.params)):
			#out_f.write(self.params[i].getcval())
			continue

# params can be:
# :LABEL to refer to a label
# @ANY to refer to "thing at this address in RAM" (can be const or Register or Label)
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
		insn = Instruction(OPCODES[lsplit[0]], list(map(Parameter, lsplit[1:])))

	if insn != None:
		instructions.append(insn)

if baseaddr < 0:
	baseaddr = 0xFFFF - bpos

for insn in instructions:
	print(insn.bpos)
	insn.write()

in_f.close()
out_f.close()
