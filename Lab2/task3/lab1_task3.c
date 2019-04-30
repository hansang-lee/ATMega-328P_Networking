#include <avr/io.h>
#define F_CPU 12000000UL // Oscilator Clock Rate
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void setup();

char sharedTimer=0;
ISR(TIMER0_OVF_vect)
{
    TCNT0 = 209;    // TCNT0 Register Initialization
                    // 12 Mhz: 12000000 Hz / 256 = 46875 Hz
                    // Frequency: 46875 Hz
                    // Period: 1 / 46875 = 0.00002133s = 21.33us            
                    // Event executed at every "1ms": 21.33us / 46.88...(47)
                    // TCNT0 init no.: 256 - 47 = 209

    sharedTimer++;

    if(sharedTimer > 1000)
        uart_transmit(sharedTimer);
}

int main()
{
    /* Initialization */
    uart_init(MYUBRR);
    setup();

    for(;;)
    {
        /* Endless transmission of character 'M' */
        uart_transmit('M');
        _delay_ms(100);
    }
}

void setup()
{
    /* Prescaler: Timer/Counter Control Register */
    /* WGM01 | WGM00
     * 0       0
     * 0       1
     * 1       0
     * 1       1*/
    TCCR0A &= ~(1 << WGM00);
    TCCR0A &= ~(1 << WGM01);
    TCCR0B = 0xD1;
    
    /* Set the ISR OVF vect */
    /* OCIE0, OCIE2 -
     * TOIE0, TOIE2 - Overflow */
    TIMSK0 |= (1 << TOIE0);
    
    /* Turn on global interrupts */
    sei();

    /* Prescaler: Timer/Counter Control Register */
    // Set prescaler to 256 and start the timer
    TCCR0B |= (1 << CS02);
}

void uart_transmit(unsigned char data)
{
    /* Wait for empty transmit buffer */
    // "UCSRnA" USART Control and Status Register- reads 3 error flags(FEn, DORn, UPEn)
    // "UDREn" Flag bit- indicates whether the transmit buffer is ready to receive new data
    //                   1:Buffer is empty, 0:Buffer's written
    while(!(UCSR0A & (1 << UDRE0)));

    // "UDRn" USART I/O Data Register- reads Data at I/O Location
    UDR0 = data;
}

unsigned char uart_receive()
{
    /* Wait for data to be received */
    // "UCSRnA" USART Control and Status Register- reads 3 error flags(FEn, DORn, UPEn)
    // "RXCn" Flag bit- indicates if there are unread data present in the receive buffer
    while(!(UCSR0A & (1 << RXC0)));

    // "UDRn" USART I/O Data Register- reads Data at I/O Location
    return UDR0;
}


void uart_init(unsigned long ubrr)
{
    /* Set Baudrate(in bits per second) */
    // "UBRRn" value- Contents of the UBRRnH and UBRRnL Registers
    // "UBRRnH" Register- contains the 4 most significant bits of USART Baudrate
    // "UBRRnL" Register- contains the 8 least significant bits of USART Baudrate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;

    /* Enable Receiver and Transmitter */
    // in "UCSRnB" Register 
    // "RXENn bit" Receiver setting- 0:Disabled, 1:Enabled
    // "TXENn bit" Transmitter setting- 0:Disabled, 1:Enabled
    UCSR0B |= (1<<RXEN0);
    UCSR0B |= (1<<TXEN0);

    /* Set Frame-format- 8data, 2stop bits */
    // in "UCSRnC" Register
    // "USBSn bit" selects the number of stop bits to be inserted by the Transmitter.
    // "UCSZn2 bit" sets the number of data bits in a frame that Receiver/Transmitter use.
    UCSR0C |= (1<<USBS0);
    UCSR0C |= (3<<UCSZ00);
}
