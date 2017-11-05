from opcodes import *
from sys import argv
from math import ceil
from argparse import ArgumentParser

def auto_int(s):
	return int(s, 0)

parser = ArgumentParser()
parser.add_argument('input', help='Input file', type=str)
parser.add_argument('output', help='Output file', type=str)
parser.add_argument('--baseaddr', default=-1, type=auto_int, dest='baseaddr', help='Base address (0 for bootloader, omit for ROM)')
parser.add_argument('--romenc', default=None, type=auto_int, dest='romenc', help='Use ROM encryption key ROMENC')
args = parser.parse_args()

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

BYTEORDER = 'little'

baseaddr = args.baseaddr

enckey = None
if args.romenc != None:
	enckey = args.romenc.to_bytes(4, BYTEORDER)

bpos = 0
labels = {}
instructions = []

in_f = open(args.input, "r")
out_f = open(args.output, "wb")

class Parameter:
	def __init__(self, src):
		if src[0] == "@":
			if src[1] == ":":
				self.rval = REGISTERS["MREGC"]
				self.cval = src[2:]
			elif src[1] == "$":
				self.rval = REGISTERS["MREG"]
				self.cval = REGISTERS[src[2:]].to_bytes(1, BYTEORDER)
			else:
				self.rval = REGISTERS["MREGC"]
				# Convert to binary
				self.cval = int(src[1:], 0)
		elif src[0] == ":":
			self.rval = REGISTERS["CREG"]
			self.cval = src[1:]
		elif src[0] == "$":
			# Convert to binary
			self.rval = REGISTERS[src[1:]]
			self.cval = None
		else:
			self.rval = REGISTERS["CREG"]
			# Check if 32-bit mode and convert as number to binary
			self.cval = int(src, 0)

	def len(self, b32):
		if self.cval == None:
			return 0
		if isinstance(self.cval, bytes):
			return len(self.cval)
		if self.rval == REGISTERS["CREG"] and b32:
			return 4
		return 2

	def getcval(self, b32):
		if isinstance(self.cval, str):
			self.cval = labels[self.cval].bpos + baseaddr

		if isinstance(self.cval, int):
			if self.rval == REGISTERS["CREG"] and b32:
				return self.cval.to_bytes(4, BYTEORDER)
			return self.cval.to_bytes(2, BYTEORDER)

		return self.cval

enccpos = 0
def encwrite(bs):
	global enccpos
	global enckey
	global out_f

	if not enckey:
		out_f.write(bs)
		enccpos += len(bs)
		enccpos %= 4
		return

	for b in bs:
		out_f.write((b ^ enckey[enccpos]).to_bytes(1, BYTEORDER))
		enccpos = enccpos + 1
		if enccpos > 3:
			enccpos = 0

class Instruction:
	def __init__(self, opcode, params = []):
		global bpos
		self.opcode = opcode
		self.params = params
		self.bpos = bpos
		self.b32 = self.opcode.type == IT_RRVV32
		bpos += self.len()

	def len(self):
		if self.opcode.type == IT_VIRTUAL:
			return 0

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			mylen += self.params[i].len(self.b32)

		return mylen

	def write(self):
		global enckey

		if self.opcode.type == IT_VIRTUAL:
			if self.params and len(self.params) > 0:
				if self.params[0] == ":ENABLE_ENC":
					enckey = int(self.params[1], 0).to_bytes(4, 'little')
				elif self.params[0] == ":DISABLE_ENC":
					enckey = None
			return

		encwrite(self.opcode.i.to_bytes(1, BYTEORDER))

		for i in range(0, len(self.params), 2):
			subval = 0
			if i < len(self.params) - 1:
				subval = self.params[i + 1].rval
			encwrite((self.params[i].rval << 4 | subval).to_bytes(1, BYTEORDER))

		for i in range(0, len(self.params)):
			cval = self.params[i].getcval(self.b32)
			if cval == None:
				continue
			encwrite(cval)

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
	lsplit = line.split(' ')

	if line[0] == ":":
		insn = Instruction(OPCODES["REM"], lsplit)
		labels[line[1:]] = insn
	elif line[0] != "#":
		insn = Instruction(OPCODES[lsplit[0]], list(map(Parameter, lsplit[1:])))

	if insn != None:
		instructions.append(insn)

if baseaddr < 0:
	baseaddr = 0xFFFF - bpos
	enccpos = baseaddr % 4

if args.romenc != None:
	b = (args.romenc ^ 0xBEBADEFA).to_bytes(4, BYTEORDER)
	out_f.write(b)

for insn in instructions:
	insn.write()

in_f.close()
out_f.close()
