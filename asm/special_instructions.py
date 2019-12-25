from instruction import Instruction
from parameter import Parameter
from opcode_defs import OPCODES
from defs import BYTEORDER
from binascii import unhexlify

NOPCODEB = OPCODES["NOP"].i.to_bytes(1, BYTEORDER)

class AlignInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		self.transpiler = transpiler
		base = int(params[0].raw, 0)
		offset = int(params[1].raw, 0)
		if offset >= base:
			raise ValueError("Offset >= Base for align")
		cur_offset = transpiler.bpos % base
		if offset < cur_offset:
			offset += base
		self.align_len = offset - cur_offset
		Instruction.__init__(self, transpiler, opcode)

	def _len(self):
		return self.align_len

	def write(self):
		for _ in range(0, self.align_len):
			self.transpiler.encwrite(NOPCODEB)

class DBInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)
    
	def getval(self):
		param = self.params[0].raw
		if isinstance(param, str):
			return bytes(param, "ascii")
		return param

	def _len(self):
		return len(self.getval())

	@staticmethod
	def handle(InstructionCtor, name, transpiler, rawParams):
		lbl = "db_" + rawParams[0]
		insn = Instruction.handle(InstructionCtor, name, transpiler, [rawParams[1]])
		transpiler.emitLabel(lbl, insn)
		transpiler.emitLabel(lbl + "_len", insn.len())
		return insn

	def write(self):
		_tmp_enckey = self.transpiler.enckey
		self.transpiler.enckey = None
		self.transpiler.encwrite(self.getval())
		self.transpiler.enckey = _tmp_enckey

class ChangeEncInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	def _len(self):
		return 0

	def write(self):
		if len(self.params) < 1:
			self.transpiler.enckey = None
		else:
			self.transpiler.enckey = self.params[0].raw.to_bytes(8, BYTEORDER)

class REMInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	def _len(self):
		return 0

	def write(self):
		pass

class MOVARGInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	@staticmethod
	def handle(InstructionCtor, name, transpiler, rawParams):
		p1 = rawParams[0]
		argno = int(rawParams[1], 10)
		if argno < 1:
			raise ValueError("MOVARG argno must be at least 1")
		p2 = "[BSP + %d]" % ((argno * 4) + 4)
		return Instruction.handle(InstructionCtor, "MOV", transpiler, [p1, p2])

class DRETInstruction(Instruction):
	def __init__(self, transpiler, opcode, params = []):
		Instruction.__init__(self, transpiler, opcode, params)

	@staticmethod
	def handle(InstructionCtor, name, transpiler, rawParams):
		useopc = None
		outParams = []
		if len(rawParams) == 0:
			useopc = "RETN"
		elif len(rawParams) == 1:
			useopc = "RETNA"
			try :
				x = int(rawParams[0], 10) * 4
				if x == 0:
					useopc = "RETN"
					outParams = []
				elif x < 0:
					raise TypeError("DRET arg must be >= 0")
				else:
					outParams = ["%d" % x]
			except ValueError:
				pass
		else:
			raise ValueError("DRET only supports 0 or 1 parameter")
		return Instruction.handle(InstructionCtor, useopc, transpiler, outParams)
