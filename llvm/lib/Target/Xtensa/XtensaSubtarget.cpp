//===- XtensaSubtarget.cpp - Xtensa Subtarget Information -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Xtensa specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "XtensaSubtarget.h"
#include "Xtensa.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "xtensa-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "XtensaGenSubtargetInfo.inc"

using namespace llvm;

XtensaSubtarget &
XtensaSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {
  std::string CPUName = CPU;
  if (CPUName.empty()) {
    // set default cpu name
    CPUName = "esp32";
  }

  HasDensity = false;
  HasSingleFloat = false;
  HasLoop = false;
  HasMAC16 = false;
  HasWindowed = false;
  HasBoolean = false;
  HasSEXT = false;
  HasNSA = false;
  HasMul32 = false;
  HasMul32High = false;
  HasDiv32 = false;
  HasS32C1I = false;
  HasTHREADPTR = false;

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
  return *this;
}

XtensaSubtarget::XtensaSubtarget(const Triple &TT, const std::string &CPU,
                                 const std::string &FS, const TargetMachine &TM)
    : XtensaGenSubtargetInfo(TT, CPU, FS), XtensaArchVersion(ESP32),
      TargetTriple(TT), InstrInfo(initializeSubtargetDependencies(CPU, FS)),
      TLInfo(TM, *this), TSInfo(), FrameLowering(), UseSmallSection(false),
      UseSoftFloat(false) {}
/*
// Return true if GV binds locally under reloc model RM.
static bool bindsLocally(const GlobalValue *GV, Reloc::Model RM) {
  // For non-PIC, all symbols bind locally.
  if (RM == Reloc::Static)
    return true;

  return GV->hasLocalLinkage() || !GV->hasDefaultVisibility();
}
*/
