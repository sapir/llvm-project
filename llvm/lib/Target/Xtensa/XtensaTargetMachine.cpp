//===- XtensaTargetMachine.cpp - Define TargetMachine for Xtensa ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about Xtensa target spec.
//
//===----------------------------------------------------------------------===//

#include "XtensaTargetMachine.h"
#include "XtensaTargetObjectFile.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

extern "C" void LLVMInitializeXtensaTarget() {
  // Register the target.
  RegisterTargetMachine<XtensaTargetMachine> A(TheXtensaTarget);

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeXtensaZOLPassPass(PR);
}

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options,
                                     bool isLittle) {
  std::string Ret = "e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-i64:64"
                    "-f64:64-a:0:32-n32";
  //       "e-m:e-p:32:32:32-i1:8:16-i8:8:16-i16:16-i32:32-"
  //       "f32:32-f64:64-f80:128-f128:128-n32";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(bool JIT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.hasValue() || JIT)
    return Reloc::Static;
  return *RM;
}

static CodeModel::Model getEffectiveCodeModel(Optional<CodeModel::Model> CM) {
  if (CM)
    return *CM;
  return CodeModel::Small;
}

XtensaTargetMachine::XtensaTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT,
                                         bool isLittle)
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options, isLittle), TT,
                        CPU, FS, Options, getEffectiveRelocModel(JIT, RM),
                        getEffectiveCodeModel(CM), OL),
      TLOF(make_unique<XtensaTargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

XtensaTargetMachine::XtensaTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT)
    : XtensaTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

const XtensaSubtarget *
XtensaTargetMachine::getSubtargetImpl(const Function &F) const {
  /* TODO Multiple subtargets
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = llvm::make_unique<XtensaSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
  */
  return &Subtarget;
}

namespace {
/// Xtensa Code Generator Pass Configuration Options.
class XtensaPassConfig : public TargetPassConfig {
public:
  XtensaPassConfig(XtensaTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  XtensaTargetMachine &getXtensaTargetMachine() const {
    return getTM<XtensaTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addPreEmitPass() override;
};
} // end anonymous namespace

bool XtensaPassConfig::addInstSelector() {
  addPass(createXtensaISelDag(getXtensaTargetMachine(), getOptLevel()));
  return false;
}

void XtensaPassConfig::addIRPasses() {
  addPass(createAtomicExpandPass());
  // TODO ZOL pass should be implemented
  // addPass(createXtensaZOLPass);
}

void XtensaPassConfig::addPreEmitPass() {
  addPass(createXtensaSizeReductionPass());
  addPass(&BranchRelaxationPassID);
}

TargetPassConfig *XtensaTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new XtensaPassConfig(*this, PM);
}

