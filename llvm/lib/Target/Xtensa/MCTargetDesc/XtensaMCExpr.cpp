//===-- XtensaMCExpr.cpp - Xtensa specific MC expression classes ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the Xtensa architecture
//
//===----------------------------------------------------------------------===//

#include "XtensaMCExpr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "xtensamcexpr"

const XtensaMCExpr *XtensaMCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                         MCContext &Ctx) {
  return new (Ctx) XtensaMCExpr(Expr, Kind);
}

void XtensaMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  bool HasVariant = getKind() != VK_Xtensa_None;
  if (HasVariant)
    OS << '%' << getVariantKindName(getKind()) << '(';
  Expr->print(OS, MAI);
  if (HasVariant)
    OS << ')';
}

bool XtensaMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                             const MCAsmLayout *Layout,
                                             const MCFixup *Fixup) const {
  return getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup);
}

void XtensaMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

XtensaMCExpr::VariantKind XtensaMCExpr::getVariantKindForName(StringRef name) {
  return StringSwitch<XtensaMCExpr::VariantKind>(name).Default(
      VK_Xtensa_Invalid);
}

StringRef XtensaMCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  }
}
