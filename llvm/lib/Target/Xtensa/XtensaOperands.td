//===- XtensaOperands.td - Xtensa instruction operands ----*- tblgen-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------===//

include "XtensaOperators.td"

// Immediate operands with a shared generic render method.
class ImmAsmOperand<string name> : AsmOperandClass 
{ 
  let Name = name;
  let RenderMethod = "addImmOperands"; 
}

// Constructs both a DAG pattern and instruction operand for an immediate
// of type VT.  PRED returns true if a node is acceptable and XFORM returns
// the operand value associated with the node.  ASMOP is the name of the
// associated asm operand, and also forms the basis of the asm print method.
class Immediate<ValueType vt, code pred, string asmop>
  : Operand<vt>, ImmLeaf<vt, pred> 
{
  let PrintMethod = "print"##asmop;
  let ParserMatchClass = !cast<AsmOperandClass>(asmop);
}

// imm8 predicate - Immediate in the range [-128,127]
def Imm8_AsmOperand: ImmAsmOperand<"imm8">;
def imm8: Immediate<i32, [{ return Imm >= -128 && Imm <= 127; }], "Imm8_AsmOperand"> {
  let EncoderMethod = "getImm8OpValue";
} 

// imm8_sh8 predicate - Immediate in the range [-32768,32512]
// imm8 value left shifted by 8 bits
def Imm8_sh8_AsmOperand: ImmAsmOperand<"imm8_sh8">;
def imm8_sh8: Immediate<i32, [{ return Imm >= -32768 && Imm <= 32512 && ((Imm & 0xFF) == 0); }], "Imm8_sh8_AsmOperand"> {
  let EncoderMethod = "getImm8_sh8OpValue";
}

// imm7n predicate - Immediate in the range [-32,95]
def Imm7n_AsmOperand: ImmAsmOperand<"imm7n">;
def imm7n: Immediate<i32, [{ return Imm >= -32 && Imm <= 95; }], "Imm7n_AsmOperand"> {
  let EncoderMethod = "getImm7nOpValue";
} 

// imm12 predicate - Immediate in the range [-2048,2047]
def Imm12_AsmOperand: ImmAsmOperand<"imm12">;
def imm12: Immediate<i32, [{ return Imm >= -2048 && Imm <= 2047; }], "Imm12_AsmOperand"> {
  let EncoderMethod = "getImm12OpValue";
}

// immn predicate - Narrow Immediate in the range [-1,15]
def Immn_AsmOperand: ImmAsmOperand<"immn">;
def immn: Immediate<i32, [{ return Imm >= -1 && (Imm != 0) && Imm <= 15; }], "Immn_AsmOperand"> {
  let EncoderMethod = "getImmnOpValue";
} 

// shimm4 predicate - Immediate in the range [0,15]
def Shimm4_AsmOperand: ImmAsmOperand<"shimm4">;
def shimm4: Immediate<i32, [{ return Imm >= 0 && Imm <= 15; }], "Shimm4_AsmOperand"> {
  let EncoderMethod = "getShimm4OpValue";
} 

// shimm1_16 predicate - Immediate in the range [1,16]
def Shimm1_16_AsmOperand: ImmAsmOperand<"shimm1_16">;
def shimm1_16: Immediate<i32, [{ return Imm >= 1 && Imm <= 16; }], "Shimm1_16_AsmOperand"> {
  let EncoderMethod = "getShimm1_16OpValue";
}  

// shimm17_31 predicate - Immediate in the range [17,31]
def Shimm17_31_AsmOperand: ImmAsmOperand<"shimm17_31">;
def shimm17_31: Immediate<i32, [{ return Imm >= 17 && Imm <= 31; }], "Shimm17_31_AsmOperand"> {
  let EncoderMethod = "getShimm17_31OpValue";
} 

// shimm16_31 predicate - Immediate in the range [16,31]
def Shimm16_31_AsmOperand: ImmAsmOperand<"shimm16_31">;
def shimm16_31: Immediate<i32, [{ return Imm >= 16 && Imm <= 31; }], "Shimm16_31_AsmOperand"> {
  let EncoderMethod = "getShimm16_31OpValue";
}

// shimm5 predicate - Immediate in the range [0,31]
def Shimm5_AsmOperand: ImmAsmOperand<"shimm5">;
def shimm5: Immediate<i32, [{ return Imm >= 0 && Imm <= 31; }], "Shimm5_AsmOperand"> {
  let EncoderMethod = "getShimm5OpValue";
}

// Memory offset 0..255 for 8-bit memory accesses
def Offset8m8_AsmOperand: ImmAsmOperand<"offset8m8">;
def offset8m8: Immediate<i32, 
  [{ return Imm >= 0 && Imm <= 255; }], 
  "Offset8m8_AsmOperand">;

