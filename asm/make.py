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

	"CSP":     6,
	"BSP":     7,
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

baseaddr = None
enckey = None

bpos = 0
labels = {}
instructions = []

in_f = open(args.input, "r")
out_f = open(args.output, "wb")

class Parameter:
	def __init__(self, src, raw=False):
		if raw:
			self.rval = None
			self.cval = src
			return
		src = src.strip()
		self.__parse(src)

	def __parse(self, src, mem=False):
		if src[0] == "[":
			if mem:
				raise ValueError("Nested [ are not allowed")
			if src[-1] != "]":
				raise ValueError("Missing ] after [")
			return self.__parse(src[1:-1], True)

		__creg = REG_CREG
		if mem:
			__creg = REG_MREGC

		srcsplit = src.split(" ")
		offset = 0
		if len(srcsplit) > 1:
			if len(srcsplit) % 2 != 1:
				raise ValueError("Invalid offset specifier length")
			for i in range(1, len(srcsplit), 2):
				if srcsplit[i] == "+":
					offset += int(srcsplit[i + 1], 0)
				elif srcsplit[i] == "-":
					offset -= int(srcsplit[i + 1], 0)
				else:
					raise ValueError("Invalid operator %s" % srcsplit[i])
			src = srcsplit[0]

		if src[0] == ":":
			self.rval = __creg
			self.cval = src[1:]
			self.cval_offset = offset
		elif src in REGISTERS:
			if mem:
				cval = REGISTERS[src]
				self.rval = REG_MREG
				if offset != 0:
					if offset < -128 or offset > 128:
						raise ValueError("This parameter only supports offsets within the inclusive range -128 to 128")
					if offset > 0:
						offset -= 1
					cval |= 0b10000
					self.cval = cval.to_bytes(1, BYTEORDER) + offset.to_bytes(1, BYTEORDER, signed=True)
				else:
					self.cval = cval.to_bytes(1, BYTEORDER)
			else:
				if offset != 0:
					raise ValueError("This parameter does not support offsets")
				self.rval = REGISTERS[src]
				self.cval = None
		else:
			try:
				self.rval = __creg
				self.cval = int(src, 0) + offset
			except:
				self.rval = __creg
				self.cval = src
				self.cval_offset = offset

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
				self.cval = lbl + self.cval_offset
			else:
				self.cval = labels[self.cval].bpos + baseaddr + self.cval_offset

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

		if self.opcode.type == IT_I8 or self.opcode.type == IT_I8I8 or self.opcode.type == IT_U8 or self.opcode.type == IT_U8U8:
			return 1 + len(self.params)

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			param = self.params[i]
			mylen += param.len(self.b64)

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
				_tmp_enckey = enckey
				enckey = None
				encwrite(cval)
				enckey = _tmp_enckey

		if self.opcode.type == IT_VIRTUAL:
			return

		encwrite(self.opcode.i.to_bytes(1, BYTEORDER))

		plen = len(self.params)

		if self.opcode.type == IT_I8 or self.opcode.type == IT_I8I8 or self.opcode.type == IT_U8 or self.opcode.type == IT_U8U8:
			if (self.opcode.type == IT_I8 or self.opcode.type == IT_U8) and plen != 1:
				raise ValueError("Instruction only expects one argument")
			if (self.opcode.type == IT_I8I8 or self.opcode.type == IT_U8U8) and plen != 2:
				raise ValueError("Instruction expects two arguments")

			for i in range(0, plen):
				encwrite(self.params[i].cval.to_bytes(1, BYTEORDER, signed=(self.opcode.type == IT_U8 or self.opcode.type == IT_U8U8)))
			return

		if self.opcode.type == IT_N:
			if plen != 0:
				raise ValueError("Instructions expects no arguments")
			return

		if self.opcode.type == IT_RRVV or self.opcode.type == IT_RRVV64:
			if plen > 2 or plen < 1:
				raise ValueError("Instruction expects either one or two arguments")

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
			return

		raise ValueError("No idea about this instruction's arguments")

