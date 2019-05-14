#include "uart.h"
#include <avr/interrupt.h>

#define _PERIOD_            1000
#define _LED_A_TOGGLE_      (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_      (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_        (PORTB ^= (1 << PB1))
#define _SEND_LOGICAL_1_    (PORTB |= (1 << PB2))
#define _SEND_LOGICAL_0_    (PORTB &= ~(1 << PB2))
#define _RECEIVED_DATA_     (PIND & (1 << PD4))

/* Interrupt */
void interrupt_setup();
void pin_change_setup();

void interrupt_setup()
{
	TIMSK0 |= (1 << OCIE0A);    // Interrupt TimerCounter0 Compare Match A
	TIMSK0 |= (1 << OCIE0B);    // Interrupt TimerCounter0 Compare Match B
	TCCR0A |= (1 << WGM01);     // CTC Mode
	TCCR0B |= (1 << CS02);      // Clock/256 = 46875
	OCR0A = 0x2f;               // 1/46875*47 = 0.001 seconds per tick
}

void pin_change_setup()
{
	/* "PCMSK[2;0]" Pin Change Mask Register */
	PCMSK2 |= (1 << PCINT19); // PD3

	/* "PCICR" Pin Change Interrupt Control Register */
	/* "PCIE2" bit is set, any change on any enabled "PCINT[23;16]" will cause an interrupt*/
	PCICR |= (1 << PCIE2);
}
