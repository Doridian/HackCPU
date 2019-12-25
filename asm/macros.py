# %beginmacro MACRONAME, ARG1, ARG2, ...
# MOV ARG1, ARG2
# .....
# %endmacro

from defs import BYTEORDER

class Macro:
    def __init__(self, params = []):
        self.params = params

    def emit(self, transpiler):
        raise ValueError("Invalid base macro")

class ROMMacro(Macro):
    def __init__(self, params = []):
        Macro.__init__(self, params)

    def emit(self, transpiler):
        doenc = len(self.params) > 0
        int_enckkey = int(0)
        if doenc:
            int_enckkey = int(self.params[0], 0)
        transpiler.baseaddr = 0
        transpiler.suffix = (int_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
        transpiler.emitInstruction("__ENABLE_ENC", [int_enckkey])

class BOOTLOADERMacro(Macro):
    def __init__(self, params = []):
        Macro.__init__(self, params)

    def emit(self, transpiler):
        doenc = len(self.params) > 0
        int_enckkey = int(0)

        if doenc:
            int_enckkey = int(self.params[0], 0)

        bootloader_offset = int(self.params[1], 0)
        if bootloader_offset > 0:
            transpiler.baseaddr = bootloader_offset
        else:
            transpiler.baseaddr = 0
        transpiler.suffix = bootloader_offset.to_bytes(4, BYTEORDER, signed=True)
        if doenc:
            transpiler.emitNop()
            transpiler.emitLabelHere("__BOOTLOADER_BEGIN")
            transpiler.emitInstruction("MOV64", ["ENCREG", int_enckkey])
            transpiler.emitInstruction("ENCON")
            transpiler.emitInstruction("__ENABLE_ENC", [int_enckkey])
            transpiler.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN]", "[__BOOTLOADER_BEGIN]"])
            transpiler.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN + 2]", "[__BOOTLOADER_BEGIN + 2]"])
            transpiler.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN + 4]", "[__BOOTLOADER_BEGIN + 4]"])

macros = {}
macros["ROM"] = ROMMacro
macros["BOOTLOADER"] = BOOTLOADERMacro

def getMacro(name):
    return macros[name]

def loadMacro(str):
    pass