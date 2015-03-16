/*
 * Oldland CPU functions
 *   Copyright (C) 2015 Jamie Iles
 *
 *  Based on example code and other ports with this copyright:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>

/*
 * 128 bytes for context and 32 bytes for red zone.
 */
#define RESERVED_CONTEXT_SIZE (128 + 32)

        .text
        .align 2

        .global SYM(_CPU_Context_switch)

SYM(_CPU_Context_switch):
	str32	$r6,  [$r0, 0x18]
	str32	$r7,  [$r0, 0x1c]
	str32	$r8,  [$r0, 0x20]
	str32	$r9,  [$r0, 0x24]
	str32	$r10, [$r0, 0x28]
	str32	$r11, [$r0, 0x2c]
	str32	$r12, [$r0, 0x30]
	str32	$fp,  [$r0, 0x34]
	str32	$sp,  [$r0, 0x38]
	str32	$lr,  [$r0, 0x3c]
	gcr	$r2, 1
	str32	$r2,  [$r0, 0x40] /* PSR */
restore:
	ldr32	$lr,  [$r1, 0x3c]
	ldr32	$sp,  [$r1, 0x38]
	ldr32	$fp,  [$r1, 0x34]
	ldr32	$r12, [$r1, 0x30]
	ldr32	$r11, [$r1, 0x2c]
	ldr32	$r10, [$r1, 0x28]
	ldr32	$r9,  [$r1, 0x24]
	ldr32	$r8,  [$r1, 0x20]
	ldr32	$r7,  [$r1, 0x1c]
	ldr32	$r6,  [$r1, 0x18]
	ldr32	$r0,  [$r1, 0x40]
	scr	1, $r0
        ret

        .align 2

        .global SYM(_CPU_Context_restore)

SYM(_CPU_Context_restore):
        mov     $r1, $r0
        b	restore

        .align 2
        .global SYM(_ISR_Handler)
        .extern SYM(_Vector_table)
SYM(_ISR_Handler):
	/*
	 * Save all of the current state onto the current task's stack.
	 *
	 * We need to store:
	 *  - the 16 GPR's
	 *  - SPSR
	 *  - FAR
	 *
	 * with a total of 72 bytes + 8 bytes padding for when we interrupt
	 * the prologue of a C function and the saved $fp + $lr are being
	 * written but $sp hasn't yet been decremented..
	 */
	sub	$sp, $sp, RESERVED_CONTEXT_SIZE
	str32	$r0,  [$sp, 0x00]
	str32	$r1,  [$sp, 0x04]
	str32	$r2,  [$sp, 0x08]
	str32	$r3,  [$sp, 0x0c]
	str32	$r4,  [$sp, 0x10]
	str32	$r5,  [$sp, 0x14]
	str32	$r6,  [$sp, 0x18]
	str32	$r7,  [$sp, 0x1c]
	str32	$r8,  [$sp, 0x20]
	str32	$r9,  [$sp, 0x24]
	str32	$r10, [$sp, 0x28]
	str32	$r11, [$sp, 0x2c]
	str32	$r12, [$sp, 0x30]
	str32	$fp,  [$sp, 0x34]
	str32	$sp,  [$sp, 0x38]
	str32	$lr,  [$sp, 0x3c]
	gcr	$r0, 1
	str32	$r0,  [$sp, 0x40] /* PSR. */
	gcr	$r0, 2
	str32	$r0,  [$sp, 0x44] /* Saved PSR. */
	gcr	$r0, 3
	str32	$r0,  [$sp, 0x48] /* FAR. */
	mov	$r12, $sp	/* $r12 (callee save) now contains the
				   interrupted stack pointer to restore. */

	/* Increment the nest level. */
	movhi	$r10, %hi(ISR_NEST_LEVEL)
	orlo	$r10, $r10, %lo(ISR_NEST_LEVEL)
	ldr32	$r2, [$r10, 0]
	add	$r2, $r2, 1
	str32	$r2, [$r10, 0]

	/* Increment thread dispatch disable level. */
	movhi	$r11, %hi(THREAD_DISPATCH_DISABLE_LEVEL)
	orlo	$r11, $r11, %lo(THREAD_DISPATCH_DISABLE_LEVEL)
	ldr32	$r3, [$r11, 0]
	add	$r3, $r3, 1
	str32	$r3, [$r11, 0]

	/*
	 * If we aren't in a nested interrupt then switch into the software
	 * interrupt stack and save the interrupted stack pointer onto the
	 * interrupt stack.
	 */
	cmp	$r2, 1
	bne	1f
	movhi	$r2, %hi(INTERRUPT_STACK_HIGH)
	orlo	$r2, $r2, %lo(INTERRUPT_STACK_HIGH)
	ldr32	$sp, [$r2, 0]
