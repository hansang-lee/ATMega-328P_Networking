#include "io.c"
#include "interrupt.c"
#include "uart.h"

/********************************************************************************
 * TASK 2-3 : UART COMMUNICATION
 * 			: MAIN, INTERRUPT 
 * 
 * Use "Timer Interrupts" to output a character pattern on the serial connection
 * Timer Interrupt Handler prints the character 'I'
 * Main Program prints the character 'M'
 ********************************************************************************/
int main()
{
	/* Setup of interrupt and uart communication */
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	sei();

	for (;;)
	{
		/* Transmission of character 'M' */
		uart_transmit('M');
		_delay_ms(_PERIOD_);
	}
}