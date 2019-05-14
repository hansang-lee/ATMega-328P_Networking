#include "io.h"

void io_init()
{
	/* LED */
	DDRB |= (1 << DDB5);
	DDRB |= (1 << DDB4);

	/* Pin Change - Sending Clock Signal */
	DDRB |= (1 << DDB1); // Output
	DDRD &= ~(1 << DDD3); // Input

	/* Sending Data */
	DDRB |= (1 << DDB2); // Output(Data Send)
	DDRD &= ~(1 << DDD4); // Input(Data Receive)
}
