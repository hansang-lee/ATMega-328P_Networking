#include "interrupt.h"
#include "crc.h"

/* Interrupt A - Data Transmitter */
volatile uint16_t transmitFlag = FLAG_SENDING_PREAMBLE;
volatile uint16_t timerA = (_PERIOD_ / 2);
volatile uint16_t transmitCounter = 0;

ISR(TIMER0_COMPA_vect)
{
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        // STEP1. SENDING PREAMBLE
        static const uint8_t transmitPreambleBuffer[(SIZE_OF_PREAMBLE/8)] = { 0b01111110 };
        if(transmitFlag == FLAG_SENDING_PREAMBLE)
        {
            if(read_bit(transmitPreambleBuffer, SIZE_OF_PREAMBLE, transmitCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            transmitCounter++;
            if(transmitCounter > SIZE_OF_PREAMBLE)
            {
                transmitCounter = 0;
                transmitFlag = FLAG_GENERATING_CRC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP2. GENERATING CRC
        static const uint8_t transmitCrcBuffer[(SIZE_OF_CRC/8)] = { 0b10101010, 
                                                                    0b10101010, 
                                                                    0b10101010, 
                                                                    0b10101010 };
        if(transmitFlag == FLAG_GENERATING_CRC)
        {
            transmitFlag = FLAG_SENDING_CRC;
        }

        ////////////////////////////////////////////////////////
        // STEP3. SENDING CRC
        if(transmitFlag == FLAG_SENDING_CRC)
        {
            if(read_bit(transmitCrcBuffer, SIZE_OF_CRC, transmitCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            transmitCounter++;
            if(transmitCounter > SIZE_OF_CRC)
            {
                transmitCounter = 0;
                transmitFlag = FLAG_SENDING_SIZE;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP4. SENDING SIZE
        static const uint8_t transmitSizeOfPayLoad[SIZE_OF_DLC] = { 0b11000011 };
        if(transmitFlag == FLAG_SENDING_SIZE)
        {
            if(read_bit(transmitSizeOfPayLoad, SIZE_OF_DLC, transmitCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            transmitCounter++;
            if(transmitCounter > SIZE_OF_DLC)
            {
                transmitCounter = 0;
                transmitFlag = FLAG_SENDING_MSG;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP5. SENDING MESSAGE
        static const uint8_t transmitPayload[(SIZE_OF_PAYLOAD/8)] = { 0b10101010,
                                                                      0b10101010,
                                                                      0b10101010,
                                                                      0b10101010};
        if(transmitFlag == FLAG_SENDING_MSG)
        {
            if(read_bit(transmitPayload, SIZE_OF_PAYLOAD, transmitCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            transmitCounter++;
            if(transmitCounter > SIZE_OF_PAYLOAD)
            {
                transmitCounter = 0;
                transmitFlag = FLAG_SENDING_PREAMBLE;
            }
        }
        ////////////////////////////////////////////////////////
	}
}

/* Pin Change Interrupt */
volatile uint16_t receiveFlag = FLAG_DETECTING_PREAMBLE;
static uint16_t receiveCounter = 0;
ISR(PCINT2_vect)
{
    ////////////////////////////////////////////////////////
    // STEP1. DETECTING PREAMBLE
    // Constantly receives messages for checking preamble
    static uint8_t receivePreambleBuffer[1] = {0x00};
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
            receiveCounter = 0;
            receiveFlag = FLAG_RECEIVING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP2. RECEIVING CRC
    // When detected PREAMBLE
    static uint32_t receiveCrcBuffer[1] = {0x00000000};
    const uint8_t crcReceivedMsg[12] = "CRC Received";
    if(receiveFlag == FLAG_RECEIVING_CRC)
    {
        // Receiving CRC
        update_crc32_buffer(receiveCrcBuffer);
        print_crc32_buffer(*receiveCrcBuffer);
        uart_transmit('\r');

        // Finished Receiving CRC
        receiveCounter++;
        if(receiveCounter > SIZE_OF_CRC)
        {
            uart_changeLine();
            print_msg(crcReceivedMsg, 13);
            uart_changeLine();
            uart_changeLine();
            receiveCrcBuffer[0] = 0x00000000;
            receiveCounter = 0;
            receiveFlag = FLAG_RECEIVING_SIZE;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP3. RECEIVING SIZE
    static uint8_t receiveDlcBuffer[1] = {0x00};
    const uint8_t dlcReceivedMsg[12] = "DLC Received";
    static uint32_t sizeOfPayload = 0;
    if(receiveFlag == FLAG_RECEIVING_SIZE)
    {
        // Receiving DLC
        update_dlc_buffer(receiveDlcBuffer);
        print_dlc_buffer(*receiveDlcBuffer);
        uart_transmit('\r');

        // Finished Receiving DLC
        receiveCounter++;
        if(receiveCounter > SIZE_OF_DLC)
        {
            uart_changeLine();
            print_msg(dlcReceivedMsg, 12);
            uart_changeLine();
            uart_changeLine();
            sizeOfPayload &= receiveDlcBuffer[0];
            receiveDlcBuffer[0] = 0x00;
            receiveCounter = 0;
            receiveFlag = FLAG_RECEIVING_MSG;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP4. RECEIVING MESSAGE
    static uint8_t receivePayloadBuffer[4] = {0};
    const uint8_t payloadReceivedMsg[16] = "Payload Received";
    if(receiveFlag == FLAG_RECEIVING_MSG)
    {
        // Receiving PAYLOAD
        update_payload_buffer(receivePayloadBuffer, 32, receiveCounter);
        print_payload_buffer(tt, 32);
        uart_transmit('\r');

        // Finished Receiving PAYLOAD
        receiveCounter++;
        if(receiveCounter > 32)
        {
            uart_changeLine();
            print_msg(payloadReceivedMsg, 16);
            uart_changeLine();
            uart_changeLine();
            
            for(int i=0; i<4; i++)
                receivePayloadBuffer[i] = 0x00;
            
            receiveCounter = 0;
            receiveFlag = FLAG_CHECKING_CRC;
        }
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
