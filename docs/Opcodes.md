| Category                 | Name     | OP  | 0xOP | Type   | P1       | P2     | Description                                                               |
|--------------------------|----------|----:|-----:|--------|----------|--------|---------------------------------------------------------------------------|
| Basic                    | MOV      |   0 |  `0` | RRVV   | RegDst   | RegSrc | RegDst = RegSrc                                                           |
|                          | MOV64    | 128 | `80` | RRVV64 | RegDst   | RegSrc | RegDst = RegSrc                                                           |
|                          | MOV8     |   1 |  `1` | RRVV   | RegDst   | RegSrc | RegDst[0:7] = RegSrc & 0xFF                                               |
|                          | MOV16    |   2 |  `2` | RRVV   | RegDst   | RegSrc | RegDst[0:15] = RegSrc & 0xFFFF                                            |
|                          | PUSH     |   3 |  `3` | RRVV   | Reg      |        | CSP -= 4, RAM[SP] = Reg                                                   |
|                          | POP      |   4 |  `4` | RRVV   | Reg      |        | Reg = RAM[SP], CSP += 4                                                   |
|                          | PUSH64   | 129 | `81` | RRVV64 | Reg      |        | CSP -= 8, RAM[SP] = Reg                                                   |
|                          | POP64    | 130 | `82` | RRVV64 | Reg      |        | Reg = RAM[SP], CSP += 8                                                   |
|                          | POPNIL   | 192 | `C0` | N      |          |        | CSP += 4                                                                  |
|                          | POPNIL64 | 193 | `C1` | N      |          |        | CSP += 8                                                                  |
|                          | NOP      | 194 | `C2` | N      |          |        |                                                                           |
| Integer Arithmetic       | ADD      |   5 |  `5` | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB      |   6 |  `6` | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL      |   7 |  `7` | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV      |   8 |  `8` | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
|                          | MOD      |   9 |  `9` | RRVV   | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |
|                          | SHL      |  10 |  `A` | RRVV   | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |
|                          | LSHR     |  11 |  `B` | RRVV   | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |
|                          | ASHR     |  12 |  `C` | RRVV   | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |
|                          | MULU     |  13 |  `D` | RRVV   | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |
|                          | DIVU     |  14 |  `E` | RRVV   | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |
| Logic                    | XOR      |  15 |  `F` | RRVV   | RegDst   | RegSrc | RegDst ^=  RegSrc                                                         |
|                          | OR       |  16 | `10` | RRVV   | RegDst   | RegSrc | RegDst                                                                    |
|                          | AND      |  17 | `11` | RRVV   | RegDst   | RegSrc | RegDst &=  RegSrc                                                         |
|                          | NOT      |  18 | `12` | RRVV   | RegDst   | RegSrc | RegDst = !(RegSrc)                                                        |
|                          | NOR      |  19 | `13` | RRVV   | RegDst   | RegSrc | RegDst                                                                    |
|                          | NAND     |  20 | `14` | RRVV   | RegDst   | RegSrc | RegDst &= !(RegSrc)                                                       |
| Compare                  | CMP      |  21 | `15` | RRVV   | Reg      | Reg2   | CR = COMPARE(Reg, Reg2)                                                   |
|                          | CMPS     |  22 | `16` | RRVV   | Reg      | Reg2   | (All CMP) Unset all CMP from FLAGR. Set FLAG_EQ and FLAG_LT appropriately |
|                          | CMP64    | 131 | `83` | RRVV64 | Reg      | Reg2   | CR = COMPARE_64BIT(Reg, Reg2)                                             |
|                          | CMP64S   | 132 | `84` | RRVV64 | Reg      | Reg2   |                                                                           |
|                          | CMPF     |  23 | `17` | RRVV   | Reg      | Reg2   | CR = COMPARE_FLOAT(Reg, Reg2)                                             |
|                          | CMP64F   | 133 | `85` | RRVV64 | Reg      | Reg2   | CR = COMPARE_DOUBLE(Reg, Reg2)                                            |
| Flow                     | JMP      |  24 | `18` | RRVV   | Reg      |        | (All J) PC = Reg                                                          |
|                          | JGE      |  25 | `19` | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |
|                          | JLE      |  26 | `1A` | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |
|                          | JE       |  27 | `1B` | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |
|                          | JNE      |  28 | `1C` | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |
|                          | JG       |  29 | `1D` | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |
|                          | JL       |  30 | `1E` | RRVV   | Reg      |        | Check if FLAG_LT                                                          |
|                          | JZ       |  31 | `1F` | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |
|                          | JNZ      |  32 | `20` | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |
|                          | CALL     |  33 | `21` | RRVV   | Reg      |        | (All C) PUSH PC, PUSH BSP, BSP = CSP, JMP Reg                             |
|                          | CGE      |  34 | `22` | RRVV   | Reg      |        | Check if !FLAG_LT                                                         |
|                          | CLE      |  35 | `23` | RRVV   | Reg      |        | Check if FLAG_LT or FLAG_EQ                                               |
|                          | CE       |  36 | `24` | RRVV   | Reg      |        | Check if FLAG_EQ                                                          |
|                          | CNE      |  37 | `25` | RRVV   | Reg      |        | Check if !FLAG_EQ                                                         |
|                          | CG       |  38 | `26` | RRVV   | Reg      |        | Check if !FLAG_LT and !FLAG_EQ                                            |
|                          | CL       |  39 | `27` | RRVV   | Reg      |        | Check if FLAG_LT                                                          |
|                          | CZ       |  40 | `28` | RRVV   | RegCheck | Reg    | Check if RegCheck = 0                                                     |
|                          | CNZ      |  41 | `29` | RRVV   | RegCheck | Reg    | Check if RegCheck != 0                                                    |
|                          | RETN     | 195 | `C3` | N      |          |        | CSP = BSP, BSP = POP, PC = POP                                            |
|                          | RETNA    |  42 | `2A` | RRVV   | Reg      |        | RETN, CSP += Reg                                                          |
| Special                  | INT      |  43 | `2B` | RRVV   | Reg      |        | CALL RAM[IHBASE+Reg*4]                                                    |
|                          | SETIH    |  44 | `2C` | RRVV   | Reg      | Reg2   | RAM[IHBASE+Reg*4] = Reg2 (0 for disable)                                  |
|                          | HALT     | 196 | `C4` | N      |          |        | Unset trap!!! Zero Memory!!! Halt execution                               |
|                          | RESET    | 197 | `C5` | N      |          |        | Unset trap!!! Reloads bootloader, jumps into it                           |
|                          | TRAP     | 198 | `C6` | N      |          |        | Trigger TRAP interrupt after next instruction                             |
|                          | ENCOFF   | 199 | `C7` | N      |          |        | FLAGR &= ~FLAG_ENCON                                                      |
|                          | ENCON    | 200 | `C8` | N      |          |        | FLAGR                                                                     |
|                          | PUSHREG  | 201 | `C9` | N      |          |        | PUSH64 R12; PUSH64 R34; PUSH64 R56                                        |
|                          | POPREG   | 202 | `CA` | N      |          |        | POP64 R56; POP64 R34; POP64 R12                                           |
| Integer Arithmetic 64bit | ADD64    | 134 | `86` | RRVV64 | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB64    | 135 | `87` | RRVV64 | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL64    | 136 | `88` | RRVV64 | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV64    | 137 | `89` | RRVV64 | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
|                          | MOD64    | 138 | `8A` | RRVV64 | RegDst   | RegSrc | RegDst %=  RegSrc                                                         |
|                          | SHL64    | 139 | `8B` | RRVV64 | RegDst   | RegSrc | RegDst <<= RegSrc                                                         |
|                          | LSHR64   | 140 | `8C` | RRVV64 | RegDst   | RegSrc | RegDst >>= RegSrc                                                         |
|                          | ASHR64   | 141 | `8D` | RRVV64 | RegDst   | RegSrc | RegDst >>>= RegSrc                                                        |
|                          | MULU64   | 142 | `8E` | RRVV64 | RegDst   | RegSrc | RegDst *= RegSrc (unsigned)                                               |
|                          | DIVU64   | 143 | `8F` | RRVV64 | RegDst   | RegSrc | RegDst /= RegSrc (unsigned)                                               |
| Float Arithmetic 32bit   | ADDF     |  45 | `2D` | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUBF     |  46 | `2E` | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MULF     |  47 | `2F` | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIVF     |  48 | `30` | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
| Float Arithmetic 64bit   | ADD64F   |  49 | `31` | RRVV   | RegDst   | RegSrc | RegDst +=  RegSrc                                                         |
|                          | SUB64F   |  50 | `32` | RRVV   | RegDst   | RegSrc | RegDst -=  RegSrc                                                         |
|                          | MUL64F   |  51 | `33` | RRVV   | RegDst   | RegSrc | RegDst *=  RegSrc                                                         |
|                          | DIV64F   |  52 | `34` | RRVV   | RegDst   | RegSrc | RegDst /=  RegSrc                                                         |
| Extra                    | DEBUG    | 203 | `CB` | N      |          |        |                                                                           |



Instruction layout allows decoder flow as follows:

```
[OPCODE  ] 
[0xxxxxxx] => RRVV
[10xxxxxx] => RRVV64
[11xxxxxx] => N
```
