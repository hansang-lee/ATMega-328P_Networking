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
void interrupts_setup();
void pin_change_setup();

/* Messages */
//const char* pinChangeMsg = "PIN-CHANGE";
//const char* interruptMsg = "INTERRUPT";
//const char* mainMsg = "MAIN";

int sharedTimer=0;
ISR(TIMER0_OVF_vect)
{
    TCNT0 = 209;    // TCNT0 Register Initialization
                    // 12 Mhz: 12000000 Hz / 256 = 46875 Hz
                    // Frequency: 46875 Hz
                    // Period: 1 / 46875 = 0.00002133s = 21.33us            
                    // Event executed at every "1ms": 21.33us / 46.88...(47)
                    // TCNT0 init no.: 256 - 47 = 209
                    //
    sharedTimer++;
    if(sharedTimer > 100)
    {
        PORTB ^= (1 << PB1);
        sharedTimer = 0;
    }
}

ISR(PCINT2_vect)
{
    PORTB ^= (1 << PB5);
}

int main()
{
    /* Set pin outputs */
    // "UDRn" USART I/O Data Register- reads Data at I/O Location
    DDRB |= (1 << PB5); // LED pin
    DDRB |= (1 << PB1); // Signal pin
    
    /* Initialization */
    //uart_init(MYUBRR);
    interrupts_setup();
    pin_change_setup();
    sei();

    for(;;)
    {
        
    }
}

void pin_change_setup()
{
    /* Pin change Interrupts */
    /* "PCMSK[2;0]" Pin Change Mask Register */
    /* Each "PCINT[23;0]" bit selects which pin change interrupt is enabled
     * on the corresponding I/O pins */
    /* "PCINT17" corresponds to "PD1" pin
     * "PCINT18" corresponds to "PD2" pin */
    PCMSK2 |= (1 << PCINT17);
    //PCMSK2 |= (1 << PCINT18);
    
    /* if "PCINT[23;0]" is set, and the "PCIE[2;0]" bit in "PCICR" is set, 
     * pin change enabled */
    /* "PCICR" Pin Change Interrupt Control Register */
    /* "PCIE2" bit is set, any change on any enabled "PCINT[23;16]" will cause an interrupt*/
    PCICR |= (1 << PCIE2);

    /* "SREG" AVR Status Register */
    /* The global interrupt flag is maintained in the I bit of the status register "SREG" */
    //sei();
}

void interrupts_setup()
{
    /* Prescaler */
    /* "TCCR0B" Timer/Counter Control Register */
    /* "CS02" Set prescaler to 256 */ 
    TCCR0B |= (1 << CS02);
    
    /* "TOIE0, TOIE2" Set "ISR OVF vect"(Overflow) */
    TIMSK0 |= (1 << TOIE0);
    
    /* "ISR" Global Interrupts On */
    //sei();
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
