/* -----------------------------------------------------------------------
   sysv.S - Copyright (c) 2004 Simon Posnjak

   CRIS Foreign Function Interface

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL CYGNUS SOLUTIONS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------- */

#define LIBFFI_ASM
#include <ffi.h>

	.text

	;; OK, when we get called we should have this (according to 
	;; AXIS ETRAX 100LX Programmer's Manual chapter 6.3)
	;; 
	;; R10:	 ffi_prep_args (func. pointer)
	;; R11:  &ecif
	;; R12:  cif->bytes
	;; R13:  fig->flags
	;; sp+0: ecif.rvalue
	;; sp+4: fn (function pointer to the function that we need to call)

	.globl  ffi_call_SYSV
	.type   ffi_call_SYSV,@function

ffi_call_SYSV:
	;; save the regs to the stack
	Push $srp
	push $r6
	push $r7
	push $r8
	push $r13
	move.d $sp,$r8
	move.d $sp,$r6
	
	;; move address of ffi_prep_args to r13
	move.d $r10, $r13

	;; make room on the stack for the args of fn
	sub.d  $r12, $sp
	
	;; void ffi_prep_args(char *stack, extended_cif *ecif)
	;; so:
	;; r10 <-- stack pointer
	;; r11 <-- &ecif (already there)
	move.d $sp,$r10
	
	;; load the ffi_prep_args function pointer into r13
	;; call the function
	jsr $r13
	
	;; save the size of the structures which are passed by stack
	move.d $r10, $r7
	
	;; move first four args in to r10..r13
	move.d [$sp+0], $r10
	move.d [$sp+4], $r11
	move.d [$sp+8], $r12
	move.d [$sp+12], $r13
	
	;; adjust the stack
	addq 16, $sp
	sub.d $r7, $r6
	cmp.d $sp, $r6
	
	bmi go_on_no_params_on_stack
	nop
	
        beq go_on_no_params_on_stack
        nop
	
	ba go_on
	nop
	
go_on_no_params_on_stack:
	move.d $r6, $sp
	
go_on:
	;; discover if we need to put rval address in to $r9
        move.d [$r8 + 0], $r7
	cmpq FFI_TYPE_STRUCT, $r7
	bne call_now
	nop
	
	move.d [$r8 + 20], $r9

call_now:
	;; move address of the function in to r7
	move.d [$r8 + 24], $r7
	
	;; call the function
	jsr $r7
	
	;; reset stack
	move.d $r8, $sp
	
	;; Load rval type in to r13
	pop $r13
	
	;; detect rval type
	cmpq FFI_TYPE_VOID, $r13
	beq epilogue
	nop

	cmpq FFI_TYPE_STRUCT, $r13
	beq epilogue
	nop
	
	cmpq FFI_TYPE_DOUBLE, $r13
	beq return_double_or_longlong
	nop

	cmpq FFI_TYPE_UINT64, $r13
	beq return_double_or_longlong
	nop

	cmpq FFI_TYPE_SINT64, $r13
	beq return_double_or_longlong
	nop

	;; just return the 32 bit value
	ba return
	nop
		
return_double_or_longlong:
	move.d [$sp+16], $r13
	move.d $r10, [$r13]
	addq 4, $r13
	move.d $r11, [$r13]
	ba epilogue
	nop	
	
return:
	move.d [$sp+16], $r13
	move.d $r10, [$r13]
	ba epilogue
	nop
		
epilogue:
	pop $r8
	pop $r7
	pop $r6
	Jump [$sp+]
	
        .size   ffi_call_SYSV,.-ffi_call_SYSV
