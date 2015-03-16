/**
 * @file rtems/score/cpu.h
 */

/*
 *  This include file contains information pertaining to the Oldland
 *  processor.
 *
 *  Copyright (c) 2013  Anthony Green
 *  Copyright (c) 2015  Jamie Iles
 *
 *  Based on code with the following copyright..
 *  COPYRIGHT (c) 1989-2006, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#define CPU_SIZEOF_POINTER         4
#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>
#include <rtems/score/oldland.h>  /* pick up machine definitions */

#include <rtems/bspIo.h>        /* printk */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE
#define RTEMS_USE_32_BIT_OBJECT
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE
#define CPU_ALLOCATE_INTERRUPT_STACK TRUE
#define CPU_HARDWARE_FP     FALSE
#define CPU_ALL_TASKS_ARE_FP     FALSE
#define CPU_IDLE_TASK_IS_FP      FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE
#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE
#define CPU_STACK_GROWS_UP               FALSE
#define CPU_STRUCTURE_ALIGNMENT

#define CPU_TIMESTAMP_USE_STRUCT_TIMESPEC TRUE
#define CPU_TIMESTAMP_USE_INT64 FALSE
#define CPU_TIMESTAMP_USE_INT64_INLINE FALSE

#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE

#define CPU_MODES_INTERRUPT_MASK   0x00000001

typedef struct {
} CPU_Per_CPU_control;

#define nogap __attribute__ ((packed))

typedef struct {
    uint32_t    r0 nogap;   /* 0x00 */
    uint32_t    r1 nogap;   /* 0x04 */
    uint32_t    r2 nogap;   /* 0x08 */
    uint32_t    r3 nogap;   /* 0x0c */
    uint32_t    r4 nogap;   /* 0x10 */
    uint32_t    r5 nogap;   /* 0x14 */
    uint32_t    r6 nogap;   /* 0x18 */
    uint32_t    r7 nogap;   /* 0x1c */
    uint32_t    r8 nogap;   /* 0x10 */
    uint32_t    r9 nogap;   /* 0x14 */
    uint32_t    r10 nogap;  /* 0x18 */
    uint32_t    r11 nogap;  /* 0x1c */
    uint32_t    r12 nogap;  /* 0x30 */
    uint32_t    fp nogap;   /* 0x34 */
    uint32_t    sp nogap;   /* 0x38 */
    uint32_t    lr nogap;   /* 0x3c */
    uint32_t    psr nogap;  /* 0x40 */
    uint32_t	spsr nogap; /* 0x44 */
    uint32_t	far nogap;  /* 0x48 */
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->sp

typedef struct {
} Context_Control_fp;

typedef struct {
    uint32_t   special_interrupt_register;
} CPU_Interrupt_frame;

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context;

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK	0
#define CPU_INTERRUPT_NUMBER_OF_VECTORS		32
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER \
    (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE
#define CPU_STACK_MINIMUM_SIZE          (4096)
#define CPU_ALIGNMENT              8
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT
#define CPU_STACK_ALIGNMENT        0

#define _CPU_Initialize_vectors()

#define CR1_IRQ_ENABLED (1 << 4)

#define _CPU_ISR_Disable(_level) ({ \
	unsigned long cr1; \
	asm volatile("gcr	%0, 1" : "=r"(cr1)); \
	_level = cr1 & CR1_IRQ_ENABLED ? 0 : 1; \
	cr1 &= ~CR1_IRQ_ENABLED; \
	asm volatile("scr	1, %0" :: "r"(cr1)); \
})

#define _CPU_ISR_Enable(_level) ({ \
	unsigned long cr1; \
	asm volatile("gcr	%0, 1" : "=r"(cr1)); \
	cr1 &= ~CR1_IRQ_ENABLED; \
	if (!_level) \
		cr1 |= CR1_IRQ_ENABLED; \
	asm volatile("scr	1, %0" :: "r"(cr1)); \
})

#define _CPU_ISR_Flash( _level ) ({ \
	_CPU_ISR_Enable( _level ); \
	_CPU_ISR_Disable( _level ); \
})

static inline void _CPU_ISR_Set_level(uint32_t new_level)
{
	unsigned long cr1;

	asm volatile("gcr	%0, 1" : "=r"(cr1));
	if (!new_level)
		cr1 |= CR1_IRQ_ENABLED;
	else
		cr1 &= ~CR1_IRQ_ENABLED;
	asm volatile("scr	1, %0" :: "r"(cr1));
}

static inline uint32_t _CPU_ISR_Get_level(void)
{
	unsigned long cr1;

	asm volatile("gcr	%0, 1" : "=r"(cr1));

	return cr1 & CR1_IRQ_ENABLED ? 0 : 1;
}

/*
 * Data and instruction cache always enabled.
 */
#define CR1_COMMON ((1 << 5) | (1 << 6))

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp, _tls_area ) \
  /* Locate Me */                                                  \
  do {                                                             \
    uintptr_t   _stack;                                            \
                                                                   \
    (void) _is_fp; /* avoid warning for being unused */            \
    (void) _isr;   /* avoid warning for being unused */            \
    _stack = ((uintptr_t)(_stack_base)) + (_size);		   \
    (_the_context)->psr = !(_isr) ? CR1_IRQ_ENABLED : 0;	   \
    (_the_context)->psr |= CR1_COMMON;				   \
    (_the_context)->far = 0xdeadbeef;				   \
    (_the_context)->spsr = (_the_context)->psr;			   \
    (_the_context)->lr = (uint32_t)(_entry_point);                 \
    (_the_context)->fp = (uint32_t)_stack;                         \
    (_the_context)->sp = (uint32_t)_stack;                         \
  } while (0)


#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) (_base) + (_offset) )

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  }

#define _CPU_Fatal_halt( _source, _error ) \
        printk("Fatal Error %d.%d Halted\n",_source,_error); \
        for(;;)

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    (_output) = 0;   /* do something to prevent warnings */ \
  }

#endif

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

#endif

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

#endif

void _CPU_Initialize(void);
void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);
void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);
void _CPU_Install_interrupt_stack( void );
void *_CPU_Thread_Idle_body( uint32_t );
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);
void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

static inline void _CPU_Context_volatile_clobber( uintptr_t pattern )
{
}

static inline void _CPU_Context_validate( uintptr_t pattern )
{
	while (1)
		continue;
}

typedef struct {
	uint32_t integer_registers [16];
	uint32_t psr;
	uint32_t far;
	uint32_t dfar;
} CPU_Exception_frame;

void _CPU_Exception_frame_print(const CPU_Exception_frame *frame);

static inline uint32_t CPU_swap_u32(uint32_t value)
{
	uint32_t   byte1, byte2, byte3, byte4, swapped;

	byte4 = (value >> 24) & 0xff;
	byte3 = (value >> 16) & 0xff;
	byte2 = (value >> 8)  & 0xff;
	byte1 =  value        & 0xff;

	swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
	return( swapped );
}

#define CPU_swap_u16( value ) \
	(((value&0xff) << 8) | ((value >> 8)&0xff))

typedef uint32_t CPU_Counter_ticks;

CPU_Counter_ticks _CPU_Counter_read(void);

static inline CPU_Counter_ticks _CPU_Counter_difference(CPU_Counter_ticks second,
							CPU_Counter_ticks first)
{
	return second - first;
}

#ifdef __cplusplus
}
#endif
#endif /* ASM */

#endif
