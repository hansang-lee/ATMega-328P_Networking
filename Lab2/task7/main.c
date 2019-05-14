#include "init.h"
#include "lab2_task7.h"

/********************************************************************************
 * "Scenario"
 * -
 * -
 * 
 * "Polynomial for CRC32"
 * 1 0000 0100 1100 0001 0001 1101 1011 0111
 * 
 * "Data Frame Format"
 * 1 Bytes      - Preamble  "01111110"
 * 33 Bits      - CRC32     "x xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx"
 * 6 Bytes 		- Payload   "xxxxxxxx ..."
 ********************************************************************************/
int main()
{
	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

	for (;;)
	{
		_delay_ms(10000);
		uart_transmit('\r');
		uart_transmit('\n');
	}
}