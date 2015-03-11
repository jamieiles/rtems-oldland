/*
 *  COPYRIGHT (c) 2015.
 *  Jamie Iles
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */
#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

#define UART_STATUS_TX_EMPTY	(1 << 0)
#define UART_STATUS_RX_READY	(1 << 1)
#define UART_DATA_OFFS		0x0
#define UART_STATUS_OFFS	0x4
#define UART_BASE		0x80000000

static inline unsigned long readl(unsigned long addr)
{
	volatile unsigned long *p = (volatile unsigned long *)addr;

	return *p;
}

static inline void writel(unsigned long v, unsigned long addr)
{
	volatile unsigned long *p = (volatile unsigned long *)addr;

	*p = v;
}

static void oldland_serial_putc(char c)
{
	if (c == '\n')
		oldland_serial_putc('\r');
	while (!(readl(UART_BASE + UART_STATUS_OFFS) &
		 UART_STATUS_TX_EMPTY))
		continue;
	writel((unsigned char)c, UART_BASE + UART_DATA_OFFS);
}

static int oldland_serial_tstc(void)
{
	return (readl(UART_BASE + UART_STATUS_OFFS) &
		UART_STATUS_RX_READY);
}

static int oldland_serial_getc(void)
{
	while (!(readl(UART_BASE + UART_STATUS_OFFS) &
		 UART_STATUS_RX_READY))
		continue;
	return readl(UART_BASE + UART_DATA_OFFS) & 0xff;
}

void console_initialize_hardware(void)
{
}

void console_outbyte_polled(int port, char ch)
{
	oldland_serial_putc(ch);
}

int console_inbyte_nonblocking(int port)
{
	return oldland_serial_tstc() ? oldland_serial_getc() : -1;
}

#include <rtems/bspIo.h>

static void oldland_output_char(char c)
{
	console_outbyte_polled(0, c);
}

static int oldland_get_char(void)
{
	return console_inbyte_nonblocking(0);
}

BSP_output_char_function_type           BSP_output_char = oldland_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = oldland_get_char;
