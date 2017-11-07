
IT_RRVV = 0
IT_N = 1
IT_RRVV32 = 2
IT_INVALID = 3
IT_VIRTUAL = 4

class OpCode:
	def __init__(self, i, itype):
		self.i = i
		self.type = itype


OPCODES = {
    # Basic
    "MOV": OpCode(0, IT_RRVV),
    "MOV32": OpCode(1, IT_RRVV32),
    "PUSH": OpCode(2, IT_RRVV),
    "POP": OpCode(3, IT_RRVV),
    "PUSH32": OpCode(4, IT_RRVV32),
    "POP32": OpCode(5, IT_RRVV32),
    "NOP": OpCode(6, IT_N),
    # Integer Arithmetic
    "ADD": OpCode(7, IT_RRVV),
    "SUB": OpCode(8, IT_RRVV),
    "MUL": OpCode(9, IT_RRVV),
    "DIV": OpCode(10, IT_RRVV),
    "MOD": OpCode(11, IT_RRVV),
    "SHL": OpCode(12, IT_RRVV),
    "LSHR": OpCode(13, IT_RRVV),
    "ASHR": OpCode(14, IT_RRVV),
    "MULU": OpCode(15, IT_RRVV),
    "DIVU": OpCode(16, IT_RRVV),
    # Logic
    "XOR": OpCode(17, IT_RRVV),
    "OR": OpCode(18, IT_RRVV),
    "AND": OpCode(19, IT_RRVV),
    "NOT": OpCode(20, IT_RRVV),
    "NOR": OpCode(21, IT_RRVV),
    "NAND": OpCode(22, IT_RRVV),
    # Compare
    "CMP": OpCode(23, IT_RRVV),
    "CMPS": OpCode(24, IT_RRVV),
    "CMP32": OpCode(25, IT_RRVV32),
    "CMP32S": OpCode(26, IT_RRVV32),
    "CMPF": OpCode(27, IT_RRVV32),
    # Flow
    "JMP": OpCode(28, IT_RRVV),
    "JGE": OpCode(29, IT_RRVV),
    "JLE": OpCode(30, IT_RRVV),
    "JE": OpCode(31, IT_RRVV),
    "JNE": OpCode(32, IT_RRVV),
    "JG": OpCode(33, IT_RRVV),
    "JL": OpCode(34, IT_RRVV),
    "JZ": OpCode(35, IT_RRVV),
    "JNZ": OpCode(36, IT_RRVV),
    "CALL": OpCode(37, IT_RRVV),
    "CGE": OpCode(38, IT_RRVV),
    "CLE": OpCode(39, IT_RRVV),
    "CE": OpCode(40, IT_RRVV),
    "CNE": OpCode(41, IT_RRVV),
    "CG": OpCode(42, IT_RRVV),
    "CL": OpCode(43, IT_RRVV),
    "CZ": OpCode(44, IT_RRVV),
    "CNZ": OpCode(45, IT_RRVV),
    "RETN": OpCode(46, IT_N),
    # Special
    "INT": OpCode(47, IT_RRVV),
    "SETIH": OpCode(48, IT_RRVV),
    "HALT": OpCode(49, IT_N),
    "RESET": OpCode(50, IT_N),
    "TRAP": OpCode(51, IT_N),
    "ENCOFF": OpCode(52, IT_N),
    "ENCON": OpCode(53, IT_N),
    "PUSHREG": OpCode(54, IT_N),
    "POPREG": OpCode(55, IT_N),
    "ENCRETN": OpCode(56, IT_N),
    # Integer Arithmetic 32bit
    "ADD32": OpCode(57, IT_RRVV32),
    "SUB32": OpCode(58, IT_RRVV32),
    "MUL32": OpCode(59, IT_RRVV32),
    "DIV32": OpCode(60, IT_RRVV32),
    "MOD32": OpCode(61, IT_RRVV32),
    "SHL32": OpCode(62, IT_RRVV32),
    "LSHR32": OpCode(63, IT_RRVV32),
    "ASHR32": OpCode(64, IT_RRVV32),
    "MULU32": OpCode(65, IT_RRVV32),
    "DIVU32": OpCode(66, IT_RRVV32),
    # Float Arithmetic 32bit
    "ADDF": OpCode(67, IT_RRVV32),
    "SUBF": OpCode(68, IT_RRVV32),
    "MULF": OpCode(69, IT_RRVV32),
    "DIVF": OpCode(70, IT_RRVV32),
    "MOV8": OpCode(71, IT_RRVV),
    "REM": OpCode(-1, IT_VIRTUAL),
}
