#BOOTLOADER 0xDEADBEEFB00B5303
MOV IHBASE, 0
MOV PSP, :RAM_MAX_ADDR
SUB PSP, 32
XOR R1, R1
XOR R2, R2
XOR R3, R3
XOR R4, R4
XOR R5, R5
XOR R6, R6

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
PUSH 1
INT 7
MOV R2, :RAM_MAX_ADDR
SUB R2, R1
ADD R2, 1
PUSH R1
PUSH R2
PUSH 1
INT 1

MOV IHBASE, R2
# We need 1024 (256 * 4) space, but since we can use the ROM's first 8 bytes which are only needed to boot (enc key) and then zero'd by us
SUB IHBASE, 1016

MOV PSP, IHBASE
SUB PSP, 4096
MOV CSP, PSP
SUB CSP, 4096

MOV64 R34, [R2]
MOV64 [R2], 0
XOR R3, 0xBEBADEFA
XOR R4, 0x0BB0FECA

ADD R2, 8
MOV [CSP], R2
ADD CSP, 4

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
MOV R1, PC
MOV R2, R1
MOD R2, 4
SUB R1, R2
XOR R2, R2
:erase_bootloader
SUB R1, 4
MOV [R1], 0
JNZ R1, :erase_bootloader
JNZ ENCREG1, :romwithenc
JNZ ENCREG2, :romwithenc
RETN
:romwithenc
ENCRETN

