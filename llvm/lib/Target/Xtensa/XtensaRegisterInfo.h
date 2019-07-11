//===-- XtensaRegisterInfo.h - Xtensa Register Information Impl ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===------------------------------------------------------------------------===//
//
// This file contains the Xtensa implementation of the TargetRegisterInfo class.
//
//===------------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSAREGISTERINFO_H
#define LLVM_LIB_TARGET_XTENSA_XTENSAREGISTERINFO_H

#include "Xtensa.h"
#include "XtensaMachineFunctionInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "XtensaGenRegisterInfo.inc"

namespace llvm {
class XtensaInstrInfo;
class XtensaSubtarget;

struct XtensaRegisterInfo : public XtensaGenRegisterInfo {
public:
  const XtensaSubtarget &Subtarget;

  XtensaRegisterInfo(const XtensaSubtarget &STI);

  // Override TargetRegisterInfo.h.
  bool requiresRegisterScavenging(const MachineFunction &MF) const override {
    return true;
  }
  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override {
    return true;
  }
  const uint16_t *
  getCalleeSavedRegs(const MachineFunction *MF = 0) const override;
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;
  void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS) const override;
  unsigned getFrameRegister(const MachineFunction &MF) const override;

private:
  virtual void eliminateFI(MachineBasicBlock::iterator II, unsigned OpNo,
                           int FrameIndex, uint64_t StackSize,
                           int64_t SPOffset) const;
};

} // end namespace llvm

#endif /* LLVM_LIB_TARGET_XTENSA_REGISTERINFO_H */