NOPCODE = OPCODES["NOP"].i.to_bytes(1, BYTEORDER)

class AlignInstruction:
	def __init__(self, base, offset):
		global bpos
		if offset >= base:
			raise("Offset >= Base for align")
		self.bpos = bpos
		cur_offset = bpos % base
		if offset < cur_offset:
			offset += base
		self._len = offset - cur_offset
		bpos += self.len()

	def len(self):
		return self._len

	def write(self):
		global NOPCODE
		for i in range(0, self._len):
			encwrite(NOPCODE)

# params can be:
# :LABEL to refer to a label
# @ANY to refer to "thing at this address in RAM" (can be const or Register or Label)
# Reg to refer to a register
# So you can do @R1 to "RAM value at address value of R1"

currentLine = ""

def parse():
	global currentLine
	global baseaddr
	global enccpos
	global labels
	global instructions

	suffix = None

	for line in in_f:
		line = line.strip()

		if len(line) < 1:
			continue

		lineCommentPos = line.find(";")
		if lineCommentPos > 0:
			line = line[:lineCommentPos]

		if len(line) < 1:
			continue

		currentLine = line

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
		elif opcodeName[0] == "#" or opcodeName[0] == ";":
			doenc = len(lsplit) > 0
			int_enckkey = int(0)
			if opcodeName == "#ROM":
				if doenc:
					int_enckkey = int(lsplit[0], 0)
				baseaddr = 0
				suffix = (int_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
				instructions.append(Instruction(OPCODES["__ENABLE_ENC"], [str(int_enckkey)]))
			elif opcodeName == "#BOOTLOADER":
				if doenc:
					int_enckkey = int(lsplit[0], 0)

				bootloader_offset = int(lsplit[1], 0)
				if bootloader_offset > 0:
					baseaddr = bootloader_offset
				else:
					baseaddr = 0
				suffix = bootloader_offset.to_bytes(4, BYTEORDER, signed=True)
				if doenc:
					instructions.append(Instruction(OPCODES["NOP"]))
					instructions.append(Instruction(OPCODES["MOV64"], [Parameter("ENCREG"), Parameter(str(int_enckkey))]))
					instructions.append(Instruction(OPCODES["ENCON"]))
					instructions.append(Instruction(OPCODES["__ENABLE_ENC"], [str(int_enckkey)]))
			elif opcodeName == "#ALIGN":
				instructions.append(AlignInstruction(int(lsplit[0], 0), int(lsplit[1], 0)))
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
			elif opc.name == "DRET":
				useopc = None
				if len(lsplit) == 0:
					useopc = "RETN"
				elif len(lsplit) == 1:
					try:
						x = int(lsplit[0], 10) * 4
						if x > 0xFF:
							raise ValueError("Too large for RETNAC")
						lsplit[0] = "%d" % x
						useopc = "RETNAC"
					except:
						useopc = "RETNA"
				else:
					raise ValueError("DRET only supports 0 or 1 parameter")
				insn = Instruction(OPCODES[useopc], list(map(Parameter, lsplit)))
			elif opc.name == "MOVARG":
				p1 = Parameter(lsplit[0])
				argno = int(lsplit[1], 10)
				if argno < 1:
					raise ValueError("MOVARG argno must be at least 1")
				p2 = Parameter("[BSP + %d]" % ((argno * 4) + 4))
				insn = Instruction(OPCODES["MOV"], [p1, p2])
			else:
				insn = Instruction(opc, list(map(Parameter, lsplit)))

		if insn != None:
			instructions.append(insn)

	if baseaddr < 0:
		raise ValueError("baseaddr < 0")

	labels["BASEADDR"] = baseaddr
	insn = Instruction(OPCODES["REM"], [])
	instructions.append(insn)
	labels["ENDADDR"] = insn

	for insn in instructions:
		insn.write()

	if suffix != None:
		out_f.write(suffix)

	in_f.close()
	out_f.close()

try:
	parse()
except:
	print(currentLine)
	raise
