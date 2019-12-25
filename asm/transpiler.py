from defs import BYTEORDER
from instruction import Instruction, AlignInstruction
from opcode_defs import OPCODES
from parameter import Parameter
from binascii import unhexlify

# params can be:
# :LABEL to refer to a label
# @ANY to refer to "thing at this address in RAM" (can be const or Register or Label)
# Reg to refer to a register
# So you can do @R1 to "RAM value at address value of R1"

class Transpiler:
    def __init__(self, input_file, output_file):
        self.enccpos = 0
        self.enckey = None
        self.labels = []
        self.instructions = []
        self.suffix = None
        self.bpos = 0
        self.in_f = open(input_file, "r")
        self.out_f = open(output_file, "wb")
        self.currentLine = ""

    def close(self):
        self.in_f.close()
        self.out_f.close()

    def encwrite(self, bs):
        if not self.enckey:
            self.out_f.write(bs)
            self.enccpos += len(bs)
            self.enccpos %= 8
            return

        for b in bs:
            self.out_f.write((b ^ self.enckey[self.enccpos]).to_bytes(1, BYTEORDER))
            enccpos = enccpos + 1
            if enccpos > 7:
                enccpos = 0

    
    def emitLabel(self, name, value):
        self.labels[name] = value
        
    def emitNop(self):
        return self.emitInstruction("NOP")

    def emitInstruction(self, name, params = []):
        insn = Instruction(self, OPCODES[name], list(map(Parameter, params)))
        self.emitInstructionRaw(insn)
        return insn

    def emitInstructionRaw(self, insn):
        self.instructions.append(insn)

    def emitLabelHere(self, name):
        insn = self.emitInstruction("REM")
        self.emitLabel(name, insn)

    def emitLine(self, line):
        line = line.strip()

        if len(line) < 1:
            return

        lineCommentPos = line.find(";")
        if lineCommentPos > 0:
            line = line[:lineCommentPos]

        if len(line) < 1:
            return

        lineSpacePos = line.find(" ")
        if lineSpacePos < 0:
            opcodeName = line
            lsplit = []
        else:
            opcodeName = line[0:lineSpacePos]
            lsplit = line[lineSpacePos+1:].strip().split(",")

        if opcodeName[0] == ":":
            self.emitLabelHere(opcodeName[1:])
        elif opcodeName[0] == "#" or opcodeName[0] == ";":
            doenc = len(lsplit) > 0
            int_enckkey = int(0)
            if opcodeName == "#ROM":
                if doenc:
                    int_enckkey = int(lsplit[0], 0)
                self.baseaddr = 0
                self.suffix = (int_enckkey ^ 0x0BB0FECABEBADEFA).to_bytes(8, BYTEORDER)
                self.emitInstruction("__ENABLE_ENC", [int_enckkey])
            elif opcodeName == "#BOOTLOADER":
                if doenc:
                    int_enckkey = int(lsplit[0], 0)

                bootloader_offset = int(lsplit[1], 0)
                if bootloader_offset > 0:
                    self.baseaddr = bootloader_offset
                else:
                    self.baseaddr = 0
                self.suffix = bootloader_offset.to_bytes(4, BYTEORDER, signed=True)
                if doenc:
                    self.emitNop()
                    self.emitLabelHere("__BOOTLOADER_BEGIN")
                    self.emitInstruction("MOV64", ["ENCREG", int_enckkey])
                    self.emitInstruction("ENCON")
                    self.emitInstruction("__ENABLE_ENC", [int_enckkey])
                    self.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN]", "[__BOOTLOADER_BEGIN]"])
                    self.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN + 2]", "[__BOOTLOADER_BEGIN + 2]"])
                    self.emitInstruction("XOR", ["[__BOOTLOADER_BEGIN + 4]", "[__BOOTLOADER_BEGIN + 4]"])
            elif opcodeName == "#ALIGN":
                self.emitInstructionRaw(AlignInstruction(self, int(lsplit[0], 0), int(lsplit[1], 0)))
        else:
            if opcodeName == "DB":
                lbl = "db_" + lsplit[0]
                rawData = line[line.find(lsplit[0]) + len(lsplit[0]) + 1:].strip()
                if rawData[0] == '"':
                    if rawData[-1] != '"' or len(rawData) < 2:
                        raise ValueError("Missing closing quote in DB string")
                    rawData = bytes(rawData[1:-1], "ascii")
                elif rawData[0:1] == "0x":
                    rawData = unhexlify(rawData[2:])
                else:
                    rawData = unhexlify(rawData.replace(" ", ""))
                insn = self.emitInstruction("DB", [rawData])
                self.emitLabel(lbl, insn)
                self.emitLabel(lbl + "_len", len(rawData))
            elif opcodeName == "DRET":
                useopc = None
                if len(lsplit) == 0:
                    useopc = "RETN"
                elif len(lsplit) == 1:
                    useopc = "RETNA"
                    try :
                        x = int(lsplit[0], 10) * 4
                        if x == 0:
                            useopc = "RETN"
                            lsplit = []
                        elif x < 0:
                            raise TypeError("DRET arg must be >= 0")
                        else:
                            lsplit[0] = "%d" % x
                    except ValueError:
                        pass
                else:
                    raise ValueError("DRET only supports 0 or 1 parameter")
                self.emitInstruction(useopc, lsplit)
            elif opcodeName == "MOVARG":
                p1 = lsplit[0]
                argno = int(lsplit[1], 10)
                if argno < 1:
                    raise ValueError("MOVARG argno must be at least 1")
                p2 = "[BSP + %d]" % ((argno * 4) + 4)
                self.emitInstruction("MOV", [p1, p2])
            else:
                self.emitInstruction(opcodeName, lsplit)

    def parse(self):
        self.suffix = None

        for line in self.in_f:
            self.currentLine = line
            self.emitLine(line)

        if self.baseaddr < 0:
            raise ValueError("baseaddr < 0")

        self.enccpos = self.baseaddr % 8

        self.emitLabel("BASEADDR",self. baseaddr)
        self.emitLabelHere("ENDADDR")

        for insn in self.instructions:
            insn.write()

        if self.suffix != None:
            self.out_f.write(self.suffix)
        
    def tryParse(self):
        try:
            self.parse()
        except:
            print(self.currentLine)
            raise

        self.close()
