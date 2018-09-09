#BOOTLOADER 0xDEADBEEFB00B5303, -4096
MOV IHBASE, 0
MOV CSP, 32
MOV BSP, CSP
XOR R1, R1
XOR R2, R2
XOR R3, R3
XOR R4, R4
XOR R5, R5
XOR R6, R6

# Store BL begin in R2
MOV R2, PC
:CALCLABEL
SUB R2, :CALCLABEL

# Read ROM to RAM
PUSH 1
INT 6
POP R1
CMP R1, 9
MOV R6, :keeploading
ADD R6, R2
JGE R6

PUSH :db_romtooshort_len
MOV R6, :db_romtooshort
ADD R6, R2
PUSH R6
PUSH 0
INT 0
HALT
DB romtooshort, "ROM too short or not present"

:keeploading
CMP R1, R2
MOV R6, :keeploading2
ADD R6, R2
JLE R6

PUSH :db_romtoolong_len
MOV R6, :db_romtoolong
ADD R6, R2
PUSH R6
PUSH 0
INT 0
HALT
DB romtoolong, "ROM too long"

:keeploading2
MOV R3, R2

:CLEAREND
# Calc clear end in R5
MOV R5, R2
ADD R5, :CLEAREND
MOV R6, :selfclearloop
ADD R6, R2
:selfclearloop
MOV [R3], 0
ADD R3, 4
CMP R3, R5
JL R6

PUSH 1
INT 7
PUSH R1
PUSH 0
PUSH 1
INT 1

RAMSIZE IHBASE
SUB IHBASE, 1025

MOV CSP, IHBASE
MOV BSP, CSP

MOV64 R34, [R1 - 8]
MOV64 [R1 - 8], 0
XOR R3, 0xBEBADEFA
XOR R4, 0x0BB0FECA

MOV R6, :unset_ih
ADD R6, R2
MOV R1, 256
:unset_ih
SUB R1, 1
SETIH R1, 0
JNZ R1, R6

MOV R6, :romwithenc
ADD R6, R2

ENCOFF
__DISABLE_ENC
MOV64 ENCREG, R34
XOR R3, R3
XOR R4, R4
XOR R5, R5
JNZ ENCREG1, R6
JNZ ENCREG2, R6
XOR R2, R2
XOR R6, R6
JMP 0
:romwithenc
MOV R6, :romwithenc_retn
ADD R6, R2
XOR [R6], ENCREG1
XOR [R6 + 4], ENCREG2
XOR [R6 + 8], ENCREG1
XOR [R6 + 12], ENCREG2
#ALIGN 8, 7
ENCON
:romwithenc_retn
XOR R2, R2
XOR R6, R6
JMP 0
