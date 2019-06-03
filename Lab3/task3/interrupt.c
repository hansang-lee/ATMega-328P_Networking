#pragma once
#include <avr/interrupt.h>
#include <stdlib.h>
#include "interrupt.h"
#include "calc.c"
#include "uart.c"
#include "layer3.c"

/* Interrupt A - Transmitter */
ISR(TIMER0_COMPA_vect)
{
	if ((timerA++) > INTERRUPT_PERIOD)
	{
        if((pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY))
        {
            switch(tFlag)
            {
                // GENERATING CRC
                //case FLAG_GENERATING_CRC:
                //    bufferClear(tFrame->crc, 32);
                //    generateCrc(tFrame->crc, tFrame->payload, tFrame->dlc[0], _polynomial);
                //    tCounter = 0; 
                //    tFlag = FLAG_SENDING_PREAMBLE;
                //    break;

                // SENDING PREAMBLE
                case FLAG_SENDING_PREAMBLE:
                    if(readBit(_preamble, tCounter)) SEND_DATA_ONE();
                    else SEND_DATA_ZERO();
                    if((++tCounter) >= 8) { tCounter = 0; tFlag = FLAG_SENDING_CRC; }
                    break;

                // SENDING CRC
                case FLAG_SENDING_CRC:
                    if(readBit(tFrame->crc, tCounter)) SEND_DATA_ONE();
                    else SEND_DATA_ZERO();
                    if((++tCounter) >= 32) { tCounter = 0; tFlag = FLAG_SENDING_DLC; }
                    break;

                // SENDING DLC
                case FLAG_SENDING_DLC:
                    if(readBit(tFrame->dlc, tCounter)) SEND_DATA_ONE();
                    else SEND_DATA_ZERO();
                    if((++tCounter) >= 8) { tCounter = 0; tFlag = FLAG_SENDING_PAYLOAD; }
                    break;

                // SENDING PAYLOAD
                case FLAG_SENDING_PAYLOAD:
                    if(readBit(tFrame->payload, tCounter)) SEND_DATA_ONE();
                    else SEND_DATA_ZERO();
                    if((++tCounter) >= tFrame->dlc[0])
                    {
                        tCounter = 0;
                        clearFrame(tFrame);
                        tFlag = FLAG_IDLE;
                        pFlag = PRIORITY_READ;
                    }
                    break;

                default:
                    break;
            }
		    timerA = 0;
        }
	}
}

/* Pin-Change-Interrupt - Receiver */
ISR(PCINT2_vect)
{
    //if(pFlag == PRIORITY_READ)
    {
        switch(rFlag)
        {
            // DETECTING PREAMBLE
            case FLAG_DETECTING_PREAMBLE:
                updateBit(rQueue, (rCounter%8), receiveData());
                if(checkPreamble(*rQueue, *_preamble))
                {
                    printBit(rQueue, 0, 8);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_preamble, 17); uart_changeLine(); uart_changeLine();
                    *rQueue = 0;
                    rCounter = 0;
                    rFlag = FLAG_RECEIVING_CRC;
                }
                break;

            // RECEIVING CRC
            case FLAG_RECEIVING_CRC:
                updateBit(rFrame->crc, rCounter, receiveData());
                if((++rCounter) >= 32)
                {
                    printBit(rFrame->crc, 0, 32);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_crc, 12); uart_changeLine(); uart_changeLine();
                    rCounter = 0;
                    rFlag = FLAG_RECEIVING_DLC;
                }
                break;

            // RECEIVING DLC
            case FLAG_RECEIVING_DLC:
                updateBit(rFrame->dlc, rCounter, receiveData());
                if((++rCounter) >= 8)
                {
                    printBit(rFrame->dlc, 0, 8);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_dlc, 12); uart_changeLine(); uart_changeLine();
                    rCounter = 0;
                    rFlag = FLAG_RECEIVING_PAYLOAD;
                }
                break;

            // RECEIVING PAYLOAD
            case FLAG_RECEIVING_PAYLOAD:
                updateBit(rFrame->payload, rCounter, receiveData());
                if((++rCounter) >= rFrame->dlc[0])
                {
                    printBit(rFrame->payload, 0, 8);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_dst, 20); uart_changeLine(); uart_changeLine();
            
                    printBit(rFrame->payload, 8, 16);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_src, 15); uart_changeLine(); uart_changeLine();

                    printBit(rFrame->payload, 16, rFrame->dlc[0]);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_payload, 17); uart_changeLine(); uart_changeLine();

                    rCounter = 0;
                    rFlag = FLAG_CHECKING_CRC;
                }
                break;

            // CHECKING CRC
            case FLAG_CHECKING_CRC:
                if((checkCrc(rFrame->crc, rFrame->payload, *(rFrame->dlc), _polynomial)))
                {
                    printBit(rFrame->crc, 0, 32);
                    uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_crc_true, 14); uart_changeLine(); uart_changeLine();
                }
                else
                {
                    printBit(rFrame->crc, 0, 32);
                    uart_transmit('\r'); uart_changeLine(); uart_transmit(' ');
                    printMsg(logMsg_crc_false, 16); uart_changeLine(); uart_changeLine();
                }
                rCounter = 0;
                rFlag = FLAG_LAYER_3;
                break;
    
            // Check Destination and Source
            case FLAG_LAYER_3:
                switch(checkAddress(rFrame))
                {
                    case MESSAGE_TURNED_BACK: // Message Turned Back
                        uart_transmit(' ');
                        printMsg(msg_turnBack, 27); uart_changeLine(); uart_changeLine();
                        clearFrame(rFrame);
                        break;

                    case MESSAGE_BROADCAST_FROM_ME:
                        break;

                    case MESSAGE_BROADCAST: // Broadcast Message
                        uart_transmit(' ');
                        printMsg(msg_broadcast, 17); uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;

                        /* Message Relay */
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        pFlag = PRIORITY_RELAY;
                        break;

                    case MESSAGE_TO_ME: // Message to Me
                        uart_transmit(' ');
                        printMsg(msg_toHere, 13);
                        uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;
                        clearFrame(rFrame);
                        break;

                    case MESSAGE_TO_ANOTHER: // Message To Another Nodes
                        uart_transmit(' ');
                        printMsg(msg_toAnother, 18);
                        uart_changeLine(); uart_changeLine();

                        /* Message Relay */
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        pFlag = PRIORITY_RELAY;
                        break;
                }
                rFlag = FLAG_DETECTING_PREAMBLE;
                break;
        }
    }
}

/* Clock Signal Interrupt */
ISR(TIMER0_COMPB_vect)
{
	if ((timerB++) > INTERRUPT_PERIOD)
	{
		timerB = 0;
        
        ////////////////////////////////////////////////////////
		/* CLOCK SIGNAL TRANSMIT */
		PIN_CHANGE();
        ////////////////////////////////////////////////////////
	}
}
