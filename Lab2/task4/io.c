#include "io.h"

void io_init()
{
	/* LED */
	DDRB |= (1 << DDB5);

	/* Pin Change - Toggling LED */
	DDRB |= (1 << DDB1); // Output
	DDRD &= ~(1 << DDD3); // Input
}