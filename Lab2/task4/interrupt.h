#include <avr/interrupt.h>

#define _PERIOD_		1000
#define _PIN_CHANGE_	(PORTB ^= (1 << PB1))
#define _LED_TOGGLE_	(PORTB ^= (1 << PB5))

/* Interrupt */
void interrupts_setup();
void pin_change_setup();