#include "data_frame.h"
//#include "interrupt.h"

volatile frame_t* sendBuffer;
volatile frame_t* receiveBuffer;

ISR(TIMER0_COMPA_vect)
{
	static volatile unsigned int timerA = (_PERIOD_ / 2);
	static volatile char i=0b10000000;
    timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;

        if(sendBuffer.payload == '\0')
        {
            /* Filling Data */
            sendBuffer.preamble = _PREAMBLE_;
            sendBuffer.payload = 0xff00ff00ff00;
            sendBuffer.crc = generate_crc(sendBuffer.payload);

            /* Send Data */
            //
            //
            _Data_SEND_;
        }
		timerA = 0;
	}
}

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

