#ROM 0x123456789abcdef0
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
CMP R3, :RAM_SIZE - 2048
JL :8bittest_next
PUSH :db_ramok_len
PUSH :db_ramok
PUSH 0
INT 0

:test_fail
HALT
DB ramok, "RAM OK"

:ram_write
MOV R1, [BSP + 8]
MOV [R1], 0x37133713
RETNA 1

:ram_check
MOV R1, [BSP + 8]
CMP [R1], 0x37133713
JNE :ram_check_fail
MOV R1, 1
RETNA 1
:ram_check_fail
MOV R1, 0
RETNA 1

NOP
NOP
NOP
NOP
