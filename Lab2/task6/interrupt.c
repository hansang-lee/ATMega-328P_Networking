#include "interrupt.h"
#include "crc.h"

/* Interrupt A - Data Transmitter */
volatile uint16_t timerA = (_PERIOD_/2);
volatile uint16_t tFlag = FLAG_GENERATING_CRC;
volatile uint16_t tCounter = 0;

uint8_t tPreambleBuffer[(SIZE_OF_PREAMBLE_BUF/8)] = { 0b01111110 };  // 0x7e
uint8_t tCrcBuffer[(SIZE_OF_CRC_BUF/8)]           = { 0x00000000 };
uint8_t tDlcBuffer[(SIZE_OF_DLC_BUF/8)]           = { 0b00100000 };  // 0x20

uint8_t tPayloadBuffer[(SIZE_OF_PAYLOAD_BUF/8)]   = { 0b01110100,    // 0x74
                                                      0b01100101,    // 0x65
                                                      0b01110011,    // 0x73
                                                      0b01110100 };  // 0x74

const uint8_t tPolynomial[5]                      = { 0b10000010,    // 0x01
                                                      0b01100000,    // 0x04
                                                      0b10001110,    // 0xc1
                                                      0b11011011,    // 0x1d
                                                      0b10000000 };  // 0xb7
