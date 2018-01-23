from opcodes import *
from sys import argv
from math import ceil
from argparse import ArgumentParser
from binascii import unhexlify

parser = ArgumentParser()
parser.add_argument("input", help="Input file", type=str)
parser.add_argument("output", help="Output file", type=str)
args = parser.parse_args()

REGISTERS = {
	"R1":      0,
	"R2":      1,
	"R3":      2,
	"R4":      3,
	"R5":      4,
	"R6":      5,

	"PSP":     6,
	"CSP":     7,
	"PC":      8,
	"MSR":     9,
	"ENCREG1": 10,
	"ENCREG2": 11,
	"IHBASE":  12,

	"R12":     0,
	"R34":     2,
	"R56":     4,
	"ENCREG":  10,
}
REG_CREG = 13
REG_MREG = 14
REG_MREGC = 15
BYTEORDER = "little"
RAM_SIZE = (1024 * 1024 * 4)
RAM_MAX_ADDR = RAM_SIZE - 1

baseaddr = None
enckey = None

bpos = 0
labels = {}
instructions = []

labels["RAM_SIZE"] = RAM_SIZE
labels["RAM_MAX_ADDR"] = RAM_MAX_ADDR

in_f = open(args.input, "r")
out_f = open(args.output, "wb")

class Parameter:
	def __init__(self, src, raw=False):
		if raw:
			self.cval = src
			return

		src = src.strip()

		if src[0] == "[":
			if src[-1] != "]":
				raise ValueError("Missing ] after [")

			if src[1] == ":":
				self.rval = REG_MREGC
				self.cval = src[2:-1]
			elif src[1:-1] in REGISTERS:
				self.rval = REG_MREG
				self.cval = REGISTERS[src[1:-1]].to_bytes(1, BYTEORDER)
			else:
				self.rval = REG_MREGC
				# Convert to binary
				self.cval = int(src[1:-1], 0)
		elif src[0] == ":":
			self.rval = REG_CREG
			self.cval = src[1:]
		elif src in REGISTERS:
			# Convert to binary
			self.rval = REGISTERS[src]
			self.cval = None
		else:
			self.rval = REG_CREG
			# Check if 64-bit mode and convert as number to binary
			self.cval = int(src, 0)

	def len(self, b64):
		if self.cval == None:
			return 0
		if isinstance(self.cval, bytes):
			return len(self.cval)
		if self.rval == REG_CREG and b64:
			return 8
		return 4

	def getcval(self, b64):
		if isinstance(self.cval, str):
			lbl = labels[self.cval]
			if isinstance(lbl, int):
				self.cval = lbl
			else:
				self.cval = labels[self.cval].bpos + baseaddr

		if isinstance(self.cval, int):
			if self.rval == REG_CREG and b64:
				return self.cval.to_bytes(8, BYTEORDER)
			return self.cval.to_bytes(4, BYTEORDER)

		return self.cval

enccpos = 0
def encwrite(bs):
	global enccpos
	global enckey
	global out_f

	if not enckey:
		out_f.write(bs)
		enccpos += len(bs)
		enccpos %= 8
		return

	for b in bs:
		out_f.write((b ^ enckey[enccpos]).to_bytes(1, BYTEORDER))
		enccpos = enccpos + 1
		if enccpos > 7:
			enccpos = 0

