#pragma once
#include "interrupt.h"
#include "crc.h"
#include "uart.h"

#define FLAG_DETECTING_PREAMBLE     0
#define FLAG_

void layer2(frame_t* frame)
{
    ////////////////////////////////////////////////////////
    // STEP1. DETECTING PREAMBLE
    if(rFlag == FLAG_DETECTING_PREAMBLE)
    {
        /* Receiving Data */
        updateBit(rQueue, (rCounter%8), receiveData());
            
        /* Printing Received-Bits-String */
        printBit(rQueue, SIZE_OF_PREAMBLE);
        uart_transmit('\r');

        /* Detected the Preamble */
        if(checkPreamble(*rQueue))
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_preamble, 17);
            uart_changeLine();
            uart_changeLine();

            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_RECEIVING_DESTINATION;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP2. RECEIVING DESTINATION ADDRESS
    else if(rFlag == FLAG_RECEIVING_DESTINATION)
    {
        /* Receiving Data */
        updateBit(rFrame->dst, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rFrame->dst, SIZE_OF_ADDRESS);
        uart_transmit('\r');

        if((++rCounter) >= SIZE_OF_ADDRESS)
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_dst, 20);
            uart_changeLine();
            uart_changeLine();

            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_RECEIVING_SOURCE;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP3. RECEIVING SOURCE ADDRESS
    else if(rFlag == FLAG_RECEIVING_SOURCE)
    {
        /* Receiving Data */
        updateBit(rFrame->src, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rFrame->src, SIZE_OF_ADDRESS);
        uart_transmit('\r');

        if((++rCounter) >= SIZE_OF_ADDRESS)
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_src, 15);
            uart_changeLine();
            uart_changeLine();
            
            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_RECEIVING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP4. RECEIVING CRC
    else if(rFlag == FLAG_RECEIVING_CRC)
    {
        /* Receiving Data */
        updateBit(rFrame->crc, rCounter, receiveData());
        
        /* Printing Received-Bits-String */
        printBit(rFrame->crc, SIZE_OF_CRC);
        uart_transmit('\r');

        /* Finished Receiving CRC */
        if((++rCounter) >= SIZE_OF_CRC)
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_crc, 12);
            uart_changeLine();
            uart_changeLine();

            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_RECEIVING_DLC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP5. RECEIVING DLC
    else if(rFlag == FLAG_RECEIVING_DLC)
    {
        /* Receiving Data */
        updateBit(rFrame->dlc, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rFrame->dlc, SIZE_OF_DLC);
        uart_transmit('\r');
        
        /* Finished Receiving DLC */
        if((++rCounter) >= SIZE_OF_DLC)
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_dlc, 12);
            uart_changeLine();
            uart_changeLine();

            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_RECEIVING_PAYLOAD;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP6. RECEIVING PAYLOAD
    else if(rFlag == FLAG_RECEIVING_PAYLOAD)
    {
        /* Receiving Data */
        updateBit(rFrame->payload, rCounter, receiveData());
        
        /* Printing Received-Bits-String */
        printBit(rFrame->payload, SIZE_OF_PAYLOAD);
        uart_transmit('\r');

        /* Finished Receiving PAYLOAD */
        if((++rCounter) >= SIZE_OF_PAYLOAD)
        {
            /* Log-Messages */
            uart_changeLine();
            uart_transmit(' ');
            printMsg(logMsg_payload, 16);
            uart_changeLine();
            uart_changeLine();
            
            /* Initialization for the next cycle */
            rCounter = 0;
            rFlag = FLAG_CHECKING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    // STEP7. CHECKING CRC
    else if(rFlag == FLAG_CHECKING_CRC)
    {
        /* Checks CRC and Sets Flag */
        if((checkCrc(rFrame->crc, rFrame->payload, *(rFrame->dlc), _polynomial)))
        {
            /* Printing Received-Bits-String */
            printBit(rFrame->crc, SIZE_OF_CRC);
            uart_changeLine();

            /* Log-Messages */
            uart_transmit(' ');
            printMsg(logMsg_crc_true, 14);
            uart_changeLine();
            uart_changeLine();
        }
        else
        {
            /* Printing Received-Bits-String */
            printBit(rFrame->crc, SIZE_OF_CRC);
            uart_transmit('\r');
            uart_changeLine();

            /* Log-Messages */
            uart_transmit(' ');
            printMsg(logMsg_crc_false, 16);
            uart_changeLine();
            uart_changeLine();
        }
        
        /* Initialization for the next cycle */
        *rQueue = 0x00;
        rCounter = 0;
        rFlag = FLAG_PROCESSING_DATA;
    }
    
    ////////////////////////////////////////////////////////
    // STEP8. LAYER3
    else if(rFlag == FLAG_PROCESSING_DATA)
    {
        layer3(rFrame);
    }
}
