//===- XtensaCallingConv.td - Calling Conventions for Xtensa ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------------===//
// This describes the calling conventions for the Xtensa ABI.
//===---------------------------------------------------------------------------===//

/// CCIfAlign - Match of the original alignment of the arg
class CCIfAlign<string Align, CCAction A>:
  CCIf<!strconcat("ArgFlags.getOrigAlign() == ", Align), A>;

//===----------------------------------------------------------------------===//
// Xtensa return value calling convention
//===----------------------------------------------------------------------===//
def RetCC_Xtensa: CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[f32], CCBitConvertToType<i32>>,

  //First two return values go in a2, a3, a4, a5
  CCIfType<[i32], CCAssignToReg<[a2, a3, a4, a5]>>,
  CCIfType<[f32], CCAssignToReg<[a2, a3, a4, a5]>>,
  CCIfType<[i64], CCAssignToRegWithShadow<[a2, a4], [a3, a5]>>
]>;

def RetCCW_Xtensa: CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[f32], CCBitConvertToType<i32>>,

  //First two return values go in a10, a11, a12, a13
  CCIfType<[i32], CCAssignToReg<[a10, a11, a12, a13]>>,
  CCIfType<[f32], CCAssignToReg<[a10, a11, a12, a13]>>,
  CCIfType<[i64], CCAssignToRegWithShadow<[a10, a12], [a11, a13]>>
]>;

//===----------------------------------------------------------------------===//
// Xtensa argument calling conventions
//===----------------------------------------------------------------------===//
/*def CC_Xtensa: CallingConv<[
  //Always assign the sret pointer to the first arg reg
  CCIfSRet<CCAssignToReg<[a2]>>,
  //Promote small int types to i32
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[f32], CCBitConvertToType<i32>>,

  // The first 6 integer arguments are passed in a2-a7
  // Large arguments (8-bytes) are always passed in an even/odd register pair 
  // even if that means to omit a register for alignment.
  CCIfType<[i32], CCIfAlign<"8", CCAssignToRegWithShadow<[a2, a4, a6], [a3, a5, a7]>>>,
  CCIfType<[i32], CCIf<"ArgFlags.getOrigAlign() != 8",
                       CCAssignToReg<[a2, a3, a4, a5, a6, a7]>>>,

  
//  CCIfType<[i32], CCAssignToReg<[a2, a3, a4, a5, a6, a7]>>,

  // Other arguments are passed in 8-byte-aligned 8-byte stack slots.
  CCIfType<[i32, i64, f32, f64], CCAssignToStack<4, 4>>
]>;

//Var args are all passed in integer regs
def CC_Xtensa_VAR : CallingConv<[
  //Always assign the sret pointer to the first arg reg
  CCIfSRet<CCAssignToReg<[a2]>>,
  //Promote small int types to i32
  CCIfType<[i1, i8,i16], CCPromoteToType<i32>>,

  // The first 6 integer arguments are passed in a2-a7
  // Large arguments (8-bytes) are always passed in an even/odd register pair 
  // even if that means to omit a register for alignment.
  CCIfType<[i32], CCIfAlign<"8", CCAssignToRegWithShadow<[a2, a4, a6], [a3, a5, a7]>>>,
  CCIfType<[i32], CCIf<"ArgFlags.getOrigAlign() != 8",
                       CCAssignToReg<[a2, a3,  a4,  a5,  a6, a7]>>>,

  // Other arguments are passed in 8-byte-aligned 8-byte stack slots.
  CCIfType<[i32, i64, f32, f64], CCAssignToStack<4, 4>>
]>;*/

//===----------------------------------------------------------------------===//
// Callee-saved register lists.
//===----------------------------------------------------------------------===//

def CSR_Xtensa: CalleeSavedRegs<(add a0, a12, a13, a14, a15)>;

// Win ABI Overflow Spill registers
def CSRW_Xtensa: CalleeSavedRegs<(add a8, a9, a10, a11, a12, a13, a14, a15)>;
// Win ABI Callee Save registers
def CSRWS_Xtensa: CalleeSavedRegs<(add a0)>;