class Instruction:
	def __init__(self, opcode, params = []):
		global bpos
		self.opcode = opcode
		self.params = params
		self.bpos = bpos
		self.b64 = self.opcode.type == IT_RRVV64
		bpos += self.len()

	def len(self):
		if self.opcode.name == "DB":
			return self.params[0].len(self.b64)

		if self.opcode.type == IT_VIRTUAL:
			return 0

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			mylen += self.params[i].len(self.b64)

		return mylen

	def write(self):
		global enckey
		global out_f

		if self.opcode.name == "__ENABLE_ENC":
			enckey = int(self.params[0], 0).to_bytes(8, BYTEORDER)
		elif self.opcode.name == "__DISABLE_ENC":
			enckey = None
		elif self.opcode.name == "DB":
			cval = self.params[0].getcval(self.b64)
			if cval != None:
				out_f.write(cval)

		if self.opcode.type == IT_VIRTUAL:
			return

		encwrite(self.opcode.i.to_bytes(1, BYTEORDER))

		if self.opcode.type == IT_V8 or self.opcode.type == IT_V8V8:
			plen = len(self.params)
			if self.opcode.type == IT_V8 and plen != 1:
				raise ValueError("Instruction only expects one argument")
			if self.opcode.type == IT_V8V8 and plen != 2:
				raise ValueError("Instruction expect two arguments")

			for i in range(0, plen):
				encwrite(self.params[i].cval.to_bytes(1, BYTEORDER, signed=True))
			return

		for i in range(0, len(self.params), 2):
			subval = 0
			if i < len(self.params) - 1:
				subval = self.params[i + 1].rval
			encwrite((self.params[i].rval << 4 | subval).to_bytes(1, BYTEORDER))

		for i in range(0, len(self.params)):
			cval = self.params[i].getcval(self.b64)
			if cval == None:
				continue
			encwrite(cval)

# params can be:
# :LABEL to refer to a label
# @ANY to refer to "thing at this address in RAM" (can be const or Register or Label)
# Reg to refer to a register
# So you can do @R1 to "RAM value at address value of R1"

for line in in_f:
	line = line.strip()
	if len(line) < 1:
		continue

	insn = None
	lineSpacePos = line.find(" ")
	if lineSpacePos < 0:
		opcodeName = line
		lsplit = []
	else:
		opcodeName = line[0:lineSpacePos]
		lsplit = line[lineSpacePos+1:].strip().split(",")

	if opcodeName[0] == ":":
		insn = Instruction(OPCODES["REM"], [])
		instructions.append(insn)
		labels[opcodeName[1:]] = insn
	elif opcodeName[0] == "#":
		doenc = len(lsplit) > 0
		int_enckkey = int(0)
		if opcodeName == "#ROM":
			if doenc:
				int_enckkey = int(lsplit[0], 0)
			baseaddr = -1
			b = (int_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
			out_f.write(b)
			instructions.append(Instruction(OPCODES["__ENABLE_ENC"], [str(int_enckkey)]))
		elif opcodeName == "#BOOTLOADER":
			if doenc:
				int_enckkey = int(lsplit[0], 0)
			baseaddr = 0
			if doenc:
				instructions.append(Instruction(OPCODES["NOP"]))
				instructions.append(Instruction(OPCODES["MOV64"], [Parameter("ENCREG"), Parameter(str(int_enckkey))]))
				instructions.append(Instruction(OPCODES["ENCON"]))
				instructions.append(Instruction(OPCODES["__ENABLE_ENC"], [str(int_enckkey)]))
	else:
		opc = OPCODES[opcodeName]
		if opc.name == "DB":
			lbl = "db_" + lsplit[0]
			rawData = line[line.find(lsplit[0]) + len(lsplit[0]) + 1:].strip()
			if rawData[0] == '"':
				if rawData[-1] != '"' or len(rawData) < 2:
					raise ValueError("Missing closing quote in DB string")
				rawData = bytes(rawData[1:-1], "ascii")
			elif rawData[0:1] == "0x":
				rawData = unhexlify(rawData[2:])
			else:
				rawData = unhexlify(rawData.replace(" ", ""))
			lstr = Parameter(rawData, True)
			insn = Instruction(opc, [lstr])
			labels[lbl] = insn
			labels[lbl + "_len"] = lstr.len(False)
		else:
			insn = Instruction(opc, list(map(Parameter, lsplit)))

	if insn != None:
		instructions.append(insn)

if baseaddr < 0:
	baseaddr = RAM_SIZE - bpos
	enccpos = baseaddr % 8

for insn in instructions:
	insn.write()

in_f.close()
out_f.close()
