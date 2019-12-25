from instruction import Instruction
from opcode_defs import OPCODES
from defs import BYTEORDER

NOPCODE = OPCODES["NOP"]
NOPCODEB = NOPCODE.i.to_bytes(1, BYTEORDER)
REMCODE = OPCODES["REM"]

class AlignInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		self.transpiler = transpiler
		base = params[0].raw
		offset = params[1].raw
		if offset >= base:
			raise ValueError("Offset >= Base for align")
		cur_offset = transpiler.bpos % base
		if offset < cur_offset:
			offset += base
		self._len = offset - cur_offset
		Instruction.__init__(self, transpiler, opcode)

	def len(self):
		return self._len

	def write(self):
		for _ in range(0, self._len):
			self.transpiler.encwrite(NOPCODEB)

class DBInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)
    
	def len(self):
		return len(self.params[0].raw)

	def write(self):
		cval = self.params[0].raw
		_tmp_enckey = self.transpiler.enckey
		self.transpiler.enckey = None
		self.transpiler.encwrite(cval)
		self.transpiler.enckey = _tmp_enckey

class ChangeEncInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	def len(self):
		return 0

	def write(self):
		if len(self.params) < 1:
			self.transpiler.enckey = None
		else:
			self.transpiler.enckey = self.params[0].raw.to_bytes(8, BYTEORDER)

class REMInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	def len(self):
		return 0

	def write(self):
		pass
