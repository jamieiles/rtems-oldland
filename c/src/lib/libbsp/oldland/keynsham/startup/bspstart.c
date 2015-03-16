#include <bsp.h>
#include <libcpu/isr_entries.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>

void bsp_start(void)
{
	bsp_interrupt_initialize();
}
