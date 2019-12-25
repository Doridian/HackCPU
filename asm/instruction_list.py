from instruction import Instruction
from special_instructions import ChangeEncInstruction, AlignInstruction, DBInstruction, REMInstruction, MOVARGInstruction, DRETInstruction
from opcode_defs import OPCODES

specialInstructionList = {}

def getInstructionClass(opcodeName):
    if opcodeName in specialInstructionList:
        return specialInstructionList[opcodeName]
    if not opcodeName in OPCODES:
        raise ValueError("Invalid instruction name " + opcodeName)
    return Instruction

specialInstructionList["__DISABLE_ENC"] = ChangeEncInstruction
specialInstructionList["__ENABLE_ENC"] = ChangeEncInstruction
specialInstructionList["ALIGN"] = AlignInstruction
specialInstructionList["DB"] = DBInstruction
specialInstructionList["REM"] = REMInstruction
specialInstructionList["MOVARG"] = MOVARGInstruction
specialInstructionList["DRET"] = DRETInstruction