ISR(TIMER0_COMPA_vect)
{
	if ((timerA++) > _PERIOD_)
	{
		//_LED_A_TOGGLE_;
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        // STEP0. GENERATING CRC
        if(tFlag == FLAG_GENERATING_CRC)
        {
            uint8_t data[5] = {0};
            uint32_t steps = (*tDlcBuffer + 1);
            
            for(int i=0; i<4; i++)
                data[i] = tPayloadBuffer[i];

            // XOR
            while(tCounter < steps)
            {
                ////////////////////////////////////////////////////
                // Debugging
                for(int i=0; i<SIZE_OF_CRC_BUF; i++)
                {
                    if((i%8)==0)
                        uart_transmit(' ');
                    if(read_bit(data, i))
                        uart_transmit('1');
                    else
                        uart_transmit('0');
                }
                uart_changeLine();
                /////
                /////

                // MSB is Zero - Left Shift
                if(!(read_bit(data, 0)))
                {
                    // data[0] ~ data[3]
                    for(int k=0; k<5; k++)
                    {
                        if(k == 0)
                            data[0] = ((data[0] & 0b01111111) << 1);
                        else
                        {
                            if(read_bit(&data[k], 0))
                                data[k-1] += 0b00000001;

                            data[k] &= 0b01111111;
                            data[k] <<= 1;
                        }
                    }
                    tCounter++;
                }
                else
                    for(int j=0; j<SIZE_OF_POLYNOMIAL; j++)
                        write_bit(data, j, (read_bit(data,j) ^ read_bit(tPolynomial,j)));
            }

            for(int i=0; i<4; i++)
                tCrcBuffer[i] = data[i];
            
            ////////////////////////////////////////////////////
            // Steping
            tCounter = 0;
            tFlag = FLAG_SENDING_PREAMBLE;
        }

        ////////////////////////////////////////////////////////
        // STEP1. SENDING PREAMBLE
        else if(tFlag == FLAG_SENDING_PREAMBLE)
        {
            if(read_bit(tPreambleBuffer, tCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            if((++tCounter) >= SIZE_OF_PREAMBLE_BUF)
            {
                tCounter = 0;
                tFlag = FLAG_SENDING_CRC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP2. SENDING CRC
        else if(tFlag == FLAG_SENDING_CRC)
        {
            if(read_bit(tCrcBuffer, tCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;
            
            if((++tCounter) >= SIZE_OF_CRC_BUF)
            {
                tCounter = 0;
                tFlag = FLAG_SENDING_DLC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP3. SENDING SIZE
        else if(tFlag == FLAG_SENDING_DLC)
        {
            if(read_bit(tDlcBuffer, tCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;

            if((++tCounter) >= SIZE_OF_DLC_BUF)
            {
                tCounter = 0;
                tFlag = FLAG_SENDING_PAYLOAD;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP5. SENDING MESSAGE
        else if(tFlag == FLAG_SENDING_PAYLOAD)
        {
            if(read_bit(tPayloadBuffer, tCounter))
                _SEND_LOGICAL_1_;
            else
                _SEND_LOGICAL_0_;
            
            if((++tCounter) >= SIZE_OF_PAYLOAD_BUF)
            {
                tCounter = 0;
                tFlag = 100;//FLAG_SENDING_PREAMBLE;
            }
        }
        ////////////////////////////////////////////////////////
	}
}

/* Pin Change Interrupt */
volatile uint16_t rFlag = FLAG_DETECTING_PREAMBLE;
volatile uint16_t rCounter = 0;

uint8_t rPreambleBuffer[1]  = {0x00};
uint8_t rCrcBuffer[4]       = {0x00, 0x00, 0x00, 0x00};
uint8_t rDlcBuffer[1]       = {0x00};
uint8_t rPayloadBuffer[4]   = {0x00, 0x00, 0x00, 0x00};

const uint8_t logMsg_preamble[18]   = "Preamble Detected";
const uint8_t logMsg_crc[13]        = "CRC Received";
const uint8_t logMsg_dlc[13]        = "DLC Received";
const uint8_t logMsg_payload[17]    = "Payload Received";

//static uint32_t sizeOfPayload = 0;
ISR(PCINT2_vect)
{
    ////////////////////////////////////////////////////////
    // STEP1. DETECTING PREAMBLE
    if(rFlag == FLAG_DETECTING_PREAMBLE)
    {
        update_preamble_buffer(rPreambleBuffer, receive_data());
        uart_transmit(' ');
        print(rPreambleBuffer, SIZE_OF_PREAMBLE_BUF);
        uart_transmit('\r');

        if(check_preamble(*rPreambleBuffer) == TRUE)
        {
            uart_changeLine();
            uart_transmit(' ');
            print_msg(logMsg_preamble, 17);
            uart_changeLine();
            uart_changeLine();
            *rPreambleBuffer = 0x00;
            rCounter = 0;
            rFlag = FLAG_RECEIVING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP2. RECEIVING CRC
    else if(rFlag == FLAG_RECEIVING_CRC)
    {
        // Receiving CRC
        update(rCrcBuffer, rCounter, receive_data());
        uart_transmit(' ');
        print(rCrcBuffer, SIZE_OF_CRC_BUF);
        uart_transmit('\r');

        // Received CRC
        if((++rCounter) >= SIZE_OF_CRC_BUF)
        {
            // Message Log
            uart_changeLine();
            uart_transmit(' ');
            print_msg(logMsg_crc, 12);
            uart_changeLine();
            uart_changeLine();

            // Initialization
            for(int i=0; i<4; i++)
                rCrcBuffer[i] = 0x00;
            rCounter = 0;

            // Change Flag
            rFlag = FLAG_RECEIVING_SIZE;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP3. RECEIVING DLC
    else if(rFlag == FLAG_RECEIVING_SIZE)
    {
        // Receiving DLC
        update(rDlcBuffer, rCounter, receive_data());
        uart_transmit(' ');
        print(rDlcBuffer, SIZE_OF_DLC_BUF);
        uart_transmit('\r');
        
        // Received DLC
        if((++rCounter) >= SIZE_OF_DLC_BUF)
        {
            // Message Log
            uart_changeLine();
            uart_transmit(' ');
            print_msg(logMsg_dlc, 12);
            uart_changeLine();
            uart_changeLine();

            // Initialization
            rDlcBuffer[0] = 0x00;
            rCounter = 0;

            // Change Flag
            rFlag = FLAG_RECEIVING_PAYLOAD;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP4. RECEIVING PAYLOAD
    else if(rFlag == FLAG_RECEIVING_PAYLOAD)
    {
        // Receiving PAYLOAD
        update(rPayloadBuffer, rCounter, receive_data());
        uart_transmit(' ');
        print(rPayloadBuffer, SIZE_OF_PAYLOAD_BUF);
        uart_transmit('\r');
        
        // Received Payload
        if((++rCounter) >= SIZE_OF_PAYLOAD_BUF)
        {
            // Message Log
            uart_changeLine();
            uart_transmit(' ');
            print_msg(logMsg_payload, 16);
            uart_changeLine();
            uart_changeLine();
            
            // Initialization
            for(int i=0; i<4; i++)
                rPayloadBuffer[i] = 0x00;
            rCounter = 0;

            // Change Flag
            rFlag = FLAG_CHECKING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP5. CHECKING CRC
    else if(rFlag == FLAG_CHECKING_CRC)
    {
        //rFlag = FLAG_DETECTING_PREAMBLE;
    }
    ////////////////////////////////////////////////////////
}

/* Interrupt B - Clock Signal */
ISR(TIMER0_COMPB_vect)
{
	static volatile uint16_t timerB = 0;
	if ((timerB++) > _PERIOD_)
	{
		//_LED_B_TOGGLE_;
		timerB = 0;
		
        ////////////////////////////////////////////////////////
		/* CLOCK SIGNAL TRANSMIT */
		_PIN_CHANGE_;
        ////////////////////////////////////////////////////////
	}
}
