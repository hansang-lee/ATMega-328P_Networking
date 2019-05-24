#include "io.c"
#include "interrupt.c"

int main()
{
    rFrame = &_rFrame;
    tFrame = &_tFrame;

	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

	for (;;)
	{

	}
}
