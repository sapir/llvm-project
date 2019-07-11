//===- XtensaInstPrinter.cpp - Convert Xtensa MCInst to assembly syntax --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------------===//
//
// This class prints an Xtensa MCInst to a .s file.
//
//===---------------------------------------------------------------------------===//

#include "XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaMCExpr.h"
#include "XtensaInstrInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#include "XtensaGenAsmWriter.inc"

void XtensaInstPrinter::printAddress(unsigned Base, int64_t Disp,
                                     raw_ostream &O) {
  O << Disp;
  if (Base) {
    O << '(';
    O << getRegisterName(Base) << ')';
  }
}

static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  } else if (const XtensaMCExpr *XE = dyn_cast<XtensaMCExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(XE->getSubExpr());
  } else if (!(SRE = dyn_cast<MCSymbolRefExpr>(Expr)))
    assert(false && "Unexpected MCExpr type.");

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
  case MCSymbolRefExpr::VK_None:
    break;
  // TODO
  default:
    llvm_unreachable("Invalid kind!");
  }

  OS << SRE->getSymbol();

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }

  if (Kind != MCSymbolRefExpr::VK_None)
    OS << ')';
}

void XtensaInstPrinter::printOperand(const MCOperand &MC, raw_ostream &O) {
  if (MC.isReg())
    O << getRegisterName(MC.getReg());
  else if (MC.isImm())
    O << MC.getImm();
  else if (MC.isExpr())
    printExpr(MC.getExpr(), O);
  else
    llvm_unreachable("Invalid operand");
}

void XtensaInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                  StringRef Annot, const MCSubtargetInfo &STI) {
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void XtensaInstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
  O << getRegisterName(RegNo);
}

void XtensaInstPrinter::printMemOperand(const MCInst *MI, int opNum,
                                        raw_ostream &OS) {
  // OS << "(";
  OS << getRegisterName(MI->getOperand(opNum + 1).getReg());
  // OS << ")";
  OS << ", ";
  printOperand(MI, opNum, OS);
}

void XtensaInstPrinter::printBranchTarget(const MCInst *MI, int opNum,
                                          raw_ostream &OS) {
  if (MI->getOperand(opNum).isImm())
    OS << ".+"; // constant branch
  printOperand(MI, opNum, OS);
}

void XtensaInstPrinter::printJumpTarget(const MCInst *MI, int opNum,
                                        raw_ostream &OS) {
  if (MI->getOperand(opNum).isImm())
    OS << ".+"; // constant branch
  printOperand(MI, opNum, OS);
}

void XtensaInstPrinter::printMemRegOperand(const MCInst *MI, int opNum,
                                           raw_ostream &OS) {
  //     OS << "0"; //No offset for this ever
  //     OS << "(";
  OS << getRegisterName(MI->getOperand(opNum).getReg());
  //     OS << ")";
  OS << ", 0";
}

void XtensaInstPrinter::printAccessRegOperand(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  uint64_t Value = MI->getOperand(OpNum).getImm();
  assert((Value < 16) && "Invalid access register number");
  O << "%a" << (unsigned int)Value;
}

void XtensaInstPrinter::printCallOperand(const MCInst *MI, int OpNum,
                                         raw_ostream &O) {
  printOperand(MI, OpNum, O);
  // O << "@PLT";
}

void XtensaInstPrinter::printOperand(const MCInst *MI, int OpNum,
                                     raw_ostream &O) {
  printOperand(MI->getOperand(OpNum), O);
}

void XtensaInstPrinter::printImm8_AsmOperand(const MCInst *MI, int OpNum,
                                             raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= -128 && Value <= 127) && "Invalid imm8 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printImm8_sh8_AsmOperand(const MCInst *MI, int OpNum,
                                                 raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= -32768 && Value <= 32512 && ((Value & 0xFF) == 0)) &&
           "Invalid imm8_sh8 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printImm7n_AsmOperand(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= -32 && Value <= 95) && "Invalid imm7n argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printImm12_AsmOperand(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= -2048 && Value <= 2047) && "Invalid imm12 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printEntry_Imm12_AsmOperand(const MCInst *MI, int OpNum,
                                                    raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 32760) && "Invalid entry_imm12 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printImmn_AsmOperand(const MCInst *MI, int OpNum,
                                             raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= -1 && (Value != 0) && Value <= 15) &&
           "Invalid immn argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printShimm4_AsmOperand(const MCInst *MI, int OpNum,
                                               raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 15) && "Invalid shimm4 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printShimm5_AsmOperand(const MCInst *MI, int OpNum,
                                               raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 31) && "Invalid shimm5 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printShimm16_31_AsmOperand(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 16 && Value <= 31) && "Invalid shimm16_31 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printShimm1_16_AsmOperand(const MCInst *MI, int OpNum,
                                                  raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 1 && Value <= 16) && "Invalid ahimm1_16 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printShimm17_31_AsmOperand(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 17 && Value <= 31) && "Invalid shimm17_31 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printOffset8m8_AsmOperand(const MCInst *MI, int OpNum,
                                                  raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 255) && "Invalid offset8m8 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printOffset8m16_AsmOperand(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 510 && ((Value & 0x1) == 0)) &&
           "Invalid offset8m16 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printOffset8m32_AsmOperand(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 1020 && ((Value & 0x3) == 0)) &&
           "Invalid offset8m32 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printOffset4m32_AsmOperand(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 0 && Value <= 60 && ((Value & 0x3) == 0)) &&
           "Invalid offset4m32 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printB4const_AsmOperand(const MCInst *MI, int OpNum,
                                                raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();

    switch (Value) {
    case -1:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 12:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
      break;
    default:
      assert((0) && "Invalid B4const argument");
    }
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printB4constu_AsmOperand(const MCInst *MI, int OpNum,
                                                 raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();

    switch (Value) {
    case 32768:
    case 65536:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
    case 12:
    case 16:
    case 32:
    case 64:
    case 128:
    case 256:
      break;
    default:
      assert((0) && "Invalid B4constu argument");
    }
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}

void XtensaInstPrinter::printSeimm7_22_AsmOperand(const MCInst *MI, int OpNum,
                                                  raw_ostream &O) {
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Value = MI->getOperand(OpNum).getImm();
    assert((Value >= 7 && Value <= 22) && "Invalid seimm7_22 argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}
