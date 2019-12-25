from opcode_defs import IT_RRVV, IT_RRVV64, IT_N, IT_VIRTUAL, OPCODES
from defs import BYTEORDER
from math import ceil

class Instruction:
	def __init__(self, transpiler, opcode, params = []):
		self.transpiler = transpiler
		self.opcode = opcode
		self.params = params
		self.b64 = self.opcode.type == IT_RRVV64
		self.bpos = transpiler.bpos
		transpiler.bpos += self.len()

	def len(self):
		if self.opcode.name == "DB":
			return len(self.params[0].raw)

		if self.opcode.type == IT_VIRTUAL:
			return 0

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			param = self.params[i]
			mylen += param.len(self.b64)

		return mylen

	def write(self):
		if self.opcode.name == "__ENABLE_ENC":
			self.transpiler.enckey = self.params[0].raw.to_bytes(8, BYTEORDER)
		elif self.opcode.name == "__DISABLE_ENC":
			self.transpiler.enckey = None
		elif self.opcode.name == "DB":
			cval = self.params[0].raw
			if cval != None:
				_tmp_enckey = self.transpiler.enckey
				self.transpiler.enckey = None
				self.transpiler.encwrite(cval)
				self.transpiler.enckey = _tmp_enckey

		if self.opcode.type == IT_VIRTUAL:
			return

		self.transpiler.encwrite(self.opcode.i.to_bytes(1, BYTEORDER))

		plen = len(self.params)

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
				self.transpiler.encwrite((self.params[i].rval << 4 | subval).to_bytes(1, BYTEORDER))

			for i in range(0, len(self.params)):
				cval = self.params[i].getcval(self.b64)
				if cval == None:
					continue
				self.transpiler.encwrite(cval)
			return

		raise ValueError("No idea about this instruction's arguments")

NOPCODE = OPCODES["NOP"].i.to_bytes(1, BYTEORDER)

class AlignInstruction:
	def __init__(self, transpiler, base, offset):
		self.transpiler = transpiler
		if offset >= base:
			raise("Offset >= Base for align")
		self.bpos = transpiler.bpos
		cur_offset = self.bpos % base
		if offset < cur_offset:
			offset += base
		self._len = offset - cur_offset
		transpiler.bpos += self.len()

	def len(self):
		return self._len

	def write(self):
		for _ in range(0, self._len):
			self.transpiler.encwrite(NOPCODE)
