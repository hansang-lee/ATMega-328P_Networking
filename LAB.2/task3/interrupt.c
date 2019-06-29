#include "interrupt.h"
#include "uart.h"

void interrupt_setup()
{
	/* Prescaler 256 */
	TCCR0B |= (1 << CS02);

	/* ISR OVF vect Mode */
	TIMSK0 |= (1 << TOIE0);
}

ISR(TIMER0_OVF_vect)
{
	/* Timer */
	static volatile unsigned int sharedTimer = 0;

	/* TCNT0 Register Initialization
	 * 12 Mhz: 12000000 Hz / 256 = 46875 Hz
	 * Frequency: 46875 Hz
	 * Period: 1 / 46875 = 0.00002133s = 21.33us   
	 * Event executed at every "1ms": 21.33us / 46.88...(47)
	 * TCNT0 init no.: 256 - 47 = 209 */
	TCNT0 = 209;

	sharedTimer++;
	if (sharedTimer > _PERIOD_)
	{
		/* Transmission of character 'I' */
		uart_transmit('I');
		sharedTimer = 0;
	}
}

void interrupt_setup()
{
	/* Prescaler 256 */
	TCCR0B |= (1 << CS02);

	/* ISR OVF vect Mode */
	TIMSK0 |= (1 << TOIE0);
}