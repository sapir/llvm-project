//===- XtensaISelDAGToDAG.cpp - A dag to dag inst selector for Xtensa -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the Xtensa target.
//
//===----------------------------------------------------------------------===//

#include "XtensaTargetMachine.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "xtensa-isel"

namespace {
// Used to build addressing modes.
struct XtensaAddressingMode {
  // The shape of the address.
  enum AddrForm {
    // base+offset
    FormBO
  };
  AddrForm Form;

  // The type of displacement.
  enum OffRange { Off12Only };
  OffRange OffR;

  // The parts of the address.  The address is equivalent to:
  //
  //     Base + Offset + Index + (IncludesDynAlloc ? ADJDYNALLOC : 0)
  SDValue Base;
  int64_t Offset;

  XtensaAddressingMode(AddrForm form, OffRange offr)
      : Form(form), OffR(offr), Base(), Offset(0) {}

  void dump() {
    errs() << "XtensaAddressingMode " << this << '\n';

    errs() << " Base ";
    if (Base.getNode() != 0)
      Base.getNode()->dump();
    else
      errs() << "null\n";

    errs() << " Offset " << Offset;
  }
};

class XtensaDAGToDAGISel : public SelectionDAGISel {
  const XtensaTargetLowering &Lowering;
  const XtensaSubtarget *Subtarget;

  // Used by XtensaOperands.td to create integer constants.
  inline SDValue getImm(const SDNode *Node, uint64_t Imm) {
    return CurDAG->getTargetConstant(Imm, SDLoc(Node), Node->getValueType(0));
  }
  /// getI32Imm - Return a target constant with the specified value, of type
  /// i32.
  SDValue getI32Imm(unsigned Imm, SDLoc DL) {
    return CurDAG->getTargetConstant(Imm, DL, MVT::i32);
  }

  bool selectMemRegAddr(SDValue Addr, SDValue &Offset, SDValue &Base,
                        int Scale) {
    EVT ValTy = Addr.getValueType();

    // if Address is FI, get the TargetFrameIndex.
    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
      Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
      Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), ValTy);

      return true;
    }

    if (TM.isPositionIndependent())
      report_fatal_error("PIC relocations is not supported");

    if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
           Addr.getOpcode() == ISD::TargetGlobalAddress))
        return false;

    // Addresses of the form FI+const or FI|const
    bool Valid = false;
    if (CurDAG->isBaseWithConstantOffset(Addr)) {
      ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
      int64_t OffsetVal = CN->getSExtValue();

      switch (Scale) {
      case 1:
        Valid = (OffsetVal >= 0 && OffsetVal <= 255);
        break;
      case 2:
        Valid =
            (OffsetVal >= 0 && OffsetVal <= 510) && ((OffsetVal & 0x1) == 0);
        break;
      case 4:
        Valid =
            (OffsetVal >= 0 && OffsetVal <= 1020) && ((OffsetVal & 0x3) == 0);
        break;
      default:
        break;
      }

      if (Valid) {
        // If the first operand is a FI, get the TargetFI Node
        if (FrameIndexSDNode *FIN =
                dyn_cast<FrameIndexSDNode>(Addr.getOperand(0)))
          Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
        else
          Base = Addr.getOperand(0);

        Offset =
            CurDAG->getTargetConstant(CN->getZExtValue(), SDLoc(Addr), ValTy);
        return true;
      }
    }

    // Last case
    Base = Addr;
    Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), Addr.getValueType());
    return true;
  }

  bool selectMemRegAddrISH1(SDValue Addr, SDValue &Offset, SDValue &Base) {
    return selectMemRegAddr(Addr, Offset, Base, 1);
  }

  bool selectMemRegAddrISH2(SDValue Addr, SDValue &Offset, SDValue &Base) {
    return selectMemRegAddr(Addr, Offset, Base, 2);
  }

  bool selectMemRegAddrISH4(SDValue Addr, SDValue &Offset, SDValue &Base) {
    return selectMemRegAddr(Addr, Offset, Base, 4);
  }

  bool selectMemRegAddrN(SDValue Addr, SDValue &Offset, SDValue &Base) {
    EVT ValTy = Addr.getValueType();
    if (ValTy.getScalarSizeInBits() != 32)
      return false;

    // if Address is FI, get the TargetFrameIndex.
    //    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    //      Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
    //      Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), ValTy);
    //      return true;
    //    }

    if (TM.isPositionIndependent())
      report_fatal_error("PIC relocations is not supported");

    if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
       Addr.getOpcode() == ISD::TargetGlobalAddress))
      return false;
 
    // Addresses of the form FI+const or FI|const
    if (CurDAG->isBaseWithConstantOffset(Addr)) {
      ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
      int64_t OffsetVal = CN->getSExtValue();

      if (OffsetVal >= 0 && OffsetVal <= 60) {

        // If the first operand is a FI, get the TargetFI Node
        //        if (FrameIndexSDNode *FIN =
        //                dyn_cast<FrameIndexSDNode>(Addr.getOperand(0)))
        //          Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
        //        else
        Base = Addr.getOperand(0);

        Offset =
            CurDAG->getTargetConstant(CN->getZExtValue(), SDLoc(Addr), ValTy);
        return true;
      }
    }

    // Last case
    //  Base = Addr;
    //  Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), Addr.getValueType());
    return false;
  }

  bool selectFrameIndex(SDValue Addr, SDValue &Offset, SDValue &Base) {
    EVT ValTy = Addr.getValueType();

    // if Address is FI, get the TargetFrameIndex.
    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
      //      MachineFrameInfo& MFI = MF->getFrameInfo();

      Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
      Offset = CurDAG->getTargetConstant(0, SDLoc(Addr), ValTy);

      return true;
    }
    if (CurDAG->isBaseWithConstantOffset(Addr)) {
      ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
      // int64_t OffsetVal = CN->getSExtValue();

      // If the first operand is a FI, get the TargetFI Node
      if (FrameIndexSDNode *FIN =
              dyn_cast<FrameIndexSDNode>(Addr.getOperand(0)))
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
      else
        Base = Addr.getOperand(0);

      Offset =
          CurDAG->getTargetConstant(CN->getZExtValue(), SDLoc(Addr), ValTy);
      return true;
    }
    return false;
  }

  bool selectRegAddr(SDValue Addr, SDValue &Base) {
    // always just register
    Base = Addr;
    return true;
  }

  // PC-relative address matching routines used by XtensaOperands.td.
  bool selectPCRelAddress(SDValue Addr, SDValue &Target) {
    if (Addr.getOpcode() == XtensaISD::PCREL_WRAPPER) {
      Target = Addr.getOperand(0);
      return true;
    }
    return false;
  }

