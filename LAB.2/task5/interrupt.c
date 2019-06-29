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