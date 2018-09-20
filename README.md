# HackCPU
CPU architecture I created, including a primitive assembler for it
- Little endian
- 32-bit native
- 64-bit arithmetic extensions (floating point and integer)
- Native XOR decryption on the instruction decoder if enabled
- Suppert for native (such as I/O) interrupts as well as user-defined
- Stack grows down, uses stack frames (one stack for both return addresses as well as userdata)

## Boot process
### CPU internal behaviour
- Initialize all registers to 0
- Load bootloader to baseaddress (last 4 bytes of bootloader), if negative load to RAMSIZE - X
- Jump to bootloader
### Default bootloader behaviour [asm/bootloader.asm](asm/bootloader.asm)
- Check ROM I/O (verify length)
- Initialize memory layout (set startup stack pointers [CSP,BSP] and interrupt handler jump table [IHBASE])
- Read ROM to address 0
- Read ROM's encryption key (last 8 bytes of ROM) and XOR with base encryption key and apply to ENCREG
- Reset all GP registers to 0
- Jump to ROM (0)

## Memory layout (with default bootloader)
`[ROM - Stack - Interrupt vector table]`

## Stack frames
```
...

Return address
Old BSP
D1

Return address
Old BSP         <---- BSP
D1
D2
D3              <---- CSP

...
```

## Further reading
[Opcodes](docs/OPCODES.md)
[Registers](docs/REGISTERS.md)
[IO](docs/IO.md)
