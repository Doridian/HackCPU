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
	"FLAGR":   9,
	"ENCREG1": 10,
	"ENCREG2": 11,
	"IHBASE":  12,

	"CREG":    13,
	"MREG":    14,
	"MREGC":   15,

	"R12":     0,
	"R34":     2,
	"R56":     4,
	"ENCREG":  10,
}
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
		elif src[0] == "[":
			if src[-1] != "]":
				raise ValueError("Missing ] after [")

			if src[1] == ":":
				self.rval = REGISTERS["MREGC"]
				self.cval = src[2:-1]
			elif src[1:-1] in REGISTERS:
				self.rval = REGISTERS["MREG"]
				self.cval = REGISTERS[src[1:-1]].to_bytes(1, BYTEORDER)
			else:
				self.rval = REGISTERS["MREGC"]
				# Convert to binary
				self.cval = int(src[1:-1], 0)
		elif src[0] == ":":
			self.rval = REGISTERS["CREG"]
			self.cval = src[1:]
		elif src in REGISTERS:
			# Convert to binary
			self.rval = REGISTERS[src]
			self.cval = None
		else:
			self.rval = REGISTERS["CREG"]
			# Check if 64-bit mode and convert as number to binary
			self.cval = int(src, 0)

	def len(self, b64):
		if self.cval == None:
			return 0
		if isinstance(self.cval, bytes):
			return len(self.cval)
		if self.rval == REGISTERS["CREG"] and b64:
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
			if self.rval == REGISTERS["CREG"] and b64:
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
		if self.opcode.name == "REM":
			return 0

		if self.opcode.name == "DB":
			return self.params[0].len(self.b64)

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			mylen += self.params[i].len(self.b64)

		return mylen

	def write(self):
		global enckey
		global out_f

		if self.opcode.name == "REM":
			if self.params and len(self.params) > 0:
				if self.params[0] == ":ENABLE_ENC":
					enckey = int(self.params[1], 0).to_bytes(8, BYTEORDER)
				elif self.params[0] == ":DISABLE_ENC":
					enckey = None
			return

		if self.opcode.name == "DB":
			cval = self.params[0].getcval(self.b64)
			if cval == None:
				return
			out_f.write(cval)
			return

		if self.opcode.type == IT_VIRTUAL:
			return

		encwrite(self.opcode.i.to_bytes(1, BYTEORDER))

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
	lsplit = line.split(" ")

	if line[0] == ":":
		insn = Instruction(OPCODES["REM"], lsplit)
		labels[line[1:]] = insn
	elif line[0] == "#":
		doenc = len(lsplit) > 1
		long_enckkey = int(0)

		if lsplit[0] == "#ROM":
			if doenc:
				long_enckkey = int(lsplit[1], 0)
			baseaddr = -1
			b = (long_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
			out_f.write(b)
			instructions.append(Instruction(OPCODES["REM"], [":ENABLE_ENC", str(long_enckkey)]))
		elif lsplit[0] == "#BOOTLOADER":
			if doenc:
				long_enckkey = int(lsplit[1], 0)
			baseaddr = 0
			if doenc:
				instructions.append(Instruction(OPCODES["NOP"]))
				instructions.append(Instruction(OPCODES["MOV64"], [Parameter("ENCREG"), Parameter(str(long_enckkey))]))
				instructions.append(Instruction(OPCODES["ENCON"]))
				instructions.append(Instruction(OPCODES["REM"], [":ENABLE_ENC", str(long_enckkey)]))
	else:
		opc = OPCODES[lsplit[0]]
		if opc.name == "DB":
			lbl = "db_" + lsplit[1]
			rawData = line[line.find(lsplit[1]) + len(lsplit[1]) + 1:]
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
			insn = Instruction(OPCODES[lsplit[0]], list(map(Parameter, lsplit[1:])))

	if insn != None:
		instructions.append(insn)

if baseaddr < 0:
	baseaddr = RAM_SIZE - bpos
	enccpos = baseaddr % 8

for insn in instructions:
	insn.write()

in_f.close()
out_f.close()
