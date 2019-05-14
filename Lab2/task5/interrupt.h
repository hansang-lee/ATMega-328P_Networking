#include "uart.h"
#include <avr/interrupt.h>

#define _PERIOD_        1000
#define _LED_A_TOGGLE_  (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_  (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_    (PORTB ^= (1 << PB1))
#define _DATA_SEND_     (PORTB ^= (1 << PB2))
#define _RECEIVED_DATA_ (PIND & (1 << PD4))

/* Interrupt */
void interrupt_setup();
void pin_change_setup();