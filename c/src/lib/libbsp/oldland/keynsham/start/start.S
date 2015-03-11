/* Copyright (C) 2015, Jamie Iles */

/* oldland start up file. */

#include "bspopts.h"

	.globl	__start
	.weak	_start
	.text
	.type	__start,@function
__start:
_start:
	movhi	$sp, %hi(_stack)	/* set the top of stack */
	orlo	$sp, $sp, %lo(_stack)
	mov	$fp, 1

	mov	$r3, 0
	movhi	$r4, %hi(__bss_start__)
	orlo	$r4, $r4, %lo(__bss_start__)
	movhi	$r5, %hi(__bss_end__)
	orlo	$r5, $r5, %lo(__bss_end__)
_clear_bss:
	str8	$r3, [$r4, 0]
	cmp	$r4, $r5
	beq	1f
	add	$r4, $r4, 1
	b	_clear_bss
1:
	movhi	$r0, %hi(__ex_table)
	orlo	$r0, $r0, %lo(__ex_table)
	scr	0, $r0

	mov	$r0, 0
	call	boot_card
	b	_start
.Lend:
	.size	__start,(.Lend-__start)
