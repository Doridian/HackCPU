#BOOTLOADER 0xDEADBEEFB00B5303
MOV IHBASE, 0
MOV CSP, 32
MOV BSP, CSP
XOR R1, R1
XOR R2, R2
XOR R3, R3
XOR R4, R4
XOR R5, R5
XOR R6, R6
:CLEAREND

# Read ROM to RAM
PUSH 1
INT 6
POP R1
CMP R1, 9
JGE :keeploading

PUSH :db_romtooshort_len
PUSH :db_romtooshort
PUSH 0
INT 0
HALT
DB romtooshort, "ROM too short or not present"

:keeploading
CMP R1, :CLEAREND
JLE :keeploading2

PUSH :db_romtoolong_len
PUSH :db_romtoolong
PUSH 0
INT 0
HALT
DB romtoolong, "ROM too long"

:keeploading2
MOV R3, :BASEADDR

:selfclearloop
MOV [R3], 0
ADD R3, 4
CMP R3, :CLEAREND
JL :selfclearloop

PUSH 1
INT 7
PUSH R1
PUSH 0
PUSH 1
INT 1

MOV IHBASE, :RAM_SIZE - 1
SUB IHBASE, 1024

MOV CSP, IHBASE
MOV BSP, CSP

MOV64 R34, [R1 - 8]
MOV64 [R1 - 8], 0
XOR R3, 0xBEBADEFA
XOR R4, 0x0BB0FECA

MOV R1, 256
:unset_ih
SUB R1, 1
SETIH R1, 0
JNZ R1, :unset_ih

ENCOFF
__DISABLE_ENC
MOV64 ENCREG, R34
XOR R3, R3
XOR R4, R4
JNZ ENCREG1, :romwithenc
JNZ ENCREG2, :romwithenc
JMP 0
:romwithenc
XOR [:romwithenc_retn], ENCREG1
XOR [:romwithenc_retn + 4], ENCREG2
#ALIGN 8, 7
ENCON
:romwithenc_retn
JMP 0
