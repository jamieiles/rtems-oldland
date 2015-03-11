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

        .text
        .align 2

        .global SYM(_CPU_Context_switch)

SYM(_CPU_Context_switch):
	str32	$r0,  [$r0, 0x00]
	str32	$r1,  [$r0, 0x04]
	str32	$r2,  [$r0, 0x08]
	str32	$r3,  [$r0, 0x0c]
	str32	$r4,  [$r0, 0x10]
	str32	$r5,  [$r0, 0x14]
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
	gpsr	$r2
	str32	$r2,  [$r0, 0x40] /* PSR */
restore:
	ldr32	$r2,  [$r1, 0x40]
	spsr	$r2
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
	ldr32	$r5,  [$r1, 0x14]
	ldr32	$r4,  [$r1, 0x10]
	ldr32	$r3,  [$r1, 0x0c]
	ldr32	$r2,  [$r1, 0x08]
	ldr32	$r0,  [$r1, 0x00]
	ldr32	$r1,  [$r1, 0x04]
        ret

        .align 2

        .global SYM(_CPU_Context_restore)

SYM(_CPU_Context_restore):
        mov     $r1, $r0
        b	restore


/*
        VHandler for Vectored Interrupts

        All IRQ's are vectored to routine _ISR_#vector_number
        This routine stacks er0 and loads er0 with vector number
        before transferring to here

*/
        .align 2
        .global SYM(_ISR_Handler)
        .extern SYM(_Vector_table)


SYM(_ISR_Handler):
        bkp


/*
        Called from ISR_Handler as a way of ending IRQ
        but allowing dispatch to another task.
        Must use RTE as CCR is still on stack but IRQ has been serviced.
        CCR and PC occupy same word so rte can be used.
        now using task stack
*/

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
	b	__irq		/* IRQ */
	b	__ifetch_abort	/* IFETCH_ABORT */
	b	__data_abort	/* DATA_ABORT */

.macro	vector name, desc
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

vector	__reset, __reset_desc
vector	__illegal_instr, __illegal_instr_desc
vector __swi, __swi_desc
vector __irq, __irq_desc
vector __ifetch_abort, __ifetch_abort_desc
vector __data_abort, __data_abort_desc

__reset_desc: .asciz "RESET"
__illegal_instr_desc: .asciz "ILLEGAL INSTRUCTION"
__swi_desc: .asciz "SWI"
__irq_desc: .asciz "IRQ"
__ifetch_abort_desc: .asciz "INSTRUCTION FETCH ABORT"
__data_abort_desc: .asciz "DATA FETCH ABORT"
