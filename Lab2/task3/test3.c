#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define _PERIOD_	1000

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupt_setup();

/* Interrupt */
ISR(TIMER0_OVF_vect)
{
	static volatile unsigned int sharedTimer = 0;
	TCNT0 = 209;    // TCNT0 Register Initialization
					// 12 Mhz: 12000000 Hz / 256 = 46875 Hz
					// Frequency: 46875 Hz
					// Period: 1 / 46875 = 0.00002133s = 21.33us            
					// Event executed at every "1ms": 21.33us / 46.88...(47)
					// TCNT0 init no.: 256 - 47 = 209

	sharedTimer++;
	if (sharedTimer > _PERIOD_)
	{
		/* Transmission of character 'I' */
		uart_transmit('I');
		sharedTimer = 0;
	}
}

/********************************************************************************
 * Use "Timer Interrupts" to output a character pattern on the serial connection
 * Timer Interrupt Handler prints the character 'I'
 * Main Program prints the character 'M'
 ********************************************************************************/
int main()
{
	/* Setup */
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	sei();

	for (;;)
	{
		/* Transmission of character 'M' */
		uart_transmit('M');
		_delay_ms(_PERIOD_);
	}
}

void interrupt_setup()
{
	/* Prescaler 256 */
	TCCR0B |= (1 << CS02);

	/* ISR OVF vect Mode */
	TIMSK0 |= (1 << TOIE0);
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
