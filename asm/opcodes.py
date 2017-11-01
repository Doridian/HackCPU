
IT_RRVV = 0
IT_N = 1
IT_RRVV32 = 2
IT_INVALID = 3

OPCODES = {
    # Basic
    "MOV": {"i": 0, "type": IT_RRVV},
    "MOV32": {"i": 1, "type": IT_RRVV32},
    "PUSH": {"i": 2, "type": IT_RRVV},
    "POP": {"i": 3, "type": IT_RRVV},
    "PUSH32": {"i": 4, "type": IT_RRVV32},
    "POP32": {"i": 5, "type": IT_RRVV32},
    "NOP": {"i": 6, "type": IT_N},
    # Integer Arithmetic
    "ADD": {"i": 7, "type": IT_RRVV},
    "SUB": {"i": 8, "type": IT_RRVV},
    "MUL": {"i": 9, "type": IT_RRVV},
    "DIV": {"i": 10, "type": IT_RRVV},
    "MOD": {"i": 11, "type": IT_RRVV},
    "SHL": {"i": 12, "type": IT_RRVV},
    "LSHR": {"i": 13, "type": IT_RRVV},
    "ASHR": {"i": 14, "type": IT_RRVV},
    "MULU": {"i": 15, "type": IT_RRVV},
    "DIVU": {"i": 16, "type": IT_RRVV},
    # Logic
    "XOR": {"i": 17, "type": IT_RRVV},
    "OR": {"i": 18, "type": IT_RRVV},
    "AND": {"i": 19, "type": IT_RRVV},
    "NOT": {"i": 20, "type": IT_RRVV},
    "NOR": {"i": 21, "type": IT_RRVV},
    "NAND": {"i": 22, "type": IT_RRVV},
    # Compare
    "CMP": {"i": 23, "type": IT_RRVV},
    "CMPS": {"i": 24, "type": IT_RRVV},
    "CMP32": {"i": 25, "type": IT_RRVV32},
    "CMP32S": {"i": 26, "type": IT_RRVV32},
    "CMPF": {"i": 27, "type": IT_RRVV32},
    # Flow
    "JMP": {"i": 28, "type": IT_RRVV},
    "JGE": {"i": 29, "type": IT_RRVV},
    "JLE": {"i": 30, "type": IT_RRVV},
    "JE": {"i": 31, "type": IT_RRVV},
    "JNE": {"i": 32, "type": IT_RRVV},
    "JG": {"i": 33, "type": IT_RRVV},
    "JL": {"i": 34, "type": IT_RRVV},
    "JZ": {"i": 35, "type": IT_RRVV},
    "JNZ": {"i": 36, "type": IT_RRVV},
    "CALL": {"i": 37, "type": IT_RRVV},
    "CGE": {"i": 38, "type": IT_RRVV},
    "CLE": {"i": 39, "type": IT_RRVV},
    "CE": {"i": 40, "type": IT_RRVV},
    "CNE": {"i": 41, "type": IT_RRVV},
    "CG": {"i": 42, "type": IT_RRVV},
    "CL": {"i": 43, "type": IT_RRVV},
    "CZ": {"i": 44, "type": IT_RRVV},
    "CNZ": {"i": 45, "type": IT_RRVV},
    "RETN": {"i": 46, "type": IT_N},
    # Special
    "INT": {"i": 47, "type": IT_RRVV},
    "SETIH": {"i": 48, "type": IT_RRVV},
    "HALT": {"i": 49, "type": IT_N},
    "RESET": {"i": 50, "type": IT_N},
    "TRAP": {"i": 51, "type": IT_N},
    "ENCOFF": {"i": 52, "type": IT_N},
    "ENCON": {"i": 53, "type": IT_N},
    "PUSHREG": {"i": 54, "type": IT_N},
    "POPREG": {"i": 55, "type": IT_N},
    "ENCRETN": {"i": 56, "type": IT_N},
    # Integer Arithmetic 32bit
    "ADD32": {"i": 57, "type": IT_RRVV32},
    "SUB32": {"i": 58, "type": IT_RRVV32},
    "MUL32": {"i": 59, "type": IT_RRVV32},
    "DIV32": {"i": 60, "type": IT_RRVV32},
    "MOD32": {"i": 61, "type": IT_RRVV32},
    "SHL32": {"i": 62, "type": IT_RRVV32},
    "LSHR32": {"i": 63, "type": IT_RRVV32},
    "ASHR32": {"i": 64, "type": IT_RRVV32},
    "MULU32": {"i": 65, "type": IT_RRVV32},
    "DIVU32": {"i": 66, "type": IT_RRVV32},
    # Float Arithmetic 32bit
    "ADDF": {"i": 67, "type": IT_RRVV32},
    "SUBF": {"i": 68, "type": IT_RRVV32},
    "MULF": {"i": 69, "type": IT_RRVV32},
    "DIVF": {"i": 70, "type": IT_RRVV32},
}

