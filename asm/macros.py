# %BEGIN MACRO, MACRONAME, ARG1, ARG2, ...
# MOV ARG1, ARG2
# .....
# %END

from defs import BYTEORDER
from re import sub, escape, compile

class Macro:
    def __init__(self, params = [], lines = []):
        self.params = params
        self.lines = lines

    def emit(self, transpiler):
        raise ValueError("Invalid base macro")

class ROMMacro(Macro):
    def __init__(self, params = [], lines = []):
        Macro.__init__(self, params, lines)

    def emit(self, transpiler):
        doenc = len(self.params) > 0
        int_enckkey = int(0)
        if doenc:
            int_enckkey = int(self.params[0], 0)
        transpiler.baseaddr = 0
        transpiler.suffix = (int_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
        transpiler.emitInstruction("__ENABLE_ENC", [int_enckkey])

class BOOTLOADERMacro(Macro):
    def __init__(self, params = [], lines = []):
        Macro.__init__(self, params, lines)

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
            transpiler.emitInstruction("XOR", ["[:__BOOTLOADER_BEGIN]", "[:__BOOTLOADER_BEGIN]"])
            transpiler.emitInstruction("XOR", ["[:__BOOTLOADER_BEGIN + 2]", "[:__BOOTLOADER_BEGIN + 2]"])
            transpiler.emitInstruction("XOR", ["[:__BOOTLOADER_BEGIN + 4]", "[:__BOOTLOADER_BEGIN + 4]"])

def makeSimpleMacro(outerParams, outerLines):
    return lambda params = [], lines = []: SIMPLEMacro(params, lines, outerParams, outerLines)

class SIMPLEMacro(Macro):
    def __init__(self, params = [], lines = [], outerParams = [], outerLines = []):
        Macro.__init__(self, params, lines)
        self.outerParams = outerParams
        self.outerLines = outerLines

    def emit(self, transpiler):
        subs = []

        for i, param in enumerate(self.outerParams):
            search = compile("([^\\w]|^)%s([^\\w]|$)" % escape(param))
            replace = "\\g<1>%s\\g<2>" % self.params[i]
            subs.append((search, replace))

        for line in self.outerLines:
            for subtupl in subs:
                line = sub(subtupl[0], subtupl[1], line)
            transpiler.emitLine(line)

class MACROMacro(Macro):
    def __init__(self, params = [], lines = []):
        Macro.__init__(self, params, lines)

    def emit(self, transpiler):
        transpiler.macros[self.params[0]] = makeSimpleMacro(self.params[1:], self.lines)

macros = {}
macros["ROM"] = ROMMacro
macros["BOOTLOADER"] = BOOTLOADERMacro
macros["MACRO"] = MACROMacro

def getMacro(transpiler, name):
    if name in transpiler.macros:
        return transpiler.macros[name]
    return macros[name]
