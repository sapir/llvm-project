//===- XtensaZOLPass.cpp - Xtensa LLVM Zero Overhead Loop Pass ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------===//

#include "Xtensa.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

using namespace llvm;

#define DEBUG_TYPE "xtensa-zol"

namespace {

class XtensaZOLPass : public LoopPass {
public:
  static char ID;

  explicit XtensaZOLPass() : LoopPass(ID) {
    initializeXtensaZOLPassPass(*PassRegistry::getPassRegistry());
  }

  llvm::StringRef getPassName() const override { return "Xtensa ZOL pass"; }

  bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    if (skipLoop(L))
      return false;

    /*AliasAnalysis *AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();*/
    ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    //    TargetLibraryInfo *TLI =
    //        &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();
    //    const TargetTransformInfo *TTI =
    //        &getAnalysis<TargetTransformInfoWrapperPass>().getTTI(
    //            *L->getHeader()->getParent());
    // const DataLayout *DL = &L->getHeader()->getModule()->getDataLayout();

    unsigned TC = SE->getSmallConstantMaxTripCount(L);

    errs() << "Loop counter = " << TC << "\n\n";

    for (Loop::block_iterator bbi = L->block_begin(); bbi != L->block_end();
         ++bbi) {
      BasicBlock *bb = *bbi;
      for (BasicBlock::iterator instr_iter = bb->begin();
           instr_iter != bb->end(); ++instr_iter) {
        Instruction *instr = &*instr_iter;
        if (SE->isSCEVable(instr->getType())) {
          const SCEV *scev = SE->getSCEV(instr);
          instr->print(errs());
          errs() << " -->  ";
          scev->print(errs());
          errs() << "\n";
        }
      }
    }

    return false;
  }

  /// This transformation requires natural loop information & requires that
  /// loop preheaders be inserted into the CFG.
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    //    AU.addRequired<TargetLibraryInfoWrapperPass>();
    //    AU.addRequired<TargetTransformInfoWrapperPass>();
    getLoopAnalysisUsage(AU);
  }
};

} // end anonymous namespace

char XtensaZOLPass::ID = 0;

INITIALIZE_PASS_BEGIN(XtensaZOLPass, "xtensa-zol", "Generate ZOL instructions",
                      false, false)
INITIALIZE_PASS_DEPENDENCY(LoopPass)
// INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)
// INITIALIZE_PASS_DEPENDENCY(TargetTransformInfoWrapperPass)
INITIALIZE_PASS_END(XtensaZOLPass, "xtensa-zol", "Generate ZOL instructions",
                    false, false)

Pass *llvm::createXtensaZOLPass() { return new XtensaZOLPass(); }