// Memory offset 0..510 for 16-bit memory accesses
def Offset8m16_AsmOperand: ImmAsmOperand<"offset8m16">;
def offset8m16: Immediate<i32, 
  [{ return Imm >= 0 && Imm <= 510 && (Imm & 0x1 == 0); }], 
  "Offset8m16_AsmOperand">;

// Memory offset 0..1020 for 32-bit memory accesses
def Offset8m32_AsmOperand: ImmAsmOperand<"offset8m32">;
def offset8m32: Immediate<i32, 
  [{ return Imm >= 0 && Imm <= 1020 && (Imm & 0x3 == 0); }], 
  "Offset8m32_AsmOperand">;

// Memory offset 0..60 for 32-bit memory accesses
def Offset4m32_AsmOperand: ImmAsmOperand<"offset4m32">;
def offset4m32: Immediate<i32, 
  [{ return Imm >= 0 && Imm <= 60 && (Imm & 0x3 == 0); }], 
  "Offset4m32_AsmOperand">;

// entry_imm12 predicate - Immediate in the range [0,32760], ENTRY parameter
def Entry_Imm12_AsmOperand: ImmAsmOperand<"entry_imm12">;
def entry_imm12: Immediate<i32, [{ return Imm >= 0 && Imm <= 32760 && (Imm & 0x3 == 0); }], "Entry_Imm12_AsmOperand"> {
  let EncoderMethod = "getEntry_Imm12OpValue";
}

// b4const predicate - Branch Immediate 4-bit signed operand
def B4const_AsmOperand: ImmAsmOperand<"b4const">;
def b4const: Immediate<i32,
  [{ switch (Imm)
     {
	   case -1: case 1: case 2: case 3:  case 4:
	   case 5:  case 6: case 7: case 8: case 10: case 12:
	   case 16: case 32: case 64: case 128: case 256: return 1;
	   default: return 0;
	 }
  }],
  "B4const_AsmOperand"> {
  let EncoderMethod = "getB4constOpValue";
}

// b4constu predicate - Branch Immediate 4-bit unsigned operand
def B4constu_AsmOperand: ImmAsmOperand<"b4constu">;
def b4constu: Immediate<i32,
  [{ switch (Imm)
     {
	   case 32768: case 65536: case 2: case 3:  case 4:
	   case 5:  case 6: case 7: case 8: case 10: case 12:
	   case 16: case 32: case 64: case 128: case 256: return 1;
	   default: return 0;
	 }
  }],
  "B4constu_AsmOperand"> {
  let EncoderMethod = "getB4constuOpValue";
}

def imm32 : ImmLeaf<i32, [{}]>, Operand<i32>;

// seimm7_22 predicate - Immediate in the range [7,22] for sign extend
def Seimm7_22_AsmOperand: ImmAsmOperand<"seimm7_22">;
def seimm7_22: Immediate<i32, [{ return Imm >= 7 && Imm <= 22; }], "Seimm7_22_AsmOperand"> {
  let EncoderMethod = "getShimmSeimm7_22OpValue";
}
//===----------------------------------------------------------------------===//
// Memory address operands
//===----------------------------------------------------------------------===//

class mem<Operand offset> : Operand<i32> 
{
  let MIOperandInfo = (ops offset, AR);
  let EncoderMethod = "getMemRegEncoding";
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMemOperand";
}

def mem_fp: mem<imm12>;
def addr_fp: ComplexPattern<iPTR, 2, "selectFrameIndex", [frameindex]>;

def mem8: mem<offset8m8>;
def mem16: mem<offset8m16>;
def mem32: mem<offset8m32>;
def mem32n: mem<offset4m32>;

def addrn: ComplexPattern<iPTR, 2, "selectMemRegAddrN", []>;
def addr_ish1: ComplexPattern<iPTR, 2, "selectMemRegAddrISH1", [frameindex]>;
def addr_ish2: ComplexPattern<iPTR, 2, "selectMemRegAddrISH2", [frameindex]>;
def addr_ish4: ComplexPattern<iPTR, 2, "selectMemRegAddrISH4", [frameindex]>;

//===----------------------------------------------------------------------===//
// Symbolic address operands
//===----------------------------------------------------------------------===//

class PCRelAddress<ValueType vt, string self>
  : ComplexPattern<vt, 1, "selectPCRelAddress", [Xtensa_pcrel_wrapper]>,
    Operand<vt> 
{
  let MIOperandInfo = (ops !cast<Operand>(self));
}

def pcrel32call: PCRelAddress<i32, "pcrel32call"> 
{
  let PrintMethod = "printCallOperand";
  let EncoderMethod = "getCallEncoding";
}

def brtarget : Operand<OtherVT> 
{
  let PrintMethod = "printBranchTarget";
  let EncoderMethod = "getBranchTargetEncoding";
}

def jumptarget: Operand<OtherVT> 
{
  let PrintMethod = "printJumpTarget";
  let EncoderMethod = "getJumpTargetEncoding";
}

