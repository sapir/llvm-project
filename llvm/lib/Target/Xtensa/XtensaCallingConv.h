//===- XtensaCallingConv.h - Xtensa Custom Calling Convention Routines -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-----------------------------------------------------------------------------===//
//
// This file contains the custom routines for the Xtensa Calling Convention that
// aren't done by tablegen.
//
//===-----------------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSACALLINGCONV_H
#define LLVM_LIB_TARGET_XTENSA_XTENSACALLINGCONV_H

namespace llvm {
namespace Xtensa {
const unsigned NumArgGPRs = 6;
const unsigned NumArgFPRs = 0;
} // namespace Xtensa
} // namespace llvm

#endif
