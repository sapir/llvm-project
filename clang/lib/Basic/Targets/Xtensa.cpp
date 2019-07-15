//===--- Xtensa.cpp - Implement Xtensa target feature support ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements Xtensa TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "Xtensa.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

void XtensaTargetInfo::getTargetDefines(const LangOptions &Opts,
                      MacroBuilder &Builder) const {
  Builder.defineMacro("__Xtensa__");
  Builder.defineMacro("__xtensa__");
  Builder.defineMacro("__XTENSA__");
  Builder.defineMacro("__XTENSA_WINDOWED_ABI__");
  Builder.defineMacro("__XTENSA_EL__");    
}

