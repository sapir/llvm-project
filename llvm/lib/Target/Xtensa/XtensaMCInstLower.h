//===- XtensaMCInstLower.h - Lower MachineInstr to MCInst -------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSAMCINSTLOWER_H
#define LLVM_LIB_TARGET_XTENSA_XTENSAMCINSTLOWER_H

#include "XtensaAsmPrinter.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCContext;
class MCInst;
class MCOperand;
class MCSymbol;
class MachineInstr;
class MachineOperand;
class XtensaAsmPrinter;

class LLVM_LIBRARY_VISIBILITY XtensaMCInstLower {
  MCContext &Ctx;
  XtensaAsmPrinter &Printer;

public:
  XtensaMCInstLower(MCContext &ctx, XtensaAsmPrinter &asmPrinter);

  // Lower MachineInstr MI to MCInst OutMI.
  void lower(const MachineInstr *MI, MCInst &OutMI) const;

  // Return an MCOperand for MO.  Return an empty operand if MO is implicit.
  MCOperand lowerOperand(const MachineOperand &MO, unsigned Offset = 0) const;

  // Return an MCOperand for MO, given that it equals Symbol + Offset.
  MCOperand lowerSymbolOperand(const MachineOperand &MO, const MCSymbol *Symbol,
                               int64_t Offset) const;

private:
  MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;

  MCSymbol *GetJumpTableSymbol(const MachineOperand &MO) const;

  MCSymbol *GetConstantPoolIndexSymbol(const MachineOperand &MO) const;

  MCSymbol *GetBlockAddressSymbol(const MachineOperand &MO) const;

  MCOperand LowerSymbolOperand(const MachineOperand &MO,
                               MachineOperand::MachineOperandType MOTy,
                               unsigned Offset) const;
};
} // end namespace llvm

#endif /* LLVM_LIB_TARGET_XTENSA_XTENSAMCINSTLOWER_H */
