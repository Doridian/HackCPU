from opcode_defs import IT_RRVV, IT_RRVV64, IT_N, IT_VIRTUAL, IT_INVALID, OPCODES
from parameter import Parameter
from defs import BYTEORDER
from math import ceil

class Instruction:
	def __init__(self, transpiler, opcode, params = []):
		self.transpiler = transpiler
		self.opcode = opcode
		self.params = params
		if self.opcode and self.opcode.type == IT_RRVV64:
			self.constlen = 8
		else:
			self.constlen = 4
		self.bpos = transpiler.bpos
		self._cache_len = None
		transpiler.bpos += self.len()

	# Return falsey for default behaviour (instantiate this class, etc), return truthy
	@staticmethod
	def handle(InstructionCtor, name, transpiler, rawParams):
		opcode = None
		if name in OPCODES:
			opcode = OPCODES[name]
		insn = InstructionCtor(transpiler, opcode, list(map(Parameter, rawParams)))
		transpiler.instructions.append(insn)
		return insn

	def len(self):
		if not self._cache_len:
			self._cache_len = self._len()
		return self._cache_len

	def _len(self):
		if not self.opcode or self.opcode.type == IT_VIRTUAL or self.opcode.type == IT_INVALID:
			raise ValueError("INVALID or unhandled VIRTUAL instruction: " + self.opcode.name)

		mylen = 1 + ceil(len(self.params) / 2)
		for i in range(0, len(self.params)):
			param = self.params[i]
			mylen += param.len(self.constlen)

		return mylen

	def write(self):
		if not self.opcode or self.opcode.type == IT_VIRTUAL or self.opcode.type == IT_INVALID:
			raise ValueError("INVALID or unhandled VIRTUAL instruction: " + self.opcode.name)

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
				cval = self.params[i].getcval(self.transpiler, self.constlen)
				if cval == None:
					continue
				self.transpiler.encwrite(cval)
			return

		raise ValueError("No idea about this instruction's arguments")
