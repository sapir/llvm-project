//===-- XtensaMCAsmInfo.h - Xtensa Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------------===//
//
// This file contains the declaration of the XtensaMCAsmInfo class.
//
//===--------------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSATARGETASMINFO_H
#define LLVM_LIB_TARGET_XTENSA_XTENSATARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class StringRef;

class XtensaMCAsmInfo : public MCAsmInfo {
public:
  explicit XtensaMCAsmInfo();
};

} // namespace llvm

#endif
