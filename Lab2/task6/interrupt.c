#include "interrupt.h"
#include "crc.h"

/* Messages */
volatile unsigned char preambleMsg[11] = "Preamble OK";

/* Buffers */
volatile const unsigned char preamble = 0b01111110;
unsigned char preamble_buffer[1] = {0};
volatile unsigned char uart_buffer = {0};

/* Interrupt A - Data Transmitter */
ISR(TIMER0_COMPA_vect)
{
	static volatile unsigned int timerA = (_PERIOD_ / 2);
	static volatile unsigned int i = 0;
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		timerA = 0;
        
        if(read_bit(preamble, i++))
            _SEND_LOGICAL_1_;
        else
            _SEND_LOGICAL_0_;
        if(i == 8) i=0;
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
		timerB = 0;
		
		/* TO DO */
		_PIN_CHANGE_;
	}
}

/* Pin Change - Data Carrier */
ISR(PCINT2_vect)
{
    /* Updates the Global Queue */
    update_preamble_buffer(preamble_buffer);
    print_buffer(*preamble_buffer);
    
    /* TO DO */    
    //if(_RECEIVED_DATA_)
    //    uart_buffer = '1';
    //else
    //    uart_buffer = '0';
    //uart_transmit(uart_buffer);

    //update_preamble_buffer(preamble_buffer, _RECEIVED_DATA_);
    if(check_preamble(*preamble_buffer) == NO_ERROR)
    {
        uart_transmit('\r');
        uart_transmit('\n');
        for(int i=0; i<11; i++)
            uart_transmit(preambleMsg[i]);
    }
}
