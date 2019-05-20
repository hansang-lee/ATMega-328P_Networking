#include "uart.h"
#include <avr/interrupt.h>

/* Turns on LEDs : PB4 and PB5 */
#define LED_A_TOGGLE()              (PORTB ^= (1 << PB5))
#define LED_B_TOGGLE()              (PORTB ^= (1 << PB4))

/* Send Data Signal : PB2 > PD4 */
#define SEND_DATA_ONE()             (PORTB |= (1 << PB2))
#define SEND_DATA_ZERO()            (PORTB &= ~(1 << PB2))

/* Send Clock Signal for Pin-Change : PB1 > PD3 */
#define PIN_CHANGE()                (PORTB ^= (1 << PB1))
#define RECEIVED_DATA()             (PIND & (1 << PD4))

/* How often run Interrups? : 1000 = 1s */
#define INTERRUPT_PERIOD            100

/* Flags at Transmitter Part */
#define FLAG_GENERATING_CRC         0
#define FLAG_SENDING_PREAMBLE       1
#define FLAG_SENDING_CRC            2
#define FLAG_SENDING_DLC            3
#define FLAG_SENDING_PAYLOAD        4

/* Flags at Receiver Part */
#define FLAG_DETECTING_PREAMBLE     0
#define FLAG_RECEIVING_CRC          1
#define FLAG_RECEIVING_DLC          2
#define FLAG_RECEIVING_PAYLOAD      3
#define FLAG_CHECKING_CRC           4

#define SIZE_OF_PREAMBLE            8
#define SIZE_OF_CRC                 32
#define SIZE_OF_DLC                 8
#define SIZE_OF_POLYNOMIAL          33
#define SIZE_OF_PAYLOAD             32

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
	PCMSK2 |= (1 << PCINT19);   // PCMSK[0;2] : Pin Change Mast Register
                                // PCINT19 bit : PD3
	PCICR |= (1 << PCIE2);      // PCICR : Pin Change Interrupt Control Register
                                // PCIE2 bit : Enabling PCINT[16;23] cause an interrupt
}
