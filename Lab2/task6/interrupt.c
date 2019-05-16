#include "interrupt.h"
#include "crc.h"

/* Interrupt A - Data Transmitter */
volatile uint16_t transmitFlag = FLAG_SENDING_PREAMBLE;
volatile uint16_t timerA = (_PERIOD_/2);
volatile uint16_t transmitCounter = 0;

const uint8_t transmitPreambleBuffer[(SIZE_OF_PREAMBLE/8)] = { 0b01111110 };
const uint8_t transmitCrcBuffer[(SIZE_OF_CRC/8)] = { 0b11110000, 
                                                     0b11110000, 
                                                     0b11110000, 
                                                     0b11110000 };
const uint8_t transmitDlcBuffer[SIZE_OF_DLC] = { 0b11110000 };
const uint8_t transmitPayload[(SIZE_OF_PAYLOAD/8)] = { 0b00001111,
                                                       0b00001111,
                                                       0b00001111,
                                                       0b00001111 };
ISR(TIMER0_COMPA_vect)
{
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        // STEP1. SENDING PREAMBLE
        if(transmitFlag == FLAG_SENDING_PREAMBLE)
        {
            if(transmitCounter < SIZE_OF_PREAMBLE)
            {
                if(read_bit(transmitPreambleBuffer, SIZE_OF_PREAMBLE, transmitCounter))
                    _SEND_LOGICAL_1_;
                else
                    _SEND_LOGICAL_0_;
                
                transmitCounter++;
            }
            else
            {
                transmitCounter = 0;
                transmitFlag = FLAG_GENERATING_CRC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP2. GENERATING CRC
        else if(transmitFlag == FLAG_GENERATING_CRC)
        {
            transmitFlag = FLAG_SENDING_CRC;
        }

        ////////////////////////////////////////////////////////
        // STEP3. SENDING CRC
        else if(transmitFlag == FLAG_SENDING_CRC)
        {
            if(transmitCounter < SIZE_OF_CRC)
            {
                if(read_bit(transmitCrcBuffer, SIZE_OF_CRC, transmitCounter))
                    _SEND_LOGICAL_1_;
                else
                    _SEND_LOGICAL_0_;

                transmitCounter++;
            }
            else
            {
                transmitCounter = 0;
                transmitFlag = FLAG_SENDING_SIZE;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP4. SENDING SIZE
        else if(transmitFlag == FLAG_SENDING_SIZE)
        {
            if(transmitCounter < SIZE_OF_DLC)
            {
                if(read_bit(transmitDlcBuffer, SIZE_OF_DLC, transmitCounter))
                    _SEND_LOGICAL_1_;
                else
                    _SEND_LOGICAL_0_;

                transmitCounter++;
            }

            else
            {
                transmitCounter = 0;
                transmitFlag = FLAG_SENDING_PAYLOAD;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP5. SENDING MESSAGE
        else if(transmitFlag == FLAG_SENDING_PAYLOAD)
        {
            if(transmitCounter < SIZE_OF_PAYLOAD)
            {
                if(read_bit(transmitPayload, SIZE_OF_PAYLOAD, transmitCounter))
                    _SEND_LOGICAL_1_;
                else
                    _SEND_LOGICAL_0_;

                transmitCounter++;
            }
            
            else
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
volatile uint16_t receiveCounter = 0;

uint8_t receivePreambleBuffer[1] = {0x00};
const uint8_t preambleReadyMsg[18] = "Preamble Detected";

uint8_t receiveCrcBuffer[4] = {0x00, 0x00, 0x00, 0x00};
const uint8_t crcReceivedMsg[13] = "CRC Received";

uint8_t receiveDlcBuffer[1] = {0x00};
const uint8_t dlcReceivedMsg[13] = "DLC Received";

//static uint32_t sizeOfPayload = 0;
uint8_t receivePayloadBuffer[4] = {0x00, 0x00, 0x00, 0x00};
const uint8_t payloadReceivedMsg[17] = "Payload Received";
ISR(PCINT2_vect)
{
    ////////////////////////////////////////////////////////
    // STEP1. DETECTING PREAMBLE
    if(receiveFlag == FLAG_DETECTING_PREAMBLE)
    {
        update_preamble_buffer(receivePreambleBuffer, receive_data());
        print(receivePreambleBuffer, SIZE_OF_PREAMBLE);
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
    else if(receiveFlag == FLAG_RECEIVING_CRC)
    {
        if(receiveCounter < SIZE_OF_CRC)
        {
            // Receiving CRC
            update(receiveCrcBuffer, receiveCounter, receive_data());
            print(receiveCrcBuffer, SIZE_OF_CRC);
            uart_transmit('\r');
            receiveCounter++;
        }

        // Received CRC
        else
        {
            // Message Log
            uart_changeLine();
            print_msg(crcReceivedMsg, 12);
            uart_changeLine();
            uart_changeLine();

            // Initialization
            for(int i=0; i<4; i++)
                receiveCrcBuffer[i] = 0x00;
            receiveCounter = 0;

            // Change Flag
            receiveFlag = FLAG_RECEIVING_SIZE;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP3. RECEIVING SIZE
    else if(receiveFlag == FLAG_RECEIVING_SIZE)
    {
        if(receiveCounter < SIZE_OF_DLC)
        {
            // Receiving DLC
            update(receiveDlcBuffer, receiveCounter, receive_data());
            print(receiveDlcBuffer, SIZE_OF_DLC);
            uart_transmit('\r');
            receiveCounter++;
        }
        // Received DLC
        else
        {
            // Message Log
            uart_changeLine();
            print_msg(dlcReceivedMsg, 12);
            uart_changeLine();
            uart_changeLine();

            // Initialization
            receiveDlcBuffer[0] = 0x00;
            receiveCounter = 0;

            // Change Flag
            receiveFlag = FLAG_RECEIVING_PAYLOAD;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP4. RECEIVING MESSAGE
    else if(receiveFlag == FLAG_RECEIVING_PAYLOAD)
    {
        if(receiveCounter < SIZE_OF_PAYLOAD)
        {
            // Receiving PAYLOAD
            update(receivePayloadBuffer, receiveCounter, receive_data());
            print(receivePayloadBuffer, 32);
            uart_transmit('\r');
            receiveCounter++;
        }
        // Received Payload
        else
        {
            // Message Log
            uart_changeLine();
            print_msg(payloadReceivedMsg, 16);
            uart_changeLine();
            uart_changeLine();
            
            // Initialization
            for(int i=0; i<4; i++)
                receivePayloadBuffer[i] = 0x00;
            receiveCounter = 0;

            // Change Flag
            receiveFlag = FLAG_CHECKING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP5. CHECKING CRC
    else if(receiveFlag == FLAG_CHECKING_CRC)
    {
        //receiveFlag = FLAG_DETECTING_PREAMBLE;
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
