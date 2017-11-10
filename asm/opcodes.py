
IT_RRVV = 0
IT_N = 1
IT_RRVV32 = 2
IT_INVALID = 3
IT_VIRTUAL = 4

class OpCode:
	def __init__(self, i, itype, iname):
		self.i = i
		self.type = itype
		self.name = iname


OPCODES = {
    # Basic
    "MOV": OpCode(0, IT_RRVV, "MOV"),
    "MOV32": OpCode(1, IT_RRVV32, "MOV32"),
    "PUSH": OpCode(2, IT_RRVV, "PUSH"),
    "POP": OpCode(3, IT_RRVV, "POP"),
    "PUSH32": OpCode(4, IT_RRVV32, "PUSH32"),
    "POP32": OpCode(5, IT_RRVV32, "POP32"),
    "NOP": OpCode(6, IT_N, "NOP"),
    # Integer Arithmetic
    "ADD": OpCode(7, IT_RRVV, "ADD"),
    "SUB": OpCode(8, IT_RRVV, "SUB"),
    "MUL": OpCode(9, IT_RRVV, "MUL"),
    "DIV": OpCode(10, IT_RRVV, "DIV"),
    "MOD": OpCode(11, IT_RRVV, "MOD"),
    "SHL": OpCode(12, IT_RRVV, "SHL"),
    "LSHR": OpCode(13, IT_RRVV, "LSHR"),
    "ASHR": OpCode(14, IT_RRVV, "ASHR"),
    "MULU": OpCode(15, IT_RRVV, "MULU"),
    "DIVU": OpCode(16, IT_RRVV, "DIVU"),
    # Logic
    "XOR": OpCode(17, IT_RRVV, "XOR"),
    "OR": OpCode(18, IT_RRVV, "OR"),
    "AND": OpCode(19, IT_RRVV, "AND"),
    "NOT": OpCode(20, IT_RRVV, "NOT"),
    "NOR": OpCode(21, IT_RRVV, "NOR"),
    "NAND": OpCode(22, IT_RRVV, "NAND"),
    # Compare
    "CMP": OpCode(23, IT_RRVV, "CMP"),
    "CMPS": OpCode(24, IT_RRVV, "CMPS"),
    "CMP32": OpCode(25, IT_RRVV32, "CMP32"),
    "CMP32S": OpCode(26, IT_RRVV32, "CMP32S"),
    "CMPF": OpCode(27, IT_RRVV32, "CMPF"),
    # Flow
    "JMP": OpCode(28, IT_RRVV, "JMP"),
    "JGE": OpCode(29, IT_RRVV, "JGE"),
    "JLE": OpCode(30, IT_RRVV, "JLE"),
    "JE": OpCode(31, IT_RRVV, "JE"),
    "JNE": OpCode(32, IT_RRVV, "JNE"),
    "JG": OpCode(33, IT_RRVV, "JG"),
    "JL": OpCode(34, IT_RRVV, "JL"),
    "JZ": OpCode(35, IT_RRVV, "JZ"),
    "JNZ": OpCode(36, IT_RRVV, "JNZ"),
    "CALL": OpCode(37, IT_RRVV, "CALL"),
    "CGE": OpCode(38, IT_RRVV, "CGE"),
    "CLE": OpCode(39, IT_RRVV, "CLE"),
    "CE": OpCode(40, IT_RRVV, "CE"),
    "CNE": OpCode(41, IT_RRVV, "CNE"),
    "CG": OpCode(42, IT_RRVV, "CG"),
    "CL": OpCode(43, IT_RRVV, "CL"),
    "CZ": OpCode(44, IT_RRVV, "CZ"),
    "CNZ": OpCode(45, IT_RRVV, "CNZ"),
    "RETN": OpCode(46, IT_N, "RETN"),
    # Special
    "INT": OpCode(47, IT_RRVV, "INT"),
    "SETIH": OpCode(48, IT_RRVV, "SETIH"),
    "HALT": OpCode(49, IT_N, "HALT"),
    "RESET": OpCode(50, IT_N, "RESET"),
    "TRAP": OpCode(51, IT_N, "TRAP"),
    "ENCOFF": OpCode(52, IT_N, "ENCOFF"),
    "ENCON": OpCode(53, IT_N, "ENCON"),
    "PUSHREG": OpCode(54, IT_N, "PUSHREG"),
    "POPREG": OpCode(55, IT_N, "POPREG"),
    "ENCRETN": OpCode(56, IT_N, "ENCRETN"),
    # Integer Arithmetic 32bit
    "ADD32": OpCode(57, IT_RRVV32, "ADD32"),
    "SUB32": OpCode(58, IT_RRVV32, "SUB32"),
    "MUL32": OpCode(59, IT_RRVV32, "MUL32"),
    "DIV32": OpCode(60, IT_RRVV32, "DIV32"),
    "MOD32": OpCode(61, IT_RRVV32, "MOD32"),
    "SHL32": OpCode(62, IT_RRVV32, "SHL32"),
    "LSHR32": OpCode(63, IT_RRVV32, "LSHR32"),
    "ASHR32": OpCode(64, IT_RRVV32, "ASHR32"),
    "MULU32": OpCode(65, IT_RRVV32, "MULU32"),
    "DIVU32": OpCode(66, IT_RRVV32, "DIVU32"),
    # Float Arithmetic 32bit
    "ADDF": OpCode(67, IT_RRVV32, "ADDF"),
    "SUBF": OpCode(68, IT_RRVV32, "SUBF"),
    "MULF": OpCode(69, IT_RRVV32, "MULF"),
    "DIVF": OpCode(70, IT_RRVV32, "DIVF"),
    "MOV8": OpCode(71, IT_RRVV, "MOV8"),
    "REM": OpCode(-1, IT_VIRTUAL, "REM"),
    "STR": OpCode(-1, IT_VIRTUAL, "STR"),
}
