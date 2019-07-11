//===-- XtensaMCAsmBackend.cpp - Xtensa assembler backend ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaMCFixupKinds.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

namespace {
class XtensaMCAsmBackend : public MCAsmBackend {
  uint8_t OSABI;
  bool IsLittleEndian = true; // TODO: maybe big-endian machine support is also
                              // needed. Now default is little-endian machine
  bool IsCodeDensity = true;

public:
  XtensaMCAsmBackend(uint8_t osABI, bool hasFeatureDensity)
      : OSABI(osABI), IsCodeDensity(hasFeatureDensity) {}

  // Override MCAsmBackend
  unsigned getNumFixupKinds() const override {
    return Xtensa::NumTargetFixupKinds;
  }
  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;
  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved) const override;
  bool mayNeedRelaxation(const MCInst &Inst) const override;
  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *Fragment,
                            const MCAsmLayout &Layout) const override;
  void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
                        MCInst &Res) const override;
  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const override;

  std::unique_ptr<MCObjectWriter>
  createObjectWriter(raw_pwrite_stream &OS) const override {
    return createXtensaObjectWriter(OS, OSABI, IsLittleEndian);
  }
};
} // end anonymous namespace

const MCFixupKindInfo &
XtensaMCAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[Xtensa::NumTargetFixupKinds] = {
      // name                    offset bits  flags
      {"fixup_xtensa_branch_6", 0, 6, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_xtensa_branch_8", 0, 8, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_xtensa_branch_12", 0, 12, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_xtensa_jump_18", 0, 18, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_xtensa_call_18", 0, 18,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits},
      {"fixup_xtensa_l32r_16", 0, 16,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits},
      {"fixup_xtensa_tprel_l32r_16", 0, 16,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits},
      {"fixup_xtensa_plt_l32r_16", 0, 16,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits}};

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);
  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
    return Value;
  case Xtensa::fixup_xtensa_branch_6: {
    Value -= 4;
    if (!isInt<6>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    unsigned Hi2 = (Value >> 4) & 0x3;
    unsigned Lo4 = (Value)&0xf;
    return (Hi2 << 4) | (Lo4 << 12);
  }
  case Xtensa::fixup_xtensa_branch_8:
    Value -= 4;
    if (!isInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xff) << 16;
  case Xtensa::fixup_xtensa_branch_12:
    Value -= 4;
    if (!isInt<12>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xfff) << 12;
  case Xtensa::fixup_xtensa_jump_18:
    Value -= 4;
    if (!isInt<18>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0x3ffff) << 6;
  case Xtensa::fixup_xtensa_call_18:
    Value -= 1;
    if (!isInt<20>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0x3ffff) << 6;
  case Xtensa::fixup_xtensa_plt_l32r_16:
  case Xtensa::fixup_xtensa_tprel_l32r_16:
  case Xtensa::fixup_xtensa_l32r_16:
    if (!isInt<18>(Value) && (Value & 0x20000))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0x3fffc) << 6;
  }
}

static unsigned getSize(unsigned Kind) {
  switch (Kind) {
  default:
    return 3;
  case Xtensa::fixup_xtensa_branch_6:
    return 2;
  }
}

void XtensaMCAsmBackend::applyFixup(const MCAssembler &Asm,
                                    const MCFixup &Fixup, const MCValue &Target,
                                    MutableArrayRef<char> Data, uint64_t Value,
                                    bool IsResolved) const {
  MCContext &Ctx = Asm.getContext();
  Value = adjustFixupValue(Fixup, Value, Ctx);

  if (!Value)
    return; // Doesn't change encoding.

  unsigned Offset = Fixup.getOffset();
  unsigned FullSize = getSize(Fixup.getKind());

  for (unsigned i = 0; i != FullSize; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

bool XtensaMCAsmBackend::mayNeedRelaxation(const MCInst &Inst) const {
  return false;
}

bool XtensaMCAsmBackend::fixupNeedsRelaxation(
    const MCFixup &Fixup, uint64_t Value, const MCRelaxableFragment *Fragment,
    const MCAsmLayout &Layout) const {
  return false;
}

void XtensaMCAsmBackend::relaxInstruction(const MCInst &Inst,
                                          const MCSubtargetInfo &STI,
                                          MCInst &Res) const {}

bool XtensaMCAsmBackend::writeNopData(uint64_t Count,
                                      MCObjectWriter *OW) const {
  uint64_t NumNops16b = 0;

  if (IsCodeDensity && (Count > 1)) {
    NumNops16b = (Count >> 1) - (Count & 0x1);
  }

  // Fill data with 16b NOP instructions first if possible
  for (uint64_t i = 0; i != NumNops16b; ++i) {
    //Currently just little-endian machine supported, 
    //but probably big-endian will be also implemented in future
    if (IsLittleEndian) {
      OW->write8(0x3d);
      OW->write8(0xf0);
    } else {
      OW->write8(0xf0);
      OW->write8(0x3d);
    }
    Count -= 2;
  }

  uint64_t NumNops24b = Count / 3;

  for (uint64_t i = 0; i != NumNops24b; ++i) {
    //Currently just little-endian machine supported, 
    //but probably big-endian will be also implemented in future
    if (IsLittleEndian) {
      OW->write8(0xf0);
      OW->write8(0x20);
      OW->write8(0x00);
    } else {
      OW->write8(0x00);
      OW->write8(0x20);
      OW->write8(0xf0);
    }
    Count -= 3;
  }

  // TODO maybe function should return error if (Count > 0)
  switch (Count) {
  default:
    break;
  case 1:
    OW->write8(0);
    break;
  case 2:
    OW->write16(0);
    break;
  }

  return true;
}

MCAsmBackend *llvm::createXtensaMCAsmBackend(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             const MCRegisterInfo &MRI,
                                             const MCTargetOptions &Options) {
  uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  bool hasFeatureDensity = STI.getFeatureBits()[Xtensa::FeatureDensity];
  return new XtensaMCAsmBackend(OSABI, hasFeatureDensity);
}
