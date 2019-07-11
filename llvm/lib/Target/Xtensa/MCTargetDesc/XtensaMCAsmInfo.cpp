//===-- XtensaMCAsmInfo.cpp - Xtensa Asm Properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the XtensaMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "XtensaMCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"

using namespace llvm;

XtensaMCAsmInfo::XtensaMCAsmInfo() {
  CodePointerSize = 8;
  CalleeSaveStackSlotSize = 8;
 //TODO maybe Big-endian mode should be added
  IsLittleEndian = true;

  CommentString = "#";
  ZeroDirective = "\t.space\t";
  Data64bitsDirective = "\t.quad\t";
  GlobalDirective = "\t.global\t";
  UsesELFSectionDirectiveForBSS = true;
  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::DwarfCFI;
  AlignmentIsInBytes = false;
}
