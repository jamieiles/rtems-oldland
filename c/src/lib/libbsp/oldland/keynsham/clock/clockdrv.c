#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/score/cpu.h>
#include <rtems/irq-extension.h>

#include "timer_defines.h"

/*
 * 1ms tick.
 */
#define TIMER0_RELOAD     ((50000000 / 1000000) * rtems_configuration_get_microseconds_per_tick())

/* CPU counter */
static CPU_Counter_ticks cpu_counter_ticks;

static inline uint32_t timer_read(unsigned int offset)
{
	return *(volatile uint32_t *)(TIMER_ADDRESS + offset);
}

static inline void timer_write(uint32_t v, unsigned int offset)
{
	*(volatile uint32_t *)(TIMER_ADDRESS + offset) = v;
}

static void keynsham_clock_isr(void)
{
	timer_write(0xffffffff, TIMER_EOI_REG_OFFS);

	cpu_counter_ticks += TIMER0_RELOAD;
}

static void keynsham_clock_handler_install(proc_ptr new_isr, proc_ptr old_isr)
{
	old_isr = NULL;
	if (rtems_interrupt_handler_install(KEYNSHAM_TIMER0_IRQ, "timer0",
					    0, new_isr, NULL) != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred(0x0defaced);
}

static void keynsham_clock_initialize(void)
{
	cpu_counter_ticks = 0;
	timer_write(TIMER0_RELOAD, TIMER_RELOAD_REG_OFFS);
	timer_write(TIMER_ENABLED_MASK | TIMER_PERIODIC_MASK |
		    TIMER_IRQ_ENABLE_MASK,
		    TIMER_CONTROL_REG_OFFS);
}

static void keynsham_clock_cleanup(void)
{
	timer_write(0, TIMER_CONTROL_REG_OFFS);
}

#define KEYNSHAM_NS_PER_CYCLE (1000000000 / 50000000)

static uint32_t keynsham_clock_nanoseconds_since_last_tick(void)
{
	return timer_read(TIMER_COUNT_REG_OFFS) * KEYNSHAM_NS_PER_CYCLE;
}

CPU_Counter_ticks _CPU_Counter_read(void)
{
	return cpu_counter_ticks + timer_read(TIMER_COUNT_REG_OFFS);
}

#define Clock_driver_support_at_tick() keynsham_clock_isr()

#define Clock_driver_support_initialize_hardware() keynsham_clock_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
	do {                                                 \
		old_isr = NULL;                                    \
		keynsham_clock_handler_install(isr, old_isr);       \
	} while (0)

#define Clock_driver_support_shutdown_hardware() keynsham_clock_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
	keynsham_clock_nanoseconds_since_last_tick

#include "../../../shared/clockdrv_shell.h"
