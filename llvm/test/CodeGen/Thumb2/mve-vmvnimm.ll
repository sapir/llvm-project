; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-arm-none-eabi -mattr=+mve -verify-machineinstrs %s -o - | FileCheck %s
; RUN: llc -mtriple=thumbv8.1m.main-arm-none-eabi -mattr=+mve.fp -verify-machineinstrs %s -o - | FileCheck %s

define arm_aapcs_vfpcc <8 x i16> @mov_int16_511(i16 *%dest) {
; CHECK-LABEL: mov_int16_511:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i16 q0, #0xfe00
; CHECK-NEXT:    bx lr
entry:
  ret <8 x i16> <i16 511, i16 511, i16 511, i16 511, i16 511, i16 511, i16 511, i16 511>
}

define arm_aapcs_vfpcc <8 x i16> @mov_int16_65281(i16 *%dest) {
; CHECK-LABEL: mov_int16_65281:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i16 q0, #0xfe
; CHECK-NEXT:    bx lr
entry:
  ret <8 x i16> <i16 65281, i16 65281, i16 65281, i16 65281, i16 65281, i16 65281, i16 65281, i16 65281>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_m7(i32 *%dest) {
; CHECK-LABEL: mov_int32_m7:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i32 q0, #0x6
; CHECK-NEXT:    bx lr
entry:
  ret <4 x i32> <i32 -7, i32 -7, i32 -7, i32 -7>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_m769(i32 *%dest) {
; CHECK-LABEL: mov_int32_m769:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i32 q0, #0x300
; CHECK-NEXT:    bx lr
entry:
  ret <4 x i32> <i32 -769, i32 -769, i32 -769, i32 -769>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_m262145(i32 *%dest) {
; CHECK-LABEL: mov_int32_m262145:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i32 q0, #0x40000
; CHECK-NEXT:    bx lr
entry:
  ret <4 x i32> <i32 -262145, i32 -262145, i32 -262145, i32 -262145>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_m134217729(i32 *%dest) {
; CHECK-LABEL: mov_int32_m134217729:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i32 q0, #0x8000000
; CHECK-NEXT:    bx lr
entry:
  ret <4 x i32> <i32 -134217729, i32 -134217729, i32 -134217729, i32 -134217729>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_4294902528(i32 *%dest) {
; CHECK-LABEL: mov_int32_4294902528:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmvn.i32 q0, #0xfcff
; CHECK-NEXT:    bx lr
entry:
  ret <4 x i32> <i32 4294902528, i32 4294902528, i32 4294902528, i32 4294902528>
}

define arm_aapcs_vfpcc <4 x i32> @mov_int32_4278386688(i32 *%dest) {
; CHECK-LABEL: mov_int32_4278386688:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    adr r0, .LCPI7_0
; CHECK-NEXT:    vldrw.u32 q0, [r0]
; CHECK-NEXT:    bx lr
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  @ %bb.1:
; CHECK-NEXT:  .LCPI7_0:
; CHECK-NEXT:    .long 4278386688 @ double -6.5147775434702224E+303
; CHECK-NEXT:    .long 4278386688
; CHECK-NEXT:    .long 4278386688 @ double -6.5147775434702224E+303
; CHECK-NEXT:    .long 4278386688
entry:
  ret <4 x i32> <i32 4278386688, i32 4278386688, i32 4278386688, i32 4278386688>
}
