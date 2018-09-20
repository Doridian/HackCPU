#ROM 0

RAMSIZE R6
SUB R6, 2048

MOV IHBASE, 0
# RAM TEST 8-bit
MOV R3, :ENDADDR
MOV R2, R3
MOD R2, 4
SUB R3, R2

:8bittest_next
PUSH R3
CALL :ram_write
PUSH R3
CALL :ram_check
CMP R1, 1
JNE :test_fail
ADD R3, 4
CMP R3, R6
JL :8bittest_next
PUSH :db_ramok_len
PUSH :db_ramok
PUSH 0
INT 0

:test_fail
HALT
DB ramok, "RAM OK"

:ram_write
MOVARG R1, 1
MOV [R1], 0x37133713
DRET 1

:ram_check
MOVARG R1, 1
CMP [R1], 0x37133713
JNE :ram_check_fail
MOV R1, 1
DRET 1
:ram_check_fail
MOV R5, [R1]
MOV R4, R1
MOV R1, 0
DRET 1

NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
