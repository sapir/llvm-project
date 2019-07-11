//===- XtensaRegisterInfo.td - Xtensa Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Class definitions.
//===----------------------------------------------------------------------===//

class XtensaReg<string n> : Register<n> {
  let Namespace = "Xtensa";
}

class XtensaRegWithSubRegs<string n, list<Register> subregs>
  : RegisterWithSubRegs<n, subregs> {
  let Namespace = "Xtensa";
}

//===----------------------------------------------------------------------===//
// General-purpose registers
//===----------------------------------------------------------------------===//

// Xtensa general purpose regs
class ARReg<bits<4> num, string n> : XtensaReg<n> {
    let HWEncoding{3-0} = num;
}

// Return Address
def a0 : ARReg<0, "a0">, DwarfRegNum<[0]>;

// Stack Pointer (callee-saved)
def sp : ARReg<1, "sp">, DwarfRegNum<[1]>;

// Function Arguments
def a2 : ARReg<2, "a2">, DwarfRegNum<[2]>;
def a3 : ARReg<3, "a3">, DwarfRegNum<[3]>;
def a4 : ARReg<4, "a4">, DwarfRegNum<[4]>;
def a5 : ARReg<5, "a5">, DwarfRegNum<[5]>;
def a6 : ARReg<6, "a6">, DwarfRegNum<[6]>;
def a7 : ARReg<7, "a7">, DwarfRegNum<[7]>;

// Static Chain
def a8 : ARReg<8, "a8">, DwarfRegNum<[8]>;

def a9 : ARReg<9, "a9">, DwarfRegNum<[9]>;
def a10 : ARReg<10, "a10">, DwarfRegNum<[10]>;
def a11 : ARReg<11, "a11">, DwarfRegNum<[11]>;

// Callee-saved
def a12 : ARReg<12, "a12">, DwarfRegNum<[12]>;
def a13 : ARReg<13, "a13">, DwarfRegNum<[13]>;
def a14 : ARReg<14, "a14">, DwarfRegNum<[14]>;

// Stack-Frame Pointer (optional) - Callee-Saved
def a15 : ARReg<15, "a15">, DwarfRegNum<[15]>;

//===----------------------------------------------------------------------===//
// Special-purpose registers
//===----------------------------------------------------------------------===//

//Xtensa special purpose regs
class SRReg<bits<8> num, string n> : XtensaReg<n> {
  let HWEncoding{7-0} = num;
}

// Shift Amount Register
def sar : SRReg<3, "sar">;

// Register Classes
//def AR : RegisterClass<"Xtensa", [i32], 32, (add 
//  a0, sp, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)>;

// Expected data value for S32C1I operation
def SCOMPARE1 : SRReg<12, "SCOMPARE1">;

// Register class with allocation order
def AR : RegisterClass<"Xtensa", [i32], 32, (add 
  a8, a9, a10, a11, a12, a13, a14, a15,
  a7, a6, a5, a4, a3, a2, a0, sp)>;

def SCOMPARER : RegisterClass<"Xtensa", [i32], 32, (add SCOMPARE1)>;
//===----------------------------------------------------------------------===//
// Floating-Point registers
//===----------------------------------------------------------------------===//

// Xtensa Floating-Point regs
class FPReg<bits<4> num, string n> : XtensaReg<n> {
  let HWEncoding{3-0} = num;
}
//todo
//def fp : FPReg<0, "fp">, DwarfRegNum<[16]>;

def f0 : FPReg<0, "f0">, DwarfRegNum<[19]>;
def f1 : FPReg<1, "f1">, DwarfRegNum<[20]>;
def f2 : FPReg<2, "f2">, DwarfRegNum<[21]>;
def f3 : FPReg<3, "f3">, DwarfRegNum<[22]>;
def f4 : FPReg<4, "f4">, DwarfRegNum<[23]>;
def f5 : FPReg<5, "f5">, DwarfRegNum<[24]>;
def f6 : FPReg<6, "f6">, DwarfRegNum<[25]>;
def f7 : FPReg<7, "f7">, DwarfRegNum<[26]>;
def f8 : FPReg<8, "f8">, DwarfRegNum<[27]>;
def f9 : FPReg<9, "f9">, DwarfRegNum<[28]>;
def f10 : FPReg<10, "f10">, DwarfRegNum<[29]>;
def f11 : FPReg<11, "f11">, DwarfRegNum<[30]>;
def f12 : FPReg<12, "f12">, DwarfRegNum<[31]>;
def f13 : FPReg<13, "f13">, DwarfRegNum<[32]>;
def f14 : FPReg<14, "f14">, DwarfRegNum<[33]>;
def f15 : FPReg<15, "f15">, DwarfRegNum<[34]>;

// Floating-Point register class with allocation order
def FPR : RegisterClass<"Xtensa", [f32], 32, (add 
  f8, f9, f10, f11, f12, f13, f14, f15,
  f7, f6, f5, f4, f3, f2, f1, f0)>;

//===----------------------------------------------------------------------===//
// Boolean registers
//===----------------------------------------------------------------------===//
class BReg<bits<4> num, string n> : XtensaReg<n> {
  let HWEncoding{3-0} = num;
}


foreach i = 0-15 in {
  def b#i  : BReg<i, "b"#i>,  DwarfRegNum<[i]>;
}

// Boolean register class with allocation order
//todo
def BR : RegisterClass<"Xtensa", [i1], 0, (add b0)>;

