#include "io.c"
#include "interrupt.c"

/********************************************************************************
 * TASK 2-4 : LED TOGGLING
 * 			: INTERRUPT COMPA, PIN CHANGE INTERRUPT 
 * 
 * "PB1" transmits clock signla
 * Indicate the receiving of a clock signal on "PD3" by using the Gertboard LEDs
 *
 * <Note>
 * Use the ATMel documentation to learn about the possibilities of a pin-change interrupt
 * handler.
 ********************************************************************************/
int main()
{
	io_init();
	cli();
	interrupts_setup();
	pin_change_setup();
	sei();

	for (;;)
	{

	}
}