/*
 * Copyright (c) 2013  Anthony Green
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>

void oldland_unhandled_exception(const CPU_Exception_frame *frame,
				 const char *name);

void _CPU_Exception_frame_print(const CPU_Exception_frame *frame)
{
}

void oldland_unhandled_exception(const CPU_Exception_frame *frame,
				 const char *name)
{
	int i;

	printk("Fatal exception (%s)\n", name);
	printk("FAR:  %08x\n", frame->far);
	printk("DFAR: %08x\n", frame->dfar);
	printk("PSR:  %08x\n", frame->psr);
	for (i = 0; i < 10; ++i)
		printk(" $r%d: %08x\n", i, frame->integer_registers[i]);
	for (i = 10; i < 13; ++i)
		printk("$r%d: %08x\n", i, frame->integer_registers[i]);
	printk(" $fp: %08x\n", frame->integer_registers[13]);
	printk(" $sp: %08x\n", frame->integer_registers[14]);
	printk(" $lr: %08x\n", frame->integer_registers[15]);

	for (;;)
		continue;
}
