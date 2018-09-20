| Index | Name    | 64-bit name |                                                                              |
|-------|---------|-------------|------------------------------------------------------------------------------|
| 0     | R1      | R12         | General purpose registers                                                    |
| 1     | R2      | R12         |                                                                              |
| 2     | R3      | R34         |                                                                              |
| 3     | R4      | R34         |                                                                              |
| 4     | R5      | R56         |                                                                              |
| 5     | R6      | R56         |                                                                              |
| 6     | CSP     |             | Current stack pointer                                                        |
| 7     | BSP     |             | Base stack pointer                                                           |
| 8     | PC      |             | Program counter                                                              |
| 9     | MSR     |             | Internal CPU register (contains FLAGR [8 bit])                               |
| 10    | ENCREG1 | ENCREG12    | Will XOR next read OP byte with &0xFF, then shift with carry-around          |
| 11    | ENCREG2 | ENCREG12    |                                                                              |
| 12    | IHBASE  |             | Interrupt handler base (if 0, interrupts are off)                            |
| 13    | CREG    |             | Will swap to const mode in instructions (NOT A REAL REGISTER)                |
| 14    | MREG    |             | Will swap to RAM mode in instructions (address in reg) (NOT A REAL REGISTER) |
| 15    | MREGC   |             | Will swap to RAM mode in instructions (address const) (NOT A REAL REGISTER)  |
