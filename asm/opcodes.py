
IT_RRVV = 0
IT_N = 1
IT_RRVV64 = 2
IT_VIRTUAL = 3
IT_I8 = 4
IT_I8I8 = 5
IT_U8 = 6
IT_U8U8 = 7
IT_INVALID = 8

class OpCode:
	def __init__(self, i, itype, iname):
		self.i = i
		self.type = itype
		self.name = iname


OPCODES = {
	"MOV": OpCode(0, IT_RRVV, "MOV"),
	"MOV8": OpCode(1, IT_RRVV, "MOV8"),
	"MOV16": OpCode(2, IT_RRVV, "MOV16"),
	"PUSH": OpCode(3, IT_RRVV, "PUSH"),
	"POP": OpCode(4, IT_RRVV, "POP"),
	"ADD": OpCode(5, IT_RRVV, "ADD"),
	"SUB": OpCode(6, IT_RRVV, "SUB"),
	"MUL": OpCode(7, IT_RRVV, "MUL"),
	"DIV": OpCode(8, IT_RRVV, "DIV"),
	"MOD": OpCode(9, IT_RRVV, "MOD"),
	"SHL": OpCode(10, IT_RRVV, "SHL"),
	"LSHR": OpCode(11, IT_RRVV, "LSHR"),
	"ASHR": OpCode(12, IT_RRVV, "ASHR"),
	"MULU": OpCode(13, IT_RRVV, "MULU"),
	"DIVU": OpCode(14, IT_RRVV, "DIVU"),
	"XOR": OpCode(15, IT_RRVV, "XOR"),
	"OR": OpCode(16, IT_RRVV, "OR"),
	"AND": OpCode(17, IT_RRVV, "AND"),
	"NOT": OpCode(18, IT_RRVV, "NOT"),
	"NOR": OpCode(19, IT_RRVV, "NOR"),
	"NAND": OpCode(20, IT_RRVV, "NAND"),
	"CMP": OpCode(21, IT_RRVV, "CMP"),
	"CMPS": OpCode(22, IT_RRVV, "CMPS"),
	"CMPF": OpCode(23, IT_RRVV, "CMPF"),
	"JMP": OpCode(24, IT_RRVV, "JMP"),
	"JGE": OpCode(25, IT_RRVV, "JGE"),
	"JLE": OpCode(26, IT_RRVV, "JLE"),
	"JE": OpCode(27, IT_RRVV, "JE"),
	"JNE": OpCode(28, IT_RRVV, "JNE"),
	"JG": OpCode(29, IT_RRVV, "JG"),
	"JL": OpCode(30, IT_RRVV, "JL"),
	"JZ": OpCode(31, IT_RRVV, "JZ"),
	"JNZ": OpCode(32, IT_RRVV, "JNZ"),
	"CALL": OpCode(33, IT_RRVV, "CALL"),
	"CGE": OpCode(34, IT_RRVV, "CGE"),
	"CLE": OpCode(35, IT_RRVV, "CLE"),
	"CE": OpCode(36, IT_RRVV, "CE"),
	"CNE": OpCode(37, IT_RRVV, "CNE"),
	"CG": OpCode(38, IT_RRVV, "CG"),
	"CL": OpCode(39, IT_RRVV, "CL"),
	"CZ": OpCode(40, IT_RRVV, "CZ"),
	"CNZ": OpCode(41, IT_RRVV, "CNZ"),
	"RETNA": OpCode(42, IT_RRVV, "RETNA"),
	"INT": OpCode(43, IT_RRVV, "INT"),
	"SETIH": OpCode(44, IT_RRVV, "SETIH"),
	"ADDF": OpCode(45, IT_RRVV, "ADDF"),
	"SUBF": OpCode(46, IT_RRVV, "SUBF"),
	"MULF": OpCode(47, IT_RRVV, "MULF"),
	"DIVF": OpCode(48, IT_RRVV, "DIVF"),
	"ADD64F": OpCode(49, IT_RRVV, "ADD64F"),
	"SUB64F": OpCode(50, IT_RRVV, "SUB64F"),
	"MUL64F": OpCode(51, IT_RRVV, "MUL64F"),
	"DIV64F": OpCode(52, IT_RRVV, "DIV64F"),
	"CPUID": OpCode(53, IT_RRVV, "CPUID"),
	"RAMSIZE": OpCode(54, IT_RRVV, "RAMSIZE"),
	"MOV64": OpCode(64, IT_RRVV64, "MOV64"),
	"PUSH64": OpCode(65, IT_RRVV64, "PUSH64"),
	"POP64": OpCode(66, IT_RRVV64, "POP64"),
	"CMP64": OpCode(67, IT_RRVV64, "CMP64"),
	"CMP64S": OpCode(68, IT_RRVV64, "CMP64S"),
	"CMP64F": OpCode(69, IT_RRVV64, "CMP64F"),
	"ADD64": OpCode(70, IT_RRVV64, "ADD64"),
	"SUB64": OpCode(71, IT_RRVV64, "SUB64"),
	"MUL64": OpCode(72, IT_RRVV64, "MUL64"),
	"DIV64": OpCode(73, IT_RRVV64, "DIV64"),
	"MOD64": OpCode(74, IT_RRVV64, "MOD64"),
	"SHL64": OpCode(75, IT_RRVV64, "SHL64"),
	"LSHR64": OpCode(76, IT_RRVV64, "LSHR64"),
	"ASHR64": OpCode(77, IT_RRVV64, "ASHR64"),
	"MULU64": OpCode(78, IT_RRVV64, "MULU64"),
	"DIVU64": OpCode(79, IT_RRVV64, "DIVU64"),
	"POPNIL": OpCode(128, IT_N, "POPNIL"),
	"POPNIL64": OpCode(129, IT_N, "POPNIL64"),
	"NOP": OpCode(130, IT_N, "NOP"),
	"RETN": OpCode(131, IT_N, "RETN"),
	"RETNAC": OpCode(132, IT_U8, "RETNAC"),
	"HALT": OpCode(133, IT_N, "HALT"),
	"RESET": OpCode(134, IT_N, "RESET"),
	"TRAP": OpCode(135, IT_N, "TRAP"),
	"ENCOFF": OpCode(136, IT_N, "ENCOFF"),
	"ENCON": OpCode(137, IT_N, "ENCON"),
	"PUSHREG": OpCode(138, IT_N, "PUSHREG"),
	"POPREG": OpCode(139, IT_N, "POPREG"),
	"DEBUG": OpCode(140, IT_N, "DEBUG"),
	# Virtual
	"REM": OpCode(-1, IT_VIRTUAL, "REM"),
	"DB": OpCode(-1, IT_VIRTUAL, "DB"),
	"DRET": OpCode(-1, IT_VIRTUAL, "DRET"),
	"MOVARG": OpCode(-1, IT_VIRTUAL, "MOVARG"),
	"__ENABLE_ENC": OpCode(-1, IT_VIRTUAL, "__ENABLE_ENC"),
	"__DISABLE_ENC": OpCode(-1, IT_VIRTUAL, "__DISABLE_ENC"),
}
