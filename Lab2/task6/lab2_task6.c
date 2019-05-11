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

#define _PERIOD_            1000
#define _LED_A_TOGGLE_      (PORTB ^= (1 << PB5))
#define _LED_B_TOGGLE_      (PORTB ^= (1 << PB4))
#define _PIN_CHANGE_        (PORTB ^= (1 << PB1))
#define _DATA_SEND_         (PORTB ^= (1 << PB2))
#define _RECEIVED_DATA_     (PIND & (1 << PD4))
#define _SIZE_OF_PAYLOAD_   4
#define _PREAMBLE_          0b01111110
#define _POLINOMIAL_        0b100000100110000010001110110110111

/* UART serial communication */
void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);

/* Interrupt */
void interrupt_setup();
void pin_change_setup();

void io_init();
void check_preamble(char preamble);
char* generate_crc(char[] payload);
void check_crc(char[] crc);

/* Data Frame Format */
volatile typedef struct data_frame_t
{
    unsigned char preamble;
    unsigned char crc[4];
    unsigned char payload[_SIZE_OF_PAYLOAD_];
}frame_t;

/* Buffers */
volatile char tmp = 0b01111110;
volatile frame_t sendBuffer;
volatile frame_t receiveBuffer;

/* Interrupt A - Data Transmitter */
ISR(TIMER0_COMPA_vect)
{
	static volatile unsigned int timerA = (_PERIOD_ / 2);
	static volatile char i=0b10000000;
    timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;

        if(sendBuffer.payload = '\0';)
        {
            /* Filling Data */
            sendBuffer.preamble = _PREAMBLE_;
            sendBuffer.payload = 0xf0f0f0f0;
            sendBuffer.crc = generate_crc(sendBuffer.payload);

            /* Send Data */
            //
            //
            _Data_SEND_;
        }
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
    // STEP0. Read sendBuffer
    //        Stack the data on receiveData
    //
    // STEP1. if(preamble) prints log
    // STEP2. if(crc) prints log
    // STEP3. prints data
    //
	if (_RECEIVED_DATA_)
		uart_transmit('1');
	else
		uart_transmit('0');
}

/********************************************************************************
 * "Scenario"
 * -
 * -
 * 
 * "Polynomial for CRC32"
 * 1 0000 0100 1100 0001 0001 1101 1011 0111
 * 
 * "Data Frame Format"
 * 1 Bytes      - Preamble  "01111110"
 * 4 Bytes      - CRC32     "xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx"
 * 0-2008 Bytes - Payload   "xxxxxxxx ..."
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

int check_preamble(char preamble)
{   
    char tmp;
    if(_PREAMBLE_ & preamble)
        return 1;
    return 0;
}

char* generate_crc(char[] payload)
{

    // STEP. DIVIDOR = _POLINOMIAL_
    // STEP. DIVIDEE = (DATA << 32)
    //
    // STEP. LOOP INITIAL VALUE = (_POLINOMIAL_ << ((_SIZE_OF_PAYLOAD_ * 8) - 1))
    // STEP. LOOP UPPERBOUND = ((_SIZE_OF_PAYLOAD_ * 8) - 1))
    // STEP. LOOP CONDITION =
    // 
    // UPPERBOUND = ((_SIZE_OF_PAYLOAD_ * 8) - 1);
    // DIVIDOR = (_POLINOMIAL_ << UPPERBOUND);
    // DIVIDEE = (PAYLOAD << 32);
    // for(int i=0; i<UPPERBOUND; i++)
    // {
    //     DIVIDEE ^= DIVIDOR;
    //     DIVIDOR >> 1;
    // }
    // TRANSMIT_DATA = (DATA << 31) -
    //
    // STEP. Generate Remainder
}

void check_crc(char[] crc)
{

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
