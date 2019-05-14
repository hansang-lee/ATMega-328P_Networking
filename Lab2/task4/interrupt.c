#include "interrupt.h"

#define _PERIOD_		1000
#define _PIN_CHANGE_	(PORTB ^= (1 << PB1))
#define _LED_TOGGLE_	(PORTB ^= (1 << PB5))

/* Interrupt A - Transmit Clock Signal */
ISR(TIMER0_COMPA_vect)
{
	static volatile unsigned int sharedTimer = 0;
	sharedTimer++;
	if (sharedTimer > _PERIOD_)
	{
		_PIN_CHANGE_;
		sharedTimer = 0;
	}
}

/* Pin Change - Toggling LED */
ISR(PCINT2_vect)
{
	_LED_TOGGLE_;
}

void interrupts_setup()
{
	TIMSK0 |= (1 << OCIE0A);    // Interrupt TimerCounter0 Compare Match A
	TCCR0A |= (1 << WGM01);     // CTC Mode
	TCCR0B |= (1 << CS02);      // Clock/256 = 46875
	OCR0A = 0x2f;               // 1/46875*47 = 0.001 seconds per tick
}

void pin_change_setup()
{
	/* "PCMSK[2;0]" Pin Change Mask Register */
	PCMSK2 |= (1 << PCINT19); // PD3

	/* "PCICR" Pin Change Interrupt Control Register
	 * "PCIE2" bit is set, any change on any enabled "PCINT[23;16]" will cause an interrupt */
	PCICR |= (1 << PCIE2);
}