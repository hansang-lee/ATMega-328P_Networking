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
                    else { SEND_DATA_ZERO(); uart_transmit('0'); }
                    
                    if((++tCounter) >= ((tFrame->dlc[0])*8))
                    {
                        if(pFlag == PRIORITY_SEND) uart_changeLine(); uart_changeLine();
                        tCounter = 0;
                        clearFrame(tFrame);
                        tFlag = FLAG_IDLE;
                        pFlag = PRIORITY_IDLE;
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
                if((checkCrc(rFrame->crc, rFrame->payload, *(rFrame->dlc), _polynomial)))
                {
                    printMsg("CRC OK", 6);
                    printBit(rFrame->crc, 0, 32);
                    uart_changeLine();
                    rFlag = FLAG_LAYER_3;
                }
                else
                {
                    printMsg("CRC NO", 6);
                    printBit(rFrame->crc, 0, 32);
                    uart_changeLine();
                    clearFrame(rFrame);
                    rFlag = FLAG_DETECTING_PREAMBLE;
                }
                rCounter = 0;
                break;

            // CHECKING ADDRESS
            case FLAG_LAYER_3:
                switch(checkAddress(rFrame))
                {
                    case MESSAGE_TURNED_BACK: // Message Turned Back X
                        uart_changeLine();
                        printMsg("TURN BACK", 9); uart_changeLine(); uart_changeLine();
                        clearFrame(rFrame);
                        break;

                    case MESSAGE_BROADCAST_FROM_ME: // Broadcast Message From Me X
                        break;

                    case MESSAGE_BROADCAST: // Broadcast Message O
                        printFrame(rFrame);
                        uart_changeLine();
                        printMsg("BROADCAST", 9); uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;

                        /* Message Relay X */
                        pFlag = PRIORITY_LOCK;
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        tFlag = FLAG_SENDING_PREAMBLE;
                        pFlag = PRIORITY_RELAY;
                        break;

                    case MESSAGE_TO_ME: // Message to Me O
                        printFrame(rFrame);
                        uart_changeLine();
                        printMsg("MSG TO ME", 9);
                        uart_changeLine(); uart_changeLine();
                        *sFrame = *rFrame;
                        clearFrame(rFrame);
                        break;

                    case MESSAGE_TO_ANOTHER: // Message To Another Nodes X
                        uart_changeLine();
                        printMsg("TO ANOTHER", 10);
                        uart_changeLine(); uart_changeLine();

                        /* Message Relay X */
                        pFlag = PRIORITY_LOCK;
                        *tFrame = *rFrame;
                        clearFrame(rFrame);
                        tFlag = FLAG_SENDING_PREAMBLE;
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
