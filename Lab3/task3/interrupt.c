#include "interrupt.h"
#include "crc.h"

/* Transmitter Interrupt */
ISR(TIMER0_COMPA_vect)
{
	if ((timerA++) > INTERRUPT_PERIOD)
	{
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        // STEP1. GENERATING CRC
        if(tFlag == FLAG_GENERATING_CRC)
        {
            /* Initializes CRC Buffer */
            for(int i=0; i<(SIZE_OF_CRC/8); i++)
                tCrcBuffer[i] = 0x00;

            /* Calculates CRC */
            generateCrc(
                    tCrcBuffer,         // destination
                    tPayloadBuffer,     // source
                    *tDlcBuffer,        // source_size
                    tPolynomial);       // polynomial
       
            /* Initialization for the next flag */
            tCounter = 0;
            tFlag = FLAG_SENDING_PREAMBLE;
        }

        ////////////////////////////////////////////////////////
        // STEP2. SENDING PREAMBLE
        else if(tFlag == FLAG_SENDING_PREAMBLE)
        {
            /* Sending Preamble */
            if(readBit(tPreambleBuffer, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();

            /* Finished Sending Preamble */
            if((++tCounter) >= SIZE_OF_PREAMBLE)
            {
                /* Initialization for the next flag */
                tCounter = 0;
                tFlag = FLAG_SENDING_DESTINATION;
            }
        }


        ////////////////////////////////////////////////////////
        // STEP3. SENDING DESTINATION ADDRESS
        else if(tFlag == FLAG_SENDING_DESTINATION)
        {
            /* Sending Destination */
            if(readBit(tDestination, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();

            /* Finished Sending Destination */
            if((++tCounter) >= SIZE_OF_ADDRESS)
            {
                tCounter = 0;
                tFlag = FLAG_SENDING_SOURCE;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP4. SENDING SOURCE ADDRESS
        else if(tFlag == FLAG_SENDING_SOURCE)
        {
            /* Sending Source */
            if(readBit(tSource, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();

            /* Finished Sending Source */
            if((++tCounter) >= SIZE_OF_ADDRESS)
            {
                tCounter = 0;
                tFlag = FLAG_SENDING_CRC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP5. SENDING CRC
        else if(tFlag == FLAG_SENDING_CRC)
        {
            /* Sending CRC */
            if(readBit(tCrcBuffer, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();
            
            /* Finished Sending CRC */
            if((++tCounter) >= SIZE_OF_CRC)
            {
                /* Initialization for the next flag */
                tCounter = 0;
                tFlag = FLAG_SENDING_DLC;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP6. SENDING DLC
        else if(tFlag == FLAG_SENDING_DLC)
        {
            /* Sending DLC */
            if(readBit(tDlcBuffer, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();

            /* Finished Sending DLC */
            if((++tCounter) >= SIZE_OF_DLC)
            {
                /* Initialization for the next flag */
                tCounter = 0;
                tFlag = FLAG_SENDING_PAYLOAD;
            }
        }

        ////////////////////////////////////////////////////////
        // STEP7. SENDING PAYLOAD
        else if(tFlag == FLAG_SENDING_PAYLOAD)
        {
            /* Sending Payload */
            if(readBit(tPayloadBuffer, tCounter)) SEND_DATA_ONE();
            else SEND_DATA_ZERO();
            
            /* Finished Sending Payload */
            if((++tCounter) >= SIZE_OF_PAYLOAD)
            {
                /* Initialization for the next flag */
                tCounter = 0;
                tFlag = 9999;//FLAG_SENDING_PREAMBLE;
            }
        }
        ////////////////////////////////////////////////////////
	}
}

/* Receiver Pin-Change-Interrupt */
ISR(PCINT2_vect)
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
        updateBit(rDestination, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rDestination, SIZE_OF_ADDRESS);
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
        updateBit(rSource, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rSource, SIZE_OF_ADDRESS);
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
        updateBit(rCrcBuffer, rCounter, receiveData());
        
        /* Printing Received-Bits-String */
        printBit(rCrcBuffer, SIZE_OF_CRC);
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
        updateBit(rDlcBuffer, rCounter, receiveData());

        /* Printing Received-Bits-String */
        printBit(rDlcBuffer, SIZE_OF_DLC);
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
        updateBit(rPayloadBuffer, rCounter, receiveData());
        
        /* Printing Received-Bits-String */
        printBit(rPayloadBuffer, SIZE_OF_PAYLOAD);
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
        if((checkCrc(rCrcBuffer, rPayloadBuffer, *rDlcBuffer, tPolynomial)))
        {
            /* Printing Received-Bits-String */
            printBit(rCrcBuffer, SIZE_OF_CRC);
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
            printBit(rCrcBuffer, SIZE_OF_CRC);
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
        for(int i=0; i<4; i++) {rCrcBuffer[i] = 0x00;}
        rDlcBuffer[0] = 0x00;
        for(int i=0; i<4; i++) {rPayloadBuffer[i] = 0x00;}
        rCounter = 0;
        rFlag = FLAG_DETECTING_PREAMBLE;
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