1:
	/* Process the interrupt. */
	call	bsp_interrupt_dispatch

	/* Decrement the nest level. */
	ldr32	$r1, [$r10, 0]
	sub	$r1, $r1, 1
	str32	$r1, [$r10, 0]

	/* Decrement thread dispatch disable level. */
	ldr32	$r3, [$r11, 0]
	sub	$r3, $r3, 1
	str32	$r3, [$r11, 0]

	/* Return to the interrupted stack. */
	mov	$sp, $r12

	/*
	 * If ISR_NEST_LEVEL > 0 || THREAD_DISPATCH_DISABLE_LEVEL > 0
	 * then restore and return from interrupt.
	 */
	cmp	$r1, 0
	bne	irq_restore
	cmp	$r3, 0
	bne	irq_restore

	/* If dispatch is needed then dispatch the new thread. */
	movhi	$r0, %hi(DISPATCH_NEEDED)
	orlo	$r0, $r0, %lo(DISPATCH_NEEDED)
	ldr32	$r1, [$r0, 0]
	cmp	$r1, 0
	beq	irq_restore

	call	_Thread_Dispatch

irq_restore:
	ldr32	$r1,  [$sp, 0x04]
	ldr32	$r2,  [$sp, 0x08]
	ldr32	$r3,  [$sp, 0x0c]
	ldr32	$r4,  [$sp, 0x10]
	ldr32	$r5,  [$sp, 0x14]
	ldr32	$r6,  [$sp, 0x18]
	ldr32	$r7,  [$sp, 0x1c]
	ldr32	$r8,  [$sp, 0x20]
	ldr32	$r9,  [$sp, 0x24]
	ldr32	$r10, [$sp, 0x28]
	ldr32	$r11, [$sp, 0x2c]
	ldr32	$r12, [$sp, 0x30]
	ldr32	$fp,  [$sp, 0x34]
	/* No need to restore $sp. */
	ldr32	$lr,  [$sp, 0x3c]
	ldr32	$r0,  [$sp, 0x40] /* PSR. */
	scr	1, $r0
	ldr32	$r0,  [$sp, 0x44] /* Saved PSR. */
	scr	2, $r0
	ldr32	$r0,  [$sp, 0x48] /* FAR. */
	scr	3, $r0
	/* Load clobbered $r0. */
	ldr32	$r0,  [$sp, 0x00]
	add	$sp, $sp, RESERVED_CONTEXT_SIZE

irq_restore_done:
	rfe

        .align 2
        .global SYM(_ISR_Dispatch)
SYM(_ISR_Dispatch):
        bkp

        .align 2
        .global SYM(_CPU_Context_save_fp)
SYM(_CPU_Context_save_fp):
        bkp


        .align 2
        .global SYM(_CPU_Context_restore_fp)
SYM(_CPU_Context_restore_fp):
        bkp

	.balign	64
	.globl __ex_table
__ex_table:
	b	__reset		/* RESET */
	b	__illegal_instr	/* ILLEGAL_INSTR */
	b	__swi		/* SWI */
	b	_ISR_Handler	/* IRQ */
	b	__ifetch_abort	/* IFETCH_ABORT */
	b	__data_abort	/* DATA_ABORT */

.macro	unhandled_vector name, desc
\name:
	sub	$sp, $sp, 0x4c
	str32	$r0,  [$sp, 0x00]
	str32	$r1,  [$sp, 0x04]
	str32	$r2,  [$sp, 0x08]
	str32	$r3,  [$sp, 0x0c]
	str32	$r4,  [$sp, 0x10]
	str32	$r5,  [$sp, 0x14]
	str32	$r6,  [$sp, 0x18]
	str32	$r7,  [$sp, 0x1c]
	str32	$r8,  [$sp, 0x20]
	str32	$r9,  [$sp, 0x24]
	str32	$r11, [$sp, 0x28]
	str32	$r12, [$sp, 0x2c]
	str32	$fp,  [$sp, 0x30]
	str32	$sp,  [$sp, 0x34]
	str32	$lr,  [$sp, 0x38]
	gpsr	$r2
	str32	$r2,  [$sp, 0x3c] /* PSR */
	gcr	$r2, 2
	str32	$r2,  [$sp, 0x40]
	gcr	$r2, 3
	str32	$r2,  [$sp, 0x44]
	mov	$r0, $sp
	movhi	$r1, %hi(\desc)
	orlo	$r1, $r1, %lo(\desc)
	call	oldland_unhandled_exception
.endm

unhandled_vector __reset, __reset_desc
unhandled_vector __illegal_instr, __illegal_instr_desc
unhandled_vector __swi, __swi_desc
unhandled_vector __ifetch_abort, __ifetch_abort_desc
unhandled_vector __data_abort, __data_abort_desc

__reset_desc: .asciz "RESET"
__illegal_instr_desc: .asciz "ILLEGAL INSTRUCTION"
__swi_desc: .asciz "SWI"
__ifetch_abort_desc: .asciz "INSTRUCTION FETCH ABORT"
__data_abort_desc: .asciz "DATA FETCH ABORT"
