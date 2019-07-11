//===-- XtensaMCTargetDesc.cpp - Xtebsa target descriptions ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "XtensaMCTargetDesc.h"
#include "InstPrinter/XtensaInstPrinter.h"
#include "XtensaMCAsmInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "XtensaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "XtensaGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "XtensaGenRegisterInfo.inc"

using namespace llvm;

static MCAsmInfo *createXtensaMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT) {
  MCAsmInfo *MAI = new XtensaMCAsmInfo();
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(
      nullptr, MRI.getDwarfRegNum(Xtensa::sp, true),
      XtensaMC::CFAOffsetFromInitialSP);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstrInfo *createXtensaMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitXtensaMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createXtensaMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitXtensaMCRegisterInfo(X, Xtensa::sp);
  return X;
}

static MCSubtargetInfo *
createXtensaMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createXtensaMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCInstPrinter *createXtensaMCInstPrinter(const Triple &TT,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new XtensaInstPrinter(MAI, MII, MRI);
}

static MCStreamer *createXtensaMCObjectStreamer(
    const Triple &T, MCContext &Context, std::unique_ptr<MCAsmBackend> &&MAB,
    raw_pwrite_stream &OS, std::unique_ptr<MCCodeEmitter> &&Emitter,
    bool RelaxAll) {

  return createELFStreamer(Context, std::move(MAB), OS, std::move(Emitter),
                           RelaxAll);
}

extern "C" void LLVMInitializeXtensaTargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(TheXtensaTarget, createXtensaMCAsmInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(TheXtensaTarget,
                                        createXtensaMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(TheXtensaTarget, createXtensaMCInstrInfo);

  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(TheXtensaTarget,
                                    createXtensaMCRegisterInfo);

  // Register the MCSubtargetInfo.
  TargetRegistry::RegisterMCSubtargetInfo(TheXtensaTarget,
                                          createXtensaMCSubtargetInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(TheXtensaTarget,
                                       createXtensaMCAsmBackend);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheXtensaTarget,
                                        createXtensaMCInstPrinter);

  // Register the MCObjectStreamer;
  TargetRegistry::RegisterELFStreamer(TheXtensaTarget,
                                      createXtensaMCObjectStreamer);
}
