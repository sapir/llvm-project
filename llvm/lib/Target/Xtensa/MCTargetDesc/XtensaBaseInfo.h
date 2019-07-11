//===-- XtensaBaseInfo.h - Top level definitions for Xtensa MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Xtensa target useful for the compiler back-end and the MC libraries.
//
//===-------------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_XTENSA_MCTARGETDESC_XTENSABASEINFO_H
#define LLVM_LIB_TARGET_XTENSA_MCTARGETDESC_XTENSABASEINFO_H

#include "XtensaMCFixupKinds.h"
#include "XtensaMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// XtensaII - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace XtensaII {
/// Target Operand Flag enum.
enum TOF {
  //===------------------------------------------------------------------===//
  // Xtensa Specific MachineOperand flags.
  MO_NO_FLAG,
  /// MO_TPREL - Represents the offset from the thread pointer
  MO_TPREL,
  MO_PLT
};

} // namespace XtensaII
} // namespace llvm

#endif
