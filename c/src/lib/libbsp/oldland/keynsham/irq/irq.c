#include <bsp/irq-generic.h>

void bsp_interrupt_dispatch(void);

static volatile uint32_t *irq_ctrl = (volatile uint32_t *)(0x80002000);

void bsp_interrupt_dispatch(void)
{
	uint32_t status = irq_ctrl[0];
	uint32_t vector;

	for (vector = 0; vector < 32; ++vector)
		if (status & (1 << vector))
			bsp_interrupt_handler_dispatch(vector);
}

void bsp_interrupt_handler_default(rtems_vector_number irqnum)
{
	printk("spurious interrupt %u\n", irqnum);
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number irqnum)
{
	irq_ctrl[1] |= (1 << irqnum);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number irqnum)
{
	irq_ctrl[2] |= (1 << irqnum);

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize()
{
	return RTEMS_SUCCESSFUL;
}
