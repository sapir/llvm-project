; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-none-eabi -mattr=+mve -o - %s | FileCheck %s
; RUN: llc -mtriple=thumbv8.1m.main-none-eabi -mattr=+mve.fp -o - %s | FileCheck --check-prefix=CHECK --check-prefix=CHECK-FP %s

define <16 x i8> @vector_add_i8(<16 x i8> %lhs, <16 x i8> %rhs) {
; CHECK-LABEL: vector_add_i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov d1, r2, r3
; CHECK-NEXT:    vmov d0, r0, r1
; CHECK-NEXT:    mov r0, sp
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vadd.i8 q0, q0, q1
; CHECK-NEXT:    vmov r0, r1, d0
; CHECK-NEXT:    vmov r2, r3, d1
; CHECK-NEXT:    bx lr
entry:
  %sum = add <16 x i8> %lhs, %rhs
  ret <16 x i8> %sum
}

define <8 x i16> @vector_add_i16(<8 x i16> %lhs, <8 x i16> %rhs) {
; CHECK-LABEL: vector_add_i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov d1, r2, r3
; CHECK-NEXT:    vmov d0, r0, r1
; CHECK-NEXT:    mov r0, sp
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vadd.i16 q0, q0, q1
; CHECK-NEXT:    vmov r0, r1, d0
; CHECK-NEXT:    vmov r2, r3, d1
; CHECK-NEXT:    bx lr
entry:
  %sum = add <8 x i16> %lhs, %rhs
  ret <8 x i16> %sum
}

define <4 x i32> @vector_add_i32(<4 x i32> %lhs, <4 x i32> %rhs) {
; CHECK-LABEL: vector_add_i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov d1, r2, r3
; CHECK-NEXT:    vmov d0, r0, r1
; CHECK-NEXT:    mov r0, sp
; CHECK-NEXT:    vldrw.u32 q1, [r0]
; CHECK-NEXT:    vadd.i32 q0, q0, q1
; CHECK-NEXT:    vmov r0, r1, d0
; CHECK-NEXT:    vmov r2, r3, d1
; CHECK-NEXT:    bx lr
entry:
  %sum = add <4 x i32> %lhs, %rhs
  ret <4 x i32> %sum
}

define <8 x half> @vector_add_f16(<8 x half> %lhs, <8 x half> %rhs) {
; CHECK-FP-LABEL: vector_add_f16:
; CHECK-FP:       @ %bb.0: @ %entry
; CHECK-FP-NEXT:    vmov d1, r2, r3
; CHECK-FP-NEXT:    vmov d0, r0, r1
; CHECK-FP-NEXT:    mov r0, sp
; CHECK-FP-NEXT:    vldrw.u32 q1, [r0]
; CHECK-FP-NEXT:    vadd.f16 q0, q0, q1
; CHECK-FP-NEXT:    vmov r0, r1, d0
; CHECK-FP-NEXT:    vmov r2, r3, d1
; CHECK-FP-NEXT:    bx lr
entry:
  %sum = fadd <8 x half> %lhs, %rhs
  ret <8 x half> %sum
}

define <4 x float> @vector_add_f32(<4 x float> %lhs, <4 x float> %rhs) {
; CHECK-FP-LABEL: vector_add_f32:
; CHECK-FP:       @ %bb.0: @ %entry
; CHECK-FP-NEXT:    vmov d1, r2, r3
; CHECK-FP-NEXT:    vmov d0, r0, r1
; CHECK-FP-NEXT:    mov r0, sp
; CHECK-FP-NEXT:    vldrw.u32 q1, [r0]
; CHECK-FP-NEXT:    vadd.f32 q0, q0, q1
; CHECK-FP-NEXT:    vmov r0, r1, d0
; CHECK-FP-NEXT:    vmov r2, r3, d1
; CHECK-FP-NEXT:    bx lr
entry:
  %sum = fadd <4 x float> %lhs, %rhs
  ret <4 x float> %sum
}
