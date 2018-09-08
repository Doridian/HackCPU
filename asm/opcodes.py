
IT_RRVV = 0
IT_N = 1
IT_RRVV64 = 2
IT_VIRTUAL = 3
IT_V8 = 4
IT_V8V8 = 5
IT_INVALID = 6

class OpCode:
	def __init__(self, i, itype, iname):
		self.i = i
		self.type = itype
		self.name = iname


OPCODES = {
	# Basic
	"MOV": OpCode(0, IT_RRVV, "MOV"),
	"MOV64": OpCode(1, IT_RRVV64, "MOV64"),
	"MOV8": OpCode(2, IT_RRVV, "MOV8"),
	"MOV16": OpCode(3, IT_RRVV, "MOV16"),
	"PUSH": OpCode(4, IT_RRVV, "PUSH"),
	"POP": OpCode(5, IT_RRVV, "POP"),
	"PUSH64": OpCode(6, IT_RRVV64, "PUSH64"),
	"POP64": OpCode(7, IT_RRVV64, "POP64"),
	"NOP": OpCode(8, IT_N, "NOP"),
	# Integer Arithmetic
	"ADD": OpCode(9, IT_RRVV, "ADD"),
	"SUB": OpCode(10, IT_RRVV, "SUB"),
	"MUL": OpCode(11, IT_RRVV, "MUL"),
	"DIV": OpCode(12, IT_RRVV, "DIV"),
	"MOD": OpCode(13, IT_RRVV, "MOD"),
	"SHL": OpCode(14, IT_RRVV, "SHL"),
	"LSHR": OpCode(15, IT_RRVV, "LSHR"),
	"ASHR": OpCode(16, IT_RRVV, "ASHR"),
	"MULU": OpCode(17, IT_RRVV, "MULU"),
	"DIVU": OpCode(18, IT_RRVV, "DIVU"),
	# Logic
	"XOR": OpCode(19, IT_RRVV, "XOR"),
	"OR": OpCode(20, IT_RRVV, "OR"),
	"AND": OpCode(21, IT_RRVV, "AND"),
	"NOT": OpCode(22, IT_RRVV, "NOT"),
	"NOR": OpCode(23, IT_RRVV, "NOR"),
	"NAND": OpCode(24, IT_RRVV, "NAND"),
	# Compare
	"CMP": OpCode(25, IT_RRVV, "CMP"),
	"CMPS": OpCode(26, IT_RRVV, "CMPS"),
	"CMP64": OpCode(27, IT_RRVV64, "CMP64"),
	"CMP64S": OpCode(28, IT_RRVV64, "CMP64S"),
	"CMPF": OpCode(29, IT_RRVV, "CMPF"),
	"CMP64F": OpCode(30, IT_RRVV64, "CMP64F"),
	# Flow
	"JMP": OpCode(31, IT_RRVV, "JMP"),
	"JGE": OpCode(32, IT_RRVV, "JGE"),
	"JLE": OpCode(33, IT_RRVV, "JLE"),
	"JE": OpCode(34, IT_RRVV, "JE"),
	"JNE": OpCode(35, IT_RRVV, "JNE"),
	"JG": OpCode(36, IT_RRVV, "JG"),
	"JL": OpCode(37, IT_RRVV, "JL"),
	"JZ": OpCode(38, IT_RRVV, "JZ"),
	"JNZ": OpCode(39, IT_RRVV, "JNZ"),
	"CALL": OpCode(40, IT_RRVV, "CALL"),
	"CGE": OpCode(41, IT_RRVV, "CGE"),
	"CLE": OpCode(42, IT_RRVV, "CLE"),
	"CE": OpCode(43, IT_RRVV, "CE"),
	"CNE": OpCode(44, IT_RRVV, "CNE"),
	"CG": OpCode(45, IT_RRVV, "CG"),
	"CL": OpCode(46, IT_RRVV, "CL"),
	"CZ": OpCode(47, IT_RRVV, "CZ"),
	"CNZ": OpCode(48, IT_RRVV, "CNZ"),
	"RETN": OpCode(49, IT_N, "RETN"),
	# Special
	"INT": OpCode(50, IT_RRVV, "INT"),
	"SETIH": OpCode(51, IT_RRVV, "SETIH"),
	"HALT": OpCode(52, IT_N, "HALT"),
	"RESET": OpCode(53, IT_N, "RESET"),
	"TRAP": OpCode(54, IT_N, "TRAP"),
	"ENCOFF": OpCode(55, IT_N, "ENCOFF"),
	"ENCON": OpCode(56, IT_N, "ENCON"),
	"PUSHREG": OpCode(57, IT_N, "PUSHREG"),
	"POPREG": OpCode(58, IT_N, "POPREG"),
	# Integer Arithmetic 64bit
	"ADD64": OpCode(59, IT_RRVV64, "ADD64"),
	"SUB64": OpCode(60, IT_RRVV64, "SUB64"),
	"MUL64": OpCode(61, IT_RRVV64, "MUL64"),
	"DIV64": OpCode(62, IT_RRVV64, "DIV64"),
	"MOD64": OpCode(63, IT_RRVV64, "MOD64"),
	"SHL64": OpCode(64, IT_RRVV64, "SHL64"),
	"LSHR64": OpCode(65, IT_RRVV64, "LSHR64"),
	"ASHR64": OpCode(66, IT_RRVV64, "ASHR64"),
	"MULU64": OpCode(67, IT_RRVV64, "MULU64"),
	"DIVU64": OpCode(68, IT_RRVV64, "DIVU64"),
	# Float Arithmetic 32bit
	"ADDF": OpCode(69, IT_RRVV, "ADDF"),
	"SUBF": OpCode(70, IT_RRVV, "SUBF"),
	"MULF": OpCode(71, IT_RRVV, "MULF"),
	"DIVF": OpCode(72, IT_RRVV, "DIVF"),
	# Float Arithmetic 64bit
	"ADD64F": OpCode(73, IT_RRVV, "ADD64F"),
	"SUB64F": OpCode(74, IT_RRVV, "SUB64F"),
	"MUL64F": OpCode(75, IT_RRVV, "MUL64F"),
	"DIV64F": OpCode(76, IT_RRVV, "DIV64F"),
	# Extra
	"DEBUG": OpCode(77, IT_N, "DEBUG"),
	"RETNA": OpCode(78, IT_RRVV, "RETNA"),
	"POPNIL": OpCode(79, IT_N, "POPNIL"),
	"POPNIL64": OpCode(80, IT_N, "POPNIL64"),
	"RETNAC": OpCode(81, IT_V8, "RETNAC"),
	# Virtual
	"REM": OpCode(-1, IT_VIRTUAL, "REM"),
	"DB": OpCode(-1, IT_VIRTUAL, "DB"),
	"__ENABLE_ENC": OpCode(-1, IT_VIRTUAL, "__ENABLE_ENC"),
	"__DISABLE_ENC": OpCode(-1, IT_VIRTUAL, "__DISABLE_ENC"),
}
