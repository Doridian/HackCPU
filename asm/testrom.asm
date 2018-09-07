#ROM 0
:baseaddr
MOV IHBASE, 0
# RAM TEST 8-bit
MOV R3, :baseaddr
MOV R2, R3
MOD R2, 4
SUB R3, R2
:8bittest_next
SUB R3, 4
PUSH R3
CALL :ram_write
CALL :ram_check
POP R3
CMP R1, 1
JNE :test_fail
JNZ R3, :8bittest_next
PUSH :db_ramok_len
PUSH :db_ramok
PUSH 0
INT 0

:test_fail
HALT
DB ramok, "RAM OK"

:ram_write
MOV R1, [BSP - 12]
MOV [R1], 0x37133713
RETN

:ram_check
MOV R1, [BSP - 12]
CMP [R1], 0x37133713
JNE :ram_check_fail
MOV R1, 1
RETN
:ram_check_fail
MOV R1, 0
RETN
