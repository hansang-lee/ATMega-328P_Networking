#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>				// #define 	F_CPU   1000000UL
#include <util/setbaud.h>			// #define 	BAUD_TOL   2
									// #define 	UBRR_VALUE
									// #define 	UBRRL_VALUE
									// #define 	UBRRH_VALUE
									// #define 	USE_2X   0
#include <avr/interrupt.h>

#define _PERIOD_        1000
#define _LED_A_TOGGLE_  (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_  (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_    (PORTB ^= (1 << PB1))
#define _DATA_SEND_     (PORTB ^= (1 << PB2))
#define _RECEIVED_DATA_  (PIND & (1 << PD4))

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupt_setup();
void pin_change_setup();

/* Initialization */
void io_init();

/* Buffers */
volatile unsigned char receiveBuffer = '\0';

/* Interrupt A - Data Transmitter */
ISR(TIMER0_COMPA_vect)
{
	static volatile unsigned int timerA = (_PERIOD_ / 2);
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		_DATA_SEND_;
		timerA = 0;
	}
}

/* Interrupt B - Clock Signal */
ISR(TIMER0_COMPB_vect)
{
	static volatile unsigned int timerB = 0;
	timerB++;
	if (timerB > _PERIOD_)
	{
		_LED_B_TOGGLE_;
		_PIN_CHANGE_;
		timerB = 0;
	}
}

/* Pin Change - Data Carrier */
ISR(PCINT2_vect)
{
	if (_RECEIVED_DATA_)
		receiveBuffer = '1';
	else
		receiveBuffer = '0';
	uart_transmit(receiveBuffer);
}

/********************************************************************************
 * <Q> Try to modify the clock frequency of the communication. What happens?
 *
 * <Q> What happens in your implementation when there is no data to send?
 *
 * <Note>
 * In preparation for later tasks, it is reasonable to introduce separate send and receive buffers.
 * For this task, they only need to hold one bit, but this will change later.
 * You must consider that both buffers are read and written by multiple activities. Prepare your
 * code so that only one activity (interrupt handler / main code) can modify the buffer at a
 * time.
 * It will be helpful to have a static send buffer with pre-filled data for testing.
 ********************************************************************************/
int main()
{
	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

	for (;;)
	{
		_delay_ms(10000);
		uart_transmit('\r');
		uart_transmit('\n');
	}
}

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
