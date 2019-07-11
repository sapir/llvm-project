//===- XtensaISelLowering.h - Xtensa DAG Lowering Interface ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------------===//
//
// This file defines the interfaces that Xtensa uses to lower LLVM code into a
// selection DAG.
//
//===--------------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSAISELLOWERING_H
#define LLVM_LIB_TARGET_XTENSA_XTENSAISELLOWERING_H

#include "Xtensa.h"
#include "llvm/CodeGen/CallingConvLower.h"
#if 0
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetLowering.h"
#endif
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"
#include <deque>
#include <string>

namespace llvm {
namespace XtensaISD {
enum {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,

  // Return with a flag operand.  Operand 0 is the chain operand.
  RET_FLAG,
  // WinABI Return
  RETW_FLAG,

  // Calls a function.  Operand 0 is the chain operand and operand 1
  // is the target address.  The arguments start at operand 2.
  // There is an optional glue operand at the end.
  CALL,
  // WinABI Call version
  CALLW,

  // Wraps a TargetGlobalAddress that should be loaded using PC-relative
  // accesses (AUIPC).  Operand 0 is the address.
  PCREL_WRAPPER,

  // Branches if a condition is true.  Operand 0 is the chain operand;
  // operand 1 is the 4-bit condition-code mask, with bit N in
  // big-endian order meaning "branch if CC=N"; operand 2 is the
  // target block and operand 3 is the flag operand.
  BRCOND,

  // Selects between operand 0 and operand 1.  Operand 2 is the
  // mask of condition-code values for which operand 0 should be
  // chosen over operand 1; it has the same form as BR_CCMASK.
  // Operand 3 is the flag operand.
  SELECT,
  SELECT_CC,
  SELECT_CC_FP,

  BR_CC_T,
  BR_CC_F,

  BR_JT,

  // Floating point unordered compare conditions
  CMPUEQ,
  CMPULE,
  CMPULT,
  CMPUO,
  // Floating point compare conditions
  CMPOEQ,
  CMPOLE,
  CMPOLT,
  // Predicate MOV
  MOVF,
  MOVT,
  // FP multipy-add/sub
  MADD,
  MSUB,
  // FP move
  MOVS,
  MOVSP,

  // shift
  SHL,
  SRA,
  SRL,
  SRC,
  SSL,
  SSR,

  MEMW,
  S32C1I,
  WSR,
  RUR
};
}

class XtensaSubtarget;

class XtensaTargetLowering : public TargetLowering {
public:
  explicit XtensaTargetLowering(const TargetMachine &TM,
                                const XtensaSubtarget &STI);

  MVT getScalarShiftAmountTy(const DataLayout &, EVT LHSTy) const override {
    return LHSTy.getSizeInBits() <= 32 ? MVT::i32 : MVT::i64;
  }

  EVT getSetCCResultType(const DataLayout &, LLVMContext &,
                         EVT VT) const override {
    return MVT::i32;
  }
  bool isFMAFasterThanFMulAndFAdd(EVT) const override { return true; }

  /// If a physical register, this returns the register that receives the
  /// exception address on entry to an EH pad.
  unsigned
  getExceptionPointerRegister(const Constant *PersonalityFn) const override;
  /// If a physical register, this returns the register that receives the
  /// exception typeid on entry to a landing pad.
  unsigned
  getExceptionSelectorRegister(const Constant *PersonalityFn) const override;

  bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;
  bool isFPImmLegal(const APFloat &Imm, EVT VT) const override;
  const char *getTargetNodeName(unsigned Opcode) const override;
  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;
  TargetLowering::ConstraintType
  getConstraintType(StringRef Constraint) const override;
  TargetLowering::ConstraintWeight
  getSingleConstraintMatchWeight(AsmOperandInfo &info,
                                 const char *constraint) const override;

  /// Returns the size of the platform's va_list object.
  unsigned getVaListSizeInBits(const DataLayout &DL) const override;

  /// LowerAsmOperandForConstraint - Lower the specified operand into the Ops
  /// vector.  If it is invalid, don't add anything to Ops. If hasMemory is
  /// true it means one of the asm constraint of the inline asm instruction
  /// being processed is 'm'.
  void LowerAsmOperandForConstraint(SDValue Op, std::string &Constraint,
                                    std::vector<SDValue> &Ops,
                                    SelectionDAG &DAG) const override;

  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *BB) const override;
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  virtual bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                              bool isVarArg,
                              const SmallVectorImpl<ISD::OutputArg> &Outs,
                              LLVMContext &Context) const;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;
  bool shouldInsertFencesForAtomic(const Instruction *I) const override {
    return true;
  }
  struct LTStr {
    bool operator()(const char *S1, const char *S2) const {
      return strcmp(S1, S2) < 0;
    }
  };

  /// ByValArgInfo - Byval argument information.
  struct ByValArgInfo {
    unsigned FirstIdx; // Index of the first register used.
    unsigned NumRegs;  // Number of registers used for this argument.
    unsigned Address;  // Offset of the stack area used to pass this argument.

    ByValArgInfo() : FirstIdx(0), NumRegs(0), Address(0) {}
  };

private:
  const XtensaSubtarget &Subtarget;

  SDValue lowerBR_JT(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerImmediate(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerImmediateFP(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSETCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerGlobalTLSAddress(GlobalAddressSDNode *Node,
                                SelectionDAG &DAG) const;
  SDValue lowerBlockAddress(BlockAddressSDNode *Node, SelectionDAG &DAG) const;
  SDValue lowerJumpTable(JumpTableSDNode *JT, SelectionDAG &DAG) const;
  SDValue lowerConstantPool(ConstantPoolSDNode *CP, SelectionDAG &DAG) const;
  SDValue lowerVASTART(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerVAARG(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerVACOPY(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSTACKSAVE(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSTACKRESTORE(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerATOMIC_FENCE(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerShiftLeftParts(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerShiftRightParts(SDValue Op, SelectionDAG &DAG, bool IsSRA) const;

  SDValue getTargetNode(SDValue Op, SelectionDAG &DAG, unsigned Flag) const;
  SDValue getAddrPCRel(SDValue Op, SelectionDAG &DAG) const;

  // Implement EmitInstrWithCustomInserter for individual operation types.
  MachineBasicBlock *emitCALL(MachineInstr *MI, MachineBasicBlock *BB) const;
  MachineBasicBlock *emitSelectCC(MachineInstr &MI,
                                  MachineBasicBlock *BB) const;
  MachineBasicBlock *emitAtomicCmpSwap(MachineInstr &MI, MachineBasicBlock *BB,
                                       int isByteOperand) const;
  MachineBasicBlock *emitAtomicRMW(MachineInstr &MI, MachineBasicBlock *BB,
                                   bool isByteOperand, unsigned Opcode,
                                   bool inv, bool minmax) const;
  MachineBasicBlock *emitAtomicRMW(MachineInstr &MI, MachineBasicBlock *BB,
                                   unsigned Opcode, bool inv,
                                   bool minmax) const;

  CCAssignFn *CCAssignFnForCall(CallingConv::ID CC, bool isVarArg) const;

  unsigned getInlineAsmMemConstraint(StringRef ConstraintCode) const override {
    if (ConstraintCode == "R")
      return InlineAsm::Constraint_R;
    else if (ConstraintCode == "ZC")
      return InlineAsm::Constraint_ZC;
    return TargetLowering::getInlineAsmMemConstraint(ConstraintCode);
  }
};

} // end namespace llvm

#endif /* LLVM_LIB_TARGET_XTENSA_XTENSAISELLOWERING_H */
