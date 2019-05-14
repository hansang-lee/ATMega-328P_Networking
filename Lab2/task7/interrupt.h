#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define _PERIOD_            1000
#define _LED_A_TOGGLE_      (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_      (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_        (PORTB ^= (1 << PB1))
#define _DATA_SEND_         (PORTB ^= (1 << PB2))
#define _RECEIVED_DATA_     (PIND & (1 << PD4))