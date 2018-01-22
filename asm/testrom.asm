#ROM 0
:baseaddr
MOV IHBASE 0
# RAM TEST 8-bit
MOV R1 :baseaddr
MOV R2 R1
MOD R2 4
SUB R1 R2
:8bittest_next
SUB R1 4
MOV [R1] 0x37133713
CMP [R1] 0x37133713
JNE :test_fail
JNZ R1 :8bittest_next
PUSH :str_ramok_len
PUSH :str_ramok
PUSH 0
INT 0

:test_fail
HALT
STR ramok "RAM OK"
