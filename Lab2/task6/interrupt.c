#include "interrupt.h"
#include "crc.h"

/* Interrupt A - Data Transmitter */
volatile uint16_t transmitFlag = FLAG_SENDING_PREAMBLE;
ISR(TIMER0_COMPA_vect)
{
    static volatile uint16_t timerA = (_PERIOD_ / 2);
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        // STEP1. SENDING PREAMBLE
        if(transmitFlag == FLAG_SENDING_PREAMBLE)
        {
	        static volatile uint16_t transmitCounter = 0;
            static volatile const uint8_t transmitPreambleBuffer = 0b01111110;

            if(read_bit(transmitPreambleBuffer, transmitCounter++))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            if(transmitCounter > 8)
            {
                transmitCounter = 0;
                transmitFlag = FLAG_GENERATING_CRC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP2. GENERATING CRC
        if(transmitFlag == FLAG_GENERATING_CRC)
        {
            transmitFlag = FLAG_SENDING_CRC;
        }

        ////////////////////////////////////////////////////////
        // STEP3. SENDING CRC
        if(transmitFlag == FLAG_SENDING_CRC)
        {
            transmitFlag = FLAG_SENDING_SIZE;
        }

        ////////////////////////////////////////////////////////
        // STEP4. SENDING SIZE
        if(transmitFlag == FLAG_SENDING_SIZE)
        {
            transmitFlag = FLAG_SENDING_MSG;
        }

        ////////////////////////////////////////////////////////
        // STEP5. SENDING MESSAGE
        if(transmitFlag == FLAG_SENDING_SIZE)
        {
            transmitFlag = FLAG_SENDING_PREAMBLE;
        }
        ////////////////////////////////////////////////////////
	}
}

/* Pin Change Interrupt */
volatile uint16_t receiveFlag = FLAG_DETECTING_PREAMBLE;
ISR(PCINT2_vect)
{
    ////////////////////////////////////////////////////////
    // STEP1. DETECTING PREAMBLE
    // Constantly receives messages for checking preamble
    static uint8_t receivePreambleBuffer[1] = {0};
    const uint8_t preambleReadyMsg[17] = "Preamble Detected";
    if(receiveFlag == FLAG_DETECTING_PREAMBLE)
    {
        update_preamble_buffer(receivePreambleBuffer);
        print_preamble_buffer(*receivePreambleBuffer);
        uart_transmit('\r');

        if(check_preamble(*receivePreambleBuffer) == TRUE)
        {
            uart_changeLine();
            print_msg(preambleReadyMsg, 17);
            uart_changeLine();
            uart_changeLine();
            receivePreambleBuffer[0] = 0x00;
            receiveFlag = FLAG_RECEIVING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP2. RECEIVING CRC
    // When detected PREAMBLE
    static uint32_t receiveCrcBuffer[1] = {0};
    static uint16_t receiveCounter = 0;
    const uint8_t crcReceivedMsg[12] = "CRC Received";
    if(receiveFlag == FLAG_RECEIVING_CRC)
    {
        // Receiving CRC data 
        if(receiveCounter < 33)
        {
            update_crc32_buffer(receiveCrcBuffer);
            print_crc32_buffer(*receiveCrcBuffer);
            uart_transmit('\r');
            receiveCounter++;
        }

        // Finished Receiving CRC data        
        else
        {
            uart_changeLine();
            print_msg(crcReceivedMsg, 12);
            uart_changeLine();
            uart_changeLine();
            receiveCrcBuffer[0] = 0;
            receiveCounter = 0;
            receiveFlag = FLAG_RECEIVING_SIZE;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP3. RECEIVING SIZE
    if(receiveFlag == FLAG_RECEIVING_SIZE)
    {

        receiveFlag = FLAG_RECEIVING_MSG;
    }

    ////////////////////////////////////////////////////////
    // STEP4. RECEIVING MESSAGE
    if(receiveFlag == FLAG_RECEIVING_MSG)
    {
        receiveFlag = FLAG_CHECKING_CRC;
    }

    ////////////////////////////////////////////////////////
    // STEP5. CHECKING CRC
    if(receiveFlag == FLAG_CHECKING_CRC)
    {
        receiveFlag = FLAG_DETECTING_PREAMBLE;
    }
    ////////////////////////////////////////////////////////
}

/* Interrupt B - Clock Signal */
ISR(TIMER0_COMPB_vect)
{
	static volatile uint16_t timerB = 0;
	timerB++;
	if (timerB > _PERIOD_)
	{
		_LED_B_TOGGLE_;
		timerB = 0;
		
        ////////////////////////////////////////////////////////
		/* CLOCK SIGNAL TRANSMIT */
		_PIN_CHANGE_;
        ////////////////////////////////////////////////////////
	}
}
