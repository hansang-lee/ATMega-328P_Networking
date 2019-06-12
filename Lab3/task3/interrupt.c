#pragma once
#include <avr/interrupt.h>
#include "interrupt.h"
#include "calc.c"
#include "uart.c"
#include "layer3.c"

/* Interrupt A - Transmitter */
ISR(TIMER0_COMPA_vect)
{
	if ((timerA++) > INTERRUPT_PERIOD)
	{
        if(((pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)))
        {
            switch(tFlag)
            {
                // SENDING PREAMBLE
                case FLAG_SENDING_PREAMBLE:
                    if((pFlag == PRIORITY_SEND) && (tCounter == 0)) 
                    {
                        printMsg("TRANSMIT", 8); uart_changeLine(); 
                        printMsg("PRE ", 4); 
                    }
                    if(readBit(_preamble, tCounter)) 
                    { 
                        SEND_DATA_ONE(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('1'); 
                    }
                    else 
                    { 
                        SEND_DATA_ZERO(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('0'); 
                    }
                    if((++tCounter) >= 8)
                    { 
                        if(pFlag == PRIORITY_SEND) uart_changeLine(); 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_CRC; 
                    }
                    break;

                // SENDING CRC
                case FLAG_SENDING_CRC:
                    if(pFlag == PRIORITY_SEND)
                    {
                        if((tCounter > 0) && ((tCounter % 8) == 0)) uart_transmit(' ');
                        if(tCounter == 0) { printMsg("CRC ", 4); }
                    }
                    if(readBit(tFrame->crc, tCounter)) 
                    { 
                        SEND_DATA_ONE(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('1'); 
                    }
                    else 
                    { 
                        SEND_DATA_ZERO(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('0'); 
                    }
                    if((++tCounter) >= 32)
                    { 
                        if(pFlag == PRIORITY_SEND) uart_changeLine(); 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_DLC; 
                    }
                    break;

                // SENDING DLC
                case FLAG_SENDING_DLC:
                    if((pFlag == PRIORITY_SEND) && (tCounter == 0)) { printMsg("DLC ", 4); }
                    if(readBit(tFrame->dlc, tCounter)) 
                    { 
                        SEND_DATA_ONE(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('1'); 
                    }
                    else 
                    { 
                        SEND_DATA_ZERO();
                        if(pFlag == PRIORITY_SEND) uart_transmit('0');
                    }
                    if((++tCounter) >= 8)
                    { 
                        if(pFlag == PRIORITY_SEND) uart_changeLine(); 
                        tCounter = 0; 
                        tFlag = FLAG_SENDING_PAYLOAD; 
                    }
                    break;

                // SENDING PAYLOAD
                case FLAG_SENDING_PAYLOAD:
                    if(pFlag == PRIORITY_SEND)
                    {
                        if((tCounter > 0) && ((tCounter == 8) || (tCounter == 16))) uart_changeLine();
                        if((tCounter > 0) && ((tCounter % 8) == 0)) uart_transmit(' ');
                        if(tCounter == 0) { printMsg("DST ", 4); }
                        else if(tCounter == 8) { uart_transmit('\r'); printMsg("SRC ", 4); }
                        else if(tCounter == 16) { uart_transmit('\r'); printMsg("PAY ", 4); }
                    }
                    if(readBit(tFrame->payload, tCounter))
                    { 
                        SEND_DATA_ONE();
                        if(pFlag == PRIORITY_SEND) uart_transmit('1'); 
                    }
                    else 
                    { 
                        SEND_DATA_ZERO(); 
                        if(pFlag == PRIORITY_SEND) uart_transmit('0'); 
                    }
                    
                    if((++tCounter) >= ((tFrame->dlc[0])*8))
                    {
                        if(pFlag == PRIORITY_SEND) uart_changeLine(); uart_changeLine();
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

/* Pin-Change-Interrupt - Receiver */
ISR(PCINT2_vect)
{
    switch(rFlag)
    {
        // DETECTING PREAMBLE
        case FLAG_DETECTING_PREAMBLE:
            updateBit(rQueue, (rCounter%8), receiveData());
            if(checkPreamble(*rQueue, *_preamble))
            {
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
                rCounter = 0;
                rFlag = FLAG_RECEIVING_DLC;
            }
            break;

        // RECEIVING DLC
        case FLAG_RECEIVING_DLC:
            updateBit(rFrame->dlc, rCounter, receiveData());
            if((++rCounter) >= 8)
            {
                rCounter = 0;
                rFlag = FLAG_RECEIVING_PAYLOAD;
            }
            break;

        // RECEIVING PAYLOAD
        case FLAG_RECEIVING_PAYLOAD:
            updateBit(rFrame->payload, rCounter, receiveData());
            if((++rCounter) >= ((rFrame->dlc[0])*8))
            {
                rCounter = 0;
                rFlag = FLAG_CHECKING_CRC;
            }
            break;

        // CHECKING CRC
        case FLAG_CHECKING_CRC:
            //if((checkCrc(rFrame->payload, *(rFrame->dlc), _polynomial)))
            if((generateCrc(rFrame->crc, rFrame->payload, *(rFrame->dlc), _polynomial)))
            {
                // CHECKING ADDRESS
                switch(checkAddress(rFrame))
                {
                    case RETURNED: // Message Turned Back
                        printMsg("TURN BACK", 9); uart_changeLine(); uart_changeLine();
                        clearFrame(rFrame);
                        break;

                    case MY_BROADCAST: // Broadcast Message From Me
                        break;

                    case BROADCAST: // Broadcast Message
                        printMsg("RECEIVE", 7); uart_changeLine();
                        printFrame(rFrame); uart_changeLine();
                        printMsg("CRC OK ", 7); uart_changeLine();
                        printMsg("BROADCAST", 9); uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;
                        pFlag = PRIORITY_LOCK;
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        tFlag = FLAG_SENDING_PREAMBLE;
                        pFlag = PRIORITY_RELAY;
                        break;

                    case MY_MSG: // Message to Me
                        printMsg("RECEIVE", 7); uart_changeLine();
                        printFrame(rFrame); uart_changeLine();
                        printMsg("CRC OK ", 7); uart_changeLine();
                        printMsg("MESSAGE TO ME", 13);
                        uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;
                        clearFrame(rFrame);
                        break;

                    case OTHER_MSG: // Message To Another Nodes
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
                printMsg("CRC NO ", 7);
                //printBit(rFrame->crc, 0, 32);
                uart_changeLine(); uart_changeLine();
                clearFrame(rFrame);
                rFlag = FLAG_DETECTING_PREAMBLE;
            }
            rCounter = 0;
            break;
    }
}

/* Clock Signal Interrupt */
ISR(TIMER0_COMPB_vect)
{
	if ((timerB++) > INTERRUPT_PERIOD)
	{
		timerB = 0;
		PIN_CHANGE();
	}
}
