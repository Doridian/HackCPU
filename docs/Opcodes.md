| Category                 | Name     | OP  | Type   | P1       | P2     | Description                                                               |
|--------------------------|----------|-----|--------|----------|--------|---------------------------------------------------------------------------|
| Basic                    | MOV      | 000 | RRVV   | RegDst   | RegSrc | RegDst = RegSrc                                                           |
|                          | MOV64    | 064 | RRVV64 | RegDst   | RegSrc | RegDst = RegSrc                                                           |
|                          | MOV8     | 001 | RRVV   | RegDst   | RegSrc | RegDst[0:7] = RegSrc & 0xFF                                               |
|                          | MOV16    | 002 | RRVV   | RegDst   | RegSrc | RegDst[0:15] = RegSrc & 0xFFFF                                            |
|                          | PUSH     | 003 | RRVV   | Reg      |        | CSP -= 4, RAM[SP] = Reg                                                   |
|                          | POP      | 004 | RRVV   | Reg      |        | Reg = RAM[SP], CSP += 4                                                   |
|                          | PUSH64   | 065 | RRVV64 | Reg      |        | CSP -= 8, RAM[SP] = Reg                                                   |
|                          | POP64    | 066 | RRVV64 | Reg      |        | Reg = RAM[SP], CSP += 8                                                   |
|                          | POPNIL   | 128 | N      |          |        | CSP += 4                                                                  |
|                          | POPNIL64 | 129 | N      |          |        | CSP += 8                                                                  |
|                          | NOP      | 130 | N      |          |        |                                                                           |
| Integer Arithmetic       | ADD      | 005 | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB      | 006 | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL      | 007 | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV      | 008 | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
|                          | MOD      | 009 | RRVV   | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |
|                          | SHL      | 010 | RRVV   | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |
|                          | LSHR     | 011 | RRVV   | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |
|                          | ASHR     | 012 | RRVV   | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |
|                          | MULU     | 013 | RRVV   | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |
|                          | DIVU     | 014 | RRVV   | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |
| Logic                    | XOR      | 015 | RRVV   | RegDst   | RegSrc | RegDst ^=  RegSrc                                                         |
|                          | OR       | 016 | RRVV   | RegDst   | RegSrc | RegDst                                                                    |
|                          | AND      | 017 | RRVV   | RegDst   | RegSrc | RegDst &=  RegSrc                                                         |
|                          | NOT      | 018 | RRVV   | RegDst   | RegSrc | RegDst = !(RegSrc)                                                        |
|                          | NOR      | 019 | RRVV   | RegDst   | RegSrc | RegDst                                                                    |
|                          | NAND     | 020 | RRVV   | RegDst   | RegSrc | RegDst &= !(RegSrc)                                                       |
| Compare                  | CMP      | 021 | RRVV   | Reg      | Reg2   | CR = COMPARE(Reg, Reg2)                                                   |
|                          | CMPS     | 022 | RRVV   | Reg      | Reg2   | (All CMP) Unset all CMP from FLAGR. Set FLAG_EQ and FLAG_LT appropriately |
|                          | CMP64    | 067 | RRVV64 | Reg      | Reg2   | CR = COMPARE_64BIT(Reg, Reg2)                                             |
|                          | CMP64S   | 068 | RRVV64 | Reg      | Reg2   |                                                                           |
|                          | CMPF     | 023 | RRVV   | Reg      | Reg2   | CR = COMPARE_FLOAT(Reg, Reg2)                                             |
|                          | CMP64F   | 069 | RRVV64 | Reg      | Reg2   | CR = COMPARE_DOUBLE(Reg, Reg2)                                            |
| Flow                     | JMP      | 024 | RRVV   | Reg      |        | (All J) PC = Reg                                                          |
|                          | JGE      | 025 | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |
|                          | JLE      | 026 | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |
|                          | JE       | 027 | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |
|                          | JNE      | 028 | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |
|                          | JG       | 029 | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |
|                          | JL       | 030 | RRVV   | Reg      |        | Check if FLAG_LT                                                          |
|                          | JZ       | 031 | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |
|                          | JNZ      | 032 | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |
|                          | CALL     | 033 | RRVV   | Reg      |        | (All C) PUSH PC, PUSH BSP, BSP = CSP, JMP Reg                             |
|                          | CGE      | 034 | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |
|                          | CLE      | 035 | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |
|                          | CE       | 036 | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |
|                          | CNE      | 037 | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |
|                          | CG       | 038 | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |
|                          | CL       | 039 | RRVV   | Reg      |        | Check if FLAG_LT                                                          |
|                          | CZ       | 040 | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |
|                          | CNZ      | 041 | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |
|                          | RETN     | 131 | N      |          |        | CSP = BSP, BSP = POP, PC = POP                                            |
|                          | RETNA    | 042 | RRVV   | Reg      |        | RETN, CSP += Reg                                                          |
|                          | RETNAC   | 132 | U8     | C        |        | RETN, CSP += C                                                            |
| Special                  | INT      | 043 | RRVV   | Reg      |        | CALL RAM[IHBASE+Reg*4]                                                    |
|                          | SETIH    | 044 | RRVV   | Reg      | Reg2   | RAM[IHBASE+Reg*4] = Reg2 (0 for disable)                                  |
|                          | HALT     | 133 | N      |          |        | Unset trap!!! Zero Memory!!! Halt execution                               |
|                          | RESET    | 134 | N      |          |        | Unset trap!!! Reloads bootloader, jumps into it                           |
|                          | TRAP     | 135 | N      |          |        | Trigger TRAP interrupt after next instruction                             |
|                          | ENCOFF   | 136 | N      |          |        | FLAGR &= ~FLAG_ENCON                                                      |
|                          | ENCON    | 137 | N      |          |        | FLAGR                                                                     |
|                          | PUSHREG  | 138 | N      |          |        | PUSH64 R12; PUSH64 R34; PUSH64 R56                                        |
|                          | POPREG   | 139 | N      |          |        | POP64 R56; POP64 R34; POP64 R12                                           |
| Integer Arithmetic 64bit | ADD64    | 070 | RRVV64 | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB64    | 071 | RRVV64 | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL64    | 072 | RRVV64 | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV64    | 073 | RRVV64 | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
|                          | MOD64    | 074 | RRVV64 | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |
|                          | SHL64    | 075 | RRVV64 | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |
|                          | LSHR64   | 076 | RRVV64 | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |
|                          | ASHR64   | 077 | RRVV64 | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |
|                          | MULU64   | 078 | RRVV64 | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |
|                          | DIVU64   | 079 | RRVV64 | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |
| Float Arithmetic 32bit   | ADDF     | 045 | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUBF     | 046 | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MULF     | 047 | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIVF     | 048 | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
| Float Arithmetic 64bit   | ADD64F   | 049 | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB64F   | 050 | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL64F   | 051 | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV64F   | 052 | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
| Extra                    | DEBUG    | 140 | N      |          |        |                                                                           |
|                          | CPUID    | 053 | RRVV   | Reg      |        | Reg = CPUID                                                               |
|                          | RAMSIZE  | 054 | RRVV   | Reg      |        | Reg= RAMSIZE                                                              |