public:
  XtensaDAGToDAGISel(XtensaTargetMachine &TM, CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(TM, OptLevel),
        Lowering(*TM.getSubtargetImpl()->getTargetLowering()),
        Subtarget(TM.getSubtargetImpl()) {}

  // Override MachineFunctionPass.
  StringRef getPassName() const override {
    return "Xtensa DAG->DAG Pattern Instruction Selection";
  }

  // Override SelectionDAGISel.
  virtual bool runOnMachineFunction(MachineFunction &MF);
  void Select(SDNode *Node) override;
  virtual void processFunctionAfterISel(MachineFunction &MF);
  bool SelectInlineAsmMemoryOperand(const SDValue &Op, unsigned ConstraintID,
                                    std::vector<SDValue> &OutOps) override;

// Include the pieces autogenerated from the target description.
#include "XtensaGenDAGISel.inc"
}; // namespace
} // end anonymous namespace

bool XtensaDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  bool ret = SelectionDAGISel::runOnMachineFunction(MF);

  processFunctionAfterISel(MF);

  return ret;
}

FunctionPass *llvm::createXtensaISelDag(XtensaTargetMachine &TM,
                                        CodeGenOpt::Level OptLevel) {
  return new XtensaDAGToDAGISel(TM, OptLevel);
}

void XtensaDAGToDAGISel::Select(SDNode *Node) {
  SDLoc DL(Node);
  // Dump information about the Node being selected
  DEBUG(errs() << "Selecting: "; Node->dump(CurDAG); errs() << "\n");

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    return;
  }

  SelectCode(Node);
}

bool XtensaDAGToDAGISel::SelectInlineAsmMemoryOperand(
    const SDValue &Op, unsigned ConstraintID, std::vector<SDValue> &OutOps) {
  switch (ConstraintID) {
  default:
    llvm_unreachable("Unexpected asm memory constraint");
  case InlineAsm::Constraint_m: {
    SDValue Base, Offset;
    // TODO
    selectMemRegAddr(Op, Base, Offset, 4);
    OutOps.push_back(Base);
    OutOps.push_back(Offset);
    return false;
  }
  case InlineAsm::Constraint_i:
  case InlineAsm::Constraint_R:
  case InlineAsm::Constraint_ZC:
    OutOps.push_back(Op);
    return false;
  }
  return false;
}

void XtensaDAGToDAGISel::processFunctionAfterISel(MachineFunction &MF) {
  /*for (auto &MBB : MF) {
    for (auto &I : MBB) {
      // TODO something useful for future
    }
  }*/
}
