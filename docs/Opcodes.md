| Category                 | Name     | OP | 0xOP | Type   | P1       | P2     | Description                                                               | Extended description |
|--------------------------|----------|----|------|--------|----------|--------|---------------------------------------------------------------------------|----------------------|
| Basic                    | MOV      | 0  | 00   | RRVV   | RegDst   | RegSrc | RegDst = RegSrc                                                           |                      |
|                          | MOV64    | 1  | 01   | RRVV64 | RegDst   | RegSrc | RegDst = RegSrc                                                           |                      |
|                          | MOV8     | 2  | 02   | RRVV   | RegDst   | RegSrc | RegDst[0:7] = RegSrc & 0xFF                                               |                      |
|                          | MOV16    | 3  | 03   | RRVV   | RegDst   | RegSrc | RegDst[0:15] = RegSrc & 0xFFFF                                            |                      |
|                          | PUSH     | 4  | 04   | RRVV   | Reg      |        | CSP -= 4, RAM[SP] = Reg                                                   |                      |
|                          | POP      | 5  | 05   | RRVV   | Reg      |        | Reg = RAM[SP], CSP += 4                                                   |                      |
|                          | PUSH64   | 6  | 06   | RRVV64 | Reg      |        | CSP -= 8, RAM[SP] = Reg                                                   |                      |
|                          | POP64    | 7  | 07   | RRVV64 | Reg      |        | Reg = RAM[SP], CSP += 8                                                   |                      |
|                          | NOP      | 8  | 08   | N      |          |        |                                                                           |                      |
| Integer Arithmetic       | ADD      | 9  | 09   | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |                      |
|                          | SUB      | 10 | 0A   | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |                      |
|                          | MUL      | 11 | 0B   | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |                      |
|                          | DIV      | 12 | 0C   | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |                      |
|                          | MOD      | 13 | 0D   | RRVV   | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |                      |
|                          | SHL      | 14 | 0E   | RRVV   | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |                      |
|                          | LSHR     | 15 | 0F   | RRVV   | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |                      |
|                          | ASHR     | 16 | 10   | RRVV   | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |                      |
|                          | MULU     | 17 | 11   | RRVV   | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |                      |
|                          | DIVU     | 18 | 12   | RRVV   | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |                      |
| Logic                    | XOR      | 19 | 13   | RRVV   | RegDst   | RegSrc | RegDst ^=  RegSrc                                                         |                      |
|                          | OR       | 20 | 14   | RRVV   | RegDst   | RegSrc | RegDst |=  RegSrc                                                         |                      |
|                          | AND      | 21 | 15   | RRVV   | RegDst   | RegSrc | RegDst &=  RegSrc                                                         |                      |
|                          | NOT      | 22 | 16   | RRVV   | RegDst   | RegSrc | RegDst = !(RegSrc)                                                        |                      |
|                          | NOR      | 23 | 17   | RRVV   | RegDst   | RegSrc | RegDst |= !(RegSrc)                                                       |                      |
|                          | NAND     | 24 | 18   | RRVV   | RegDst   | RegSrc | RegDst &= !(RegSrc)                                                       |                      |
| Compare                  | CMP      | 25 | 19   | RRVV   | Reg      | Reg2   | CR = COMPARE(Reg, Reg2)                                                   |                      |
|                          | CMPS     | 26 | 1A   | RRVV   | Reg      | Reg2   | (All CMP) Unset all CMP from FLAGR. Set FLAG_EQ and FLAG_LT appropriately |                      |
|                          | CMP64    | 27 | 1B   | RRVV64 | Reg      | Reg2   | CR = COMPARE_64BIT(Reg, Reg2)                                             |                      |
|                          | CMP64S   | 28 | 1C   | RRVV64 | Reg      | Reg2   |                                                                           |                      |
|                          | CMPF     | 29 | 1D   | RRVV   | Reg      | Reg2   | CR = COMPARE_FLOAT(Reg, Reg2)                                             |                      |
|                          | CMP64F   | 30 | 1E   | RRVV64 | Reg      | Reg2   | CR = COMPARE_DOUBLE(Reg, Reg2)                                            |                      |
| Flow                     | JMP      | 31 | 1F   | RRVV   | Reg      |        | (All J) PC = Reg                                                          |                      |
|                          | JGE      | 32 | 20   | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |                      |
|                          | JLE      | 33 | 21   | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |                      |
|                          | JE       | 34 | 22   | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |                      |
|                          | JNE      | 35 | 23   | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |                      |
|                          | JG       | 36 | 24   | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |                      |
|                          | JL       | 37 | 25   | RRVV   | Reg      |        | Check if FLAG_LT                                                          |                      |
|                          | JZ       | 38 | 26   | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |                      |
|                          | JNZ      | 39 | 27   | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |                      |
|                          | CALL     | 40 | 28   | RRVV   | Reg      |        | (All C) PUSH PC, PUSH BSP, BSP = CSP, JMP Reg                             |                      |
|                          | CGE      | 41 | 29   | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |                      |
|                          | CLE      | 42 | 2A   | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |                      |
|                          | CE       | 43 | 2B   | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |                      |
|                          | CNE      | 44 | 2C   | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |                      |
|                          | CG       | 45 | 2D   | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |                      |
|                          | CL       | 46 | 2E   | RRVV   | Reg      |        | Check if FLAG_LT                                                          |                      |
|                          | CZ       | 47 | 2F   | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |                      |
|                          | CNZ      | 48 | 30   | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |                      |
|                          | RETN     | 49 | 31   | N      |          |        | CSP = BSP, BSP = POP, PC = POP                                            |                      |
| Special                  | INT      | 50 | 32   | RRVV   | Reg      |        | CALL RAM[IHBASE+Reg*4]                                                    |                      |
|                          | SETIH    | 51 | 33   | RRVV   | Reg      | Reg2   | RAM[IHBASE+Reg*4] = Reg2 (0 for disable)                                  |                      |
|                          | HALT     | 52 | 34   | N      |          |        | Unset trap!!! Zero Memory!!! Halt execution                               |                      |
|                          | RESET    | 53 | 35   | N      |          |        | Unset trap!!! Reloads bootloader, jumps into it                           |                      |
|                          | TRAP     | 54 | 36   | N      |          |        | Trigger TRAP interrupt after next instruction                             |                      |
|                          | ENCOFF   | 55 | 37   | N      |          |        | FLAGR &= ~FLAG_ENCON                                                      |                      |
|                          | ENCON    | 56 | 38   | N      |          |        | FLAGR |= FLAG_ENCON                                                       |                      |
|                          | PUSHREG  | 57 | 39   | N      |          |        | PUSH64 R12; PUSH64 R34; PUSH64 R56                                        |                      |
|                          | POPREG   | 58 | 3A   | N      |          |        | POP64 R56; POP64 R34; POP64 R12                                           |                      |
| Integer Arithmetic 64bit | ADD64    | 59 | 3B   | RRVV64 | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |                      |
|                          | SUB64    | 60 | 3C   | RRVV64 | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |                      |
|                          | MUL64    | 61 | 3D   | RRVV64 | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |                      |
|                          | DIV64    | 62 | 3E   | RRVV64 | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |                      |
|                          | MOD64    | 63 | 3F   | RRVV64 | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |                      |
|                          | SHL64    | 64 | 40   | RRVV64 | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |                      |
|                          | LSHR64   | 65 | 41   | RRVV64 | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |                      |
|                          | ASHR64   | 66 | 42   | RRVV64 | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |                      |
|                          | MULU64   | 67 | 43   | RRVV64 | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |                      |
|                          | DIVU64   | 68 | 44   | RRVV64 | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |                      |
| Float Arithmetic 32bit   | ADDF     | 69 | 45   | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |                      |
|                          | SUBF     | 70 | 46   | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |                      |
|                          | MULF     | 71 | 47   | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |                      |
|                          | DIVF     | 72 | 48   | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |                      |
| Float Arithmetic 64bit   | ADD64F   | 73 | 49   | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |                      |
|                          | SUB64F   | 74 | 4A   | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |                      |
|                          | MUL64F   | 75 | 4B   | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |                      |
|                          | DIV64F   | 76 | 4C   | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |                      |
| Extra                    | DEBUG    | 77 | 4D   | N      |          |        |                                                                           |                      |
|                          | RETNA    | 78 | 4E   | RRVV   | Reg      |        | RETN, CSP += Reg                                                          |                      |
|                          | POPNIL   | 79 | 4F   | N      |          |        | CSP += 4                                                                  |                      |
|                          | POPNIL64 | 80 | 50   | N      |          |        | CSP += 8                                                                  |                      |
|                          | RETNAC   | 81 | 51   | U8     | C        |        | RETN, CSP += C                                                            |                      |
|                          | CPUID    | 82 | 52   | RRVV   | Reg      |        | Reg = CPUID                                                               |                      |
|                          | RAMSIZE  | 83 | 53   | RRVV   | Reg      |        | Reg= RAMSIZE                                                              |                      |
