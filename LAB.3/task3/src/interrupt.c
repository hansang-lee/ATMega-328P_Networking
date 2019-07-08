#pragma once
#include <avr/interrupt.h>
#include "interrupt.h"
#include "calc.c"
#include "uart.c"
#include "layer3.c"

/*! Data-Signal Interrupt - Packet Transmitter */
ISR(TIMER0_COMPA_vect)
{
	if ((timerA++) > INTERRUPT_PERIOD)
	{
        if(((pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)))
        {
            switch(tFlag)
            {
                // Step 1. Sending Preamble
                case FLAG_SENDING_PREAMBLE:
                    if(readBit(_preamble, tCounter)) 
						SEND_DATA_ONE(); 
                    else 
						SEND_DATA_ZERO(); 
                    if((++tCounter) >= 8)
                    { 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_CRC; 
                    }
                    break;

                // Step 2. Sending Crc
                case FLAG_SENDING_CRC:
                    if(readBit(tFrame->crc, tCounter)) 
						SEND_DATA_ONE(); 
                    else 
						SEND_DATA_ZERO(); 
                    if((++tCounter) >= 32)
                    { 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_DLC; 
                    }
                    break;

                // Step 3. Sending Size of Payload
                case FLAG_SENDING_DLC:
                    if(readBit(tFrame->dlc, tCounter)) 
						SEND_DATA_ONE();
                    else 
						SEND_DATA_ZERO(); 
                    if((++tCounter) >= 8)
                    { 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_PAYLOAD; 
                    }
                    break;

                // Step 4. Sending Payload
                case FLAG_SENDING_PAYLOAD:
                    if(readBit(tFrame->payload, tCounter)) 
						SEND_DATA_ONE();
                    else 
						SEND_DATA_ZERO(); 
                    if((++tCounter) >= ((tFrame->dlc[0])*8))
                    {
                        if(pFlag == PRIORITY_SEND)
                        {
                            printMsg("TRANSMIT", 8); 
							uart_changeLine();
                            printFrame(tFrame); 
							uart_changeLine(); 
							uart_changeLine();
                        }

                        tCounter = 0;
                        clearFrame(tFrame);
                        tFlag = FLAG_IDLE;
                        pFlag = PRIORITY_IDLE;
                    }
                    break;
            }
		    timerA = 0;
        }
	}
}

/*! Clock-Signal Interrupt */
ISR(TIMER0_COMPB_vect)
{
	if ((timerB++) > INTERRUPT_PERIOD)
	{
		timerB = 0;
		PIN_CHANGE();
	}
}

/*! Pin-Change Interrupt - Packet Receiver*/
ISR(PCINT2_vect)
{
    switch(rFlag)
    {
        // Step 1. Detecting Preamble
        case FLAG_DETECTING_PREAMBLE:
            updateBit(rQueue, (rCounter%8), receiveData());
            if(checkPreamble(*rQueue, *_preamble))
            {
                *rQueue = 0;
                rCounter = 0;
                rFlag = FLAG_RECEIVING_CRC;
            }
            break;

        // Step 2. Receiving Crc
        case FLAG_RECEIVING_CRC:
            updateBit(rFrame->crc, rCounter, receiveData());
            if((++rCounter) >= 32)
            {
                rCounter = 0;
                rFlag = FLAG_RECEIVING_DLC;
            }
            break;

        // Step 3. Receiving Dlc
        case FLAG_RECEIVING_DLC:
            updateBit(rFrame->dlc, rCounter, receiveData());
            if((++rCounter) >= 8)
            {
                rCounter = 0;
                rFlag = FLAG_RECEIVING_PAYLOAD;
            }
            break;

        // Step 4. Receiving Payload
        case FLAG_RECEIVING_PAYLOAD:
            updateBit(rFrame->payload, rCounter, receiveData());
            if((++rCounter) >= ((rFrame->dlc[0])*8))
            {
                rCounter = 0;
                rFlag = FLAG_CHECKING_CRC;
            }
            break;

        // Step 5. Checking Crc
        case FLAG_CHECKING_CRC:
            if((makeCrc(rFrame->crc, rFrame->payload, *(rFrame->dlc), _polynomial, CHECK)))
            {
                // Checking Source-Address and Destination-Address
                switch(checkAddress(rFrame))
                {
					// Case 1. Message that you sent has returned
                    case RETURNED:
                        printMsg("TURN BACK", 9); 
						uart_changeLine(); 
						uart_changeLine();
                        clearFrame(rFrame);
                        break;

					// Case 2. Broadcast Message that you sent has returned
                    case MY_BROADCAST:
                        clearFrame(rFrame);
                        break;

					// Case 3. Broadcast Message
                    case BROADCAST:
                        printMsg("RECEIVE", 7); 
						uart_changeLine();
                        printFrame(rFrame); 
						uart_changeLine();
                        printMsg("CRC OK", 6); 
						uart_changeLine();
                        printMsg("BROADCAST", 9); 
						uart_changeLine(); 
						uart_changeLine();
                        *sFrame = *rFrame;
                        pFlag = PRIORITY_LOCK;
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        tFlag = FLAG_SENDING_PREAMBLE;
                        pFlag = PRIORITY_RELAY;
                        break;
						
					// Case 4. Message to me
                    case MY_MSG:
                        printMsg("RECEIVE", 7); 
						uart_changeLine();
                        printFrame(rFrame); 
						uart_changeLine();
                        printMsg("CRC OK", 6); 
						uart_changeLine();
                        printMsg("MESSAGE TO ME", 13);
                        uart_changeLine(); 
						uart_changeLine();
                        *sFrame = *rFrame;
                        clearFrame(rFrame);
                        break;

					// Case 5. Message to another
                    case OTHER_MSG:
                        pFlag = PRIORITY_LOCK;
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        tFlag = FLAG_SENDING_PREAMBLE;
                        pFlag = PRIORITY_RELAY;
                        break;
                }
                rFlag = FLAG_DETECTING_PREAMBLE;
            }
            else
            {
                printMsg("CRC NO", 6);
                uart_changeLine(); 
				uart_changeLine();
                clearFrame(rFrame);
                rFlag = FLAG_DETECTING_PREAMBLE;
            }
            rCounter = 0;
            break;
    }
}
