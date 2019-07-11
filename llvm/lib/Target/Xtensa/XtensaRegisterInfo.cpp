//===- XtensaRegisterInfo.cpp - Xtensa Register Information ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Xtensa implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "XtensaRegisterInfo.h"
#include "XtensaInstrInfo.h"
#include "XtensaSubtarget.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "xtensa-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "XtensaGenRegisterInfo.inc"

using namespace llvm;

// WinABI callee save list - empty
static const MCPhysReg CSRWE_Xtensa_SaveList[] = {0};

// WinABI call preserved mask - empty
static const uint32_t CSRWE_Xtensa_RegMask[] = {0};

XtensaRegisterInfo::XtensaRegisterInfo(const XtensaSubtarget &STI)
    : XtensaGenRegisterInfo(Xtensa::a0), Subtarget(STI) {}

const uint16_t *
XtensaRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  if (Subtarget.isWinABI())
    return CSRWE_Xtensa_SaveList; // CSRW_Xtensa_SaveList ?
  else
    return CSR_Xtensa_SaveList;
}

const uint32_t *
XtensaRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID) const {
  if (Subtarget.isWinABI())
    return CSRWE_Xtensa_RegMask;
  else
    return CSR_Xtensa_RegMask;
}

BitVector XtensaRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();

  Reserved.set(Xtensa::a0);
  if (TFI->hasFP(MF)) {
    // fp is the frame pointer.  Reserve all aliases.
    Reserved.set(Xtensa::a15);
  }

  // sp is the stack pointer.  Reserve all aliases.
  Reserved.set(Xtensa::sp);
  Reserved.set(Xtensa::a7);
  return Reserved;
}

void XtensaRegisterInfo::eliminateFI(MachineBasicBlock::iterator II,
                                     unsigned OpNo, int FrameIndex,
                                     uint64_t StackSize,
                                     int64_t SPOffset) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  int MinCSFI = 0;
  int MaxCSFI = -1;

  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }

  // The following stack frame objects are always referenced relative to $sp:
  //  1. Outgoing arguments.
  //  2. Pointer to dynamically allocated stack space.
  //  3. Locations for callee-saved registers.
  //  4. Locations for eh data registers.
  // Everything else is referenced relative to whatever register
  // getFrameRegister() returns.
  unsigned FrameReg;

  if ((FrameIndex >= MinCSFI && FrameIndex <= MaxCSFI))
    FrameReg = Xtensa::sp;
  else
    FrameReg = getFrameRegister(MF);

  // Calculate final offset.
  // - There is no need to change the offset if the frame object is one of the
  //   following: an outgoing argument, pointer to a dynamically allocated
  //   stack space or a $gp restore location,
  // - If the frame object is any of the following, its offset must be adjusted
  //   by adding the size of the stack:
  //   incoming argument, callee-saved register location or local variable.
  bool IsKill = false;
  int64_t Offset;

  Offset = SPOffset + (int64_t)StackSize;
  // loads and stores have the immediate before the FI
  // FIXME: this is a bit hacky
  if (MI.mayLoadOrStore())
    Offset += MI.getOperand(OpNo - 1).getImm();
  else
    Offset += MI.getOperand(OpNo + 1).getImm();

  DEBUG(errs() << "Offset     : " << Offset << "\n"
               << "<--------->\n");

  bool Valid = false;
  switch (MI.getOpcode()) {
  case Xtensa::L8I_P:
  case Xtensa::L8UI:
  case Xtensa::S8I:
    Valid = (Offset >= 0 && Offset <= 255);
    break;
  case Xtensa::L16SI:
  case Xtensa::L16UI:
  case Xtensa::S16I:
    Valid = (Offset >= 0 && Offset <= 510);
    break;
  case Xtensa::LEA_ADD:
    Valid = (Offset >= -128 && Offset <= 127);
    break;
  default:
    Valid = (Offset >= 0 && Offset <= 1020);
    break;
  }

  // If MI is not a debug value, make sure Offset fits in the 16-bit immediate
  // field.
  if (!MI.isDebugValue() && !Valid) {
    MachineBasicBlock &MBB = *MI.getParent();
    DebugLoc DL = II->getDebugLoc();
    unsigned ADD = Xtensa::ADD;
    unsigned Reg;
    const XtensaInstrInfo &TII = *static_cast<const XtensaInstrInfo *>(
        MBB.getParent()->getSubtarget().getInstrInfo());

    TII.loadImmediate(MBB, II, &Reg, Offset);
    BuildMI(MBB, II, DL, TII.get(ADD), Reg)
        .addReg(FrameReg)
        .addReg(Reg, RegState::Kill);

    FrameReg = Reg;
    Offset = 0;
    IsKill = true;
  }

  MI.getOperand(OpNo).ChangeToRegister(FrameReg, false, false, IsKill);
  // loads and stores have the immediate before the FI
  // FIXME: this is a bit hacky
  if (MI.mayLoadOrStore())
    MI.getOperand(OpNo - 1).ChangeToImmediate(Offset);
  else
    MI.getOperand(OpNo + 1).ChangeToImmediate(Offset);
}

void XtensaRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();

  DEBUG(errs() << "\nFunction : " << MF.getName() << "\n"; errs()
                                                           << "<--------->\n"
                                                           << MI);

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  uint64_t stackSize = MF.getFrameInfo().getStackSize();
  int64_t spOffset = MF.getFrameInfo().getObjectOffset(FrameIndex);

  DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
               << "spOffset   : " << spOffset << "\n"
               << "stackSize  : " << stackSize << "\n");

  eliminateFI(MI, FIOperandNum, FrameIndex, stackSize, spOffset);
}

unsigned XtensaRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? (Subtarget.isWinABI() ? Xtensa::a7 : Xtensa::a15)
                        : Xtensa::sp;
}
