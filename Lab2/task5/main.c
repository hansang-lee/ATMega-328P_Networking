#include "io.h"
#include "interrupt.c"

/********************************************************************************
 * <Q> Try to modify the clock frequency of the communication. What happens?
 *
 * <Q> What happens in your implementation when there is no data to send?
 *
 * <Note>
 * In preparation for later tasks, it is reasonable to introduce separate send and receive buffers.
 * For this task, they only need to hold one bit, but this will change later.
 * You must consider that both buffers are read and written by multiple activities. Prepare your
 * code so that only one activity (interrupt handler / main code) can modify the buffer at a
 * time.
 * It will be helpful to have a static send buffer with pre-filled data for testing.
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
