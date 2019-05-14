#include "init.h"
#include <avr/io.h>

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

void uart_transmit(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

unsigned char uart_receive()
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void uart_init(unsigned long ubrr)
{
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	UCSR0B |= (1 << RXEN0);
	UCSR0B |= (1 << TXEN0);

	UCSR0C |= (1 << USBS0);
	UCSR0C |= (3 << UCSZ00);
}