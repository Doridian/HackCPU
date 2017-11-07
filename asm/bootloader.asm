#BOOTLOADER 0xDEADBEEF
MOV $IHBASE 0
MOV $PSP 0xFFDF
XOR $R1 $R1
XOR $R2 $R2
XOR $R3 $R3
XOR $R4 $R4
PUSH 1
INT 6
POP $R1
JNZ $R1 :haltme
POP $R1
CMP $R1 5
JL :haltme
PUSH 1
INT 7
MOV $R2 0xFFFF
SUB $R2 $R1
PUSH $R1
PUSH $R2
PUSH 1
INT 1
MOV $IHBASE $R2
# We need 512 (256 * 2) space, but since we can use the ROM's first 4 bytes which are only needed to boot (enc key) and then zero'd by us
SUB $IHBASE 508
MOV $PSP $IHBASE
SUB $PSP 0x100
MOV $CSP $PSP
SUB $CSP 0x100
MOV32 $R34 @$R2
MOV32 @$R2 0
XOR $R3 0xDEFA
XOR $R4 0xBEBA
ADD $R2 4
MOV @$CSP $R2
ADD $CSP 2
ENCOFF
:DISABLE_ENC
MOV32 $ENCREG $R34
XOR $R3 $R3
XOR $R4 $R4
MOV $R1 $PC
MOV $R2 $R1
MOD $R2 2
ADD $R1 $R2
ADD $R1 2
XOR $R2 $R2
:erase_bootloader
SUB $R1 2
MOV @$R1 0
JNZ $R1 :erase_bootloader
JNZ $ENCREG1 :romwithenc
JNZ $ENCREG2 :romwithenc
RETN
:romwithenc
ENCRETN
:haltme
HALT
