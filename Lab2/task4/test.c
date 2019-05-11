#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define _PERIOD_	1000
#define _PIN_CHANGE_	(PORTB ^= (1 << PB1))
#define _LED_TOGGLE_	(PORTB ^= (1 << PB5))

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupts_setup();
void pin_change_setup();

/* Initialization */
void io_init();

/* Interrupt - Pin Chaning */
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

int main()
{
	io_init();
	cli();
	//uart_init(MYUBRR);
	interrupts_setup();
	pin_change_setup();
	sei();

	for (;;)
	{

	}
}

void io_init()
{
	/* LED */
	DDRB |= (1 << DDB5);

	/* Pin Change - Toggling LED */
	DDRB |= (1 << DDB1); // Output
	DDRD &= ~(1 << DDD3); // Input
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
