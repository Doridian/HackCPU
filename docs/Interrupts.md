| Index | Name              |                                                                              | Triggered by |
|-------|-------------------|------------------------------------------------------------------------------|--------------|
| 0     | I/O Write         | ID = POP; RAMOffset = POP; Length = POP; Writes RAM[RAMOffset:+Length] to ID | Program      |
| 1     | I/O Read          | ID = POP; RAMOffset = POP; Length = POP; Reads ID to RAM[RAMOffset:+Length]  | Program      |
| 2     | I/O Write Ptr Set | ID = POP; Offset =POP; Sets I/O write pointer for ID to Offset               | Program      |
| 3     | I/O Read Ptr Set  | ID = POP; Offset =POP; Sets I/O read pointer for ID to Offset                | Program      |
| 4     | I/O Write Ptr Get | ID = POP; PUSHes I/O write pointer for ID                                    | Program      |
| 5     | I/O Read Ptr Get  | ID = POP; PUSHes I/O read pointer for ID                                     | Program      |
| 6     | I/O Length Get    | ID = POP; PUSHes I/O length for ID (or -1 if not available)                  | Program      |
| 7     | I/O Reset         | ID = POP; Sets I/O read and write pointer to 0 for ID                        | Program      |
| 8     | HW info           | ID = POP; PUSH HWINFO to stack [first 32 bits]                               | Program      |
| 9     | HW info ptr       | ID = POP; RAMOffset = POP; Write HWINFO to RAMOffset; PUSH len               | Program      |
| ...   |                   |                                                                              |              |
| 128   | Illegal Opcode    | On any illegal opcode, PUSH $PC                                              | CPU          |
| 129   | Error             | On any error, PUSH $PC, PUSH errno                                           | CPU          |
| 130   | Trap              | If trap set after next instruction, unsets trap, PUSH $PC                    | CPU          |
