#include "interrupt.h"

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
