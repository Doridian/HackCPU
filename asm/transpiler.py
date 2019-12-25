from defs import BYTEORDER
from instruction_list import getInstructionClass
from parameter import Parameter

def lineSplit(line):
    line = line.strip()
    params = []
    curStr = ''
    inQuote = False
    inBracket = False

    def appendParam(c = ''):
        nonlocal curStr
        css = curStr.strip()
        if len(css) < 1:
            return
        params.append(css + c)
        curStr = ''

    for i in range(0, len(line)):
        c = line[i]

        if c == '"':
            if inBracket:
                raise ValueError('Encountered " while in [')
            inQuote = not inQuote
            if not inQuote:
                appendParam(c)
                continue
        elif not inQuote:
            if c == '[':
                if inBracket:
                    raise ValueError('Encountered [ while in [')
                inBracket = True
            elif c == ']':
                if not inBracket:
                    raise ValueError('Encountered ] while not in [')
                inBracket = False
                appendParam(c)
                continue
            elif not inBracket:
                if c == ' ':
                    continue
                elif c == ',':
                    appendParam()
                    continue

        curStr += c

    appendParam()
    return params

class Transpiler:
    def __init__(self, input_file, output_file):
        self.enccpos = 0

        self.enckey = ""
        self.enckey = None

        self.labels = {}
        self.instructions = []
        self.suffix = None
        self.bpos = 0
        self.in_f = open(input_file, "r")
        self.out_f = open(output_file, "wb")
        self.currentLine = ""
        self.writepos = 0

    def close(self):
        self.in_f.close()
        self.out_f.close()

    def encwrite(self, bs):
        self.writepos += len(bs)

        if not self.enckey:
            self.out_f.write(bs)
            self.enccpos += len(bs)
            self.enccpos %= 8
            return

        for b in bs:
            self.out_f.write((b ^ self.enckey[self.enccpos]).to_bytes(1, BYTEORDER))
            self.enccpos = self.enccpos + 1
            if self.enccpos > 7:
                self.enccpos = 0

    
    def emitLabel(self, name, value):
        self.labels[name] = value
        
    def emitNop(self):
        return self.emitInstruction("NOP")

    def emitInstruction(self, name, params = []):
        InstructionCtor = getInstructionClass(name)
        return InstructionCtor.handle(InstructionCtor, name, self, params)

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
            lsplit = lineSplit(line[lineSpacePos+1:])

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
                self.emitInstruction("ALIGN", [int(lsplit[0], 0), int(lsplit[1], 0)])
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
            write_oldpos = self.writepos
            insn_len = insn.len()

            insn.write()

            write_len = self.writepos - write_oldpos

            if write_len != insn_len:
                raise ValueError("Instruction %s gave length %d but wrote %d" % (insn.opcode and insn.opcode.name or str(insn), insn_len, write_len))

        if self.suffix != None:
            self.out_f.write(self.suffix)
        
    def tryParse(self):
        try:
            self.parse()
        except:
            print(self.currentLine)
            raise

        self.close()
