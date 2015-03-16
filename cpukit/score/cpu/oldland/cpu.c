/*
 *  Moxie CPU Dependent Source
 *
 *  COPYRIGHT (c) 2011 Anthony Green
 *
 *  Based on example code and other ports with this copyright:
 *
 *  COPYRIGHT (c) 1989-1999, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>

void _CPU_Initialize(void)
{
}

void _CPU_ISR_install_raw_handler(uint32_t vector, proc_ptr new_handler,
				  proc_ptr *old_handler)
{
}

void _CPU_ISR_install_vector(uint32_t vector, proc_ptr new_handler,
			     proc_ptr *old_handler)
{
	*old_handler = _ISR_Vector_table[vector];
	_CPU_ISR_install_raw_handler(vector, new_handler, old_handler);
	_ISR_Vector_table[vector] = new_handler;
}

void _CPU_Install_interrupt_stack(void)
{
}
