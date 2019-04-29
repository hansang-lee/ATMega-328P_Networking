#include <avr/io.h>
#define F_CPU 12000000UL
#include <util/delay.h>
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/setbaud.h>

#define NUM_PINS 12
#define SLOW 50
#define FAST 20

void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);
void showSlowAnimation(int[]);
void showFastAnimation(int[]);

int main()
{
    /* Variables */
    int pins[NUM_PINS] = {PB5, PB4, PB3, PB2, PB1, PB0, PD7, PD6, PD5, PD4, PD3, PD2};
	unsigned char input;

    /* Initialization */
    uart_init(MYUBRR);

    for(;;)
    {
        /* Receive Input */
        input = uart_receive();
        uart_transmit(input);

        /* Slow Mode */
        if(input == 'S')
        {
            showSlowAnimation(pins);
        }

        /* Fast Mode */
        else if(input == 'F')
        {
            showFastAnimation(pins);  
        }
    }
}

void showSlowAnimation(int pins[])
{
    int i = 0;
    int order = 0;
    while(order < 2)
    {
        while((-1<i)&&(i<6))
        {
            if(order == 0) {PORTB |= (1 << pins[i]);}
            else if(order == 1) {PORTB &= ~(1 << pins[i]);}
            _delay_ms(SLOW);
            i++;
	    }
        while((5<i)&&(i<12))
        {
            if(order == 0) {PORTD |= (1 << pins[i]);}
            else if(order == 1) {PORTD &= ~(1 << pins[i]);}
            _delay_ms(SLOW);
            i++;
        }
        i=0;
        order++;
    }
}

void showFastAnimation(int pins[])
{
    int i = 0;
    int order = 0;
    while(order < 2)
    {
        while((-1<i)&&(i<6))
        {
            if(order == 0) {PORTB |= (1 << pins[i]);}
            else if(order == 1) {PORTB &= ~(1 << pins[i]);}
            _delay_ms(FAST);
            i++;
	    }
        while((5<i)&&(i<12))
        {
            if(order == 0) {PORTD |= (1 << pins[i]);}
            else if(order == 1) {PORTD &= ~(1 << pins[i]);}
            _delay_ms(FAST);
            i++;
        }
        i=0;
        order++;
    }
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
    
    /* Set output */
    DDRB = 0xff;
    DDRD = 0xff;
}
