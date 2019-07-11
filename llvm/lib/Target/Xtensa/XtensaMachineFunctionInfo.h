//===- XtensaMachineFunctionInfo.h - Xtensa machine function info ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------===//
//
// This file declares Xtensa-specific per-machine-function information.
//
//===-------------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSAMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_XTENSA_XTENSAMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class XtensaFunctionInfo : public MachineFunctionInfo {
  MachineFunction &MF;

  unsigned VarArgsFirstGPR;
  int VarArgsStackOffset;
  unsigned VarArgsFrameIndex;

public:
  explicit XtensaFunctionInfo(MachineFunction &MF)
      : MF(MF), VarArgsFirstGPR(0), VarArgsStackOffset(0),
        VarArgsFrameIndex(0) {
    MF.setAlignment(2);
  }

  unsigned getVarArgsFirstGPR() const { return VarArgsFirstGPR; }
  void setVarArgsFirstGPR(unsigned GPR) { VarArgsFirstGPR = GPR; }

  int getVarArgsStackOffset() const { return VarArgsStackOffset; }
  void setVarArgsStackOffset(int Offset) { VarArgsStackOffset = Offset; }

  // Get and set the frame index of the first stack vararg.
  unsigned getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(unsigned FI) { VarArgsFrameIndex = FI; }
  
  //TODO: large frame size definition should be specified more precisely
  bool isLargeFrame() {  return (MF.getFrameInfo().estimateStackSize(MF) > 512) ? true : false;}
};

} // namespace llvm

#endif /* LLVM_LIB_TARGET_XTENSA_XTENSAMACHINEFUNCTIONINFO_H */
