#include <stdio.h>
#include <avr/io.h>
#define F_CPU 12000000UL // Oscilator Clock Rate
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define _PERIOD_        1000
#define _LED_A_TOGGLE_  (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_  (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_    (PORTB ^= (1 << PB1))

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupt_setup();
void pin_change_setup();

/* Buffers */
volatile unsigned char tmpBuffer[5] = "Hello";
volatile unsigned char sendBuffer = '0';
volatile unsigned char receiveBuffer= '0';

/* Interrupt A - Data Transmitter (Layer 2) */
ISR(TIMER0_COMPA_vect)
{
    static volatile unsigned int timerA = (_PERIOD_/2);
    timerA++;
    if(timerA > _PERIOD_)
    {
        /* To Do */
        sendBuffer = 'A';
        _PIN_CHANGE_;

        timerA = 0;
    }
}

/* Interrupt B - Data Receiver (Layer 2) */
ISR(TIMER0_COMPB_vect)
{
    static volatile unsigned int timerB = 0;
    timerB++;
    if(timerB > _PERIOD_)
    {
        /* To Do */
        receiveBuffer = sendBuffer;
        sendBuffer = '0';


        timerB = 0;
    }
}

/* Pin Change - Data Carrier */
ISR(PCINT2_vect)
{
    
}

/* *********************************************************************************
 * Main */
int main()
{
    /* Set pin outputs */
    DDRB |= (1 << PB5);
    DDRB |= (1 << PB4);
    DDRB |= (1 << PB1);
    DDRD |= (1 << PD1);

    /* Initialization */
    cli();
    uart_init(MYUBRR);
    interrupt_setup();
    pin_change_setup();
    sei();

    for(;;)
    {
        for(int i=0; i<10; i++)
        {
            uart_transmit('A');
            _delay_ms(100);
        }
            uart_transmit('\r');
            uart_transmit('\n');
    }
}
/* *********************************************************************************/

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
    /* "PCMSK[2;0]" Pin Change Mask Register
     * "PCINT17 bit" corresponds to "PD1 pin"
     * "PCINT18 bit" corresponds to "PD2 pin" */
    PCMSK2 |= (1 << PCINT17);
    //PCMSK2 |= (1 << PCINT18);
    
    /* if "PCINT[23;0]" is set, and the "PCIE[2;0]" bit in "PCICR" is set, 
     * pin change enabled */
    /* "PCICR" Pin Change Interrupt Control Register */
    /* "PCIE2" bit is set, any change on any enabled "PCINT[23;16]" will cause an interrupt*/
    PCICR |= (1 << PCIE2);
}

void uart_transmit(unsigned char data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

unsigned char uart_receive()
{
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}


void uart_init(unsigned long ubrr)
{
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    UCSR0B |= (1<<RXEN0);
    UCSR0B |= (1<<TXEN0);

    UCSR0C |= (1<<USBS0);
    UCSR0C |= (3<<UCSZ00);
}
