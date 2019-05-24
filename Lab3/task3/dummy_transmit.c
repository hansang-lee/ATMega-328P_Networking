#pragma once
#include <avr/interrupt.h>
#include "calc.c"
#include "uart.c"

uint8_t tCrcBuffer[4]       = { 0 };
uint8_t tDlcBuffer[1]       = { 0x20 };
uint8_t tPayloadBuffer[251] = { 0x74, 0x65, 0x73, 0x74 };
uint8_t tDestination[1]     = { 0x10 };
uint8_t tSource[1]          = { 0x09 };
uint32_t Flag = FLAG_RELAY_MESSAGE;
uint8_t Counter = 0;

void dummyTransmit()
{
    /* STEP1. GENERATING CRC */
    if(Flag == FLAG_RELAY_MESSAGE)
    {
        bufferClear(tCrcBuffer, 32);
        rightShift(tPayloadBuffer, *tDlcBuffer, 2);
        tPayloadBuffer[0] = *tDestination;
        tPayloadBuffer[1] = *tSource;
        *tDlcBuffer += 0x10;
        generateCrc(tCrcBuffer, tPayloadBuffer, *tDlcBuffer, _polynomial);
       
        Counter = 0;
        Flag = FLAG_SENDING_PREAMBLE;
    }

    /* STEP2. SENDING PREAMBLE */
    else if(Flag == FLAG_SENDING_PREAMBLE)
    {
        if(readBit(_preamble, Counter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++Counter) >= 8)
        {
            Counter = 0;
            Flag = FLAG_SENDING_CRC;
        }
    }
   
    /* STEP3. SENDING CRC */
    else if(Flag == FLAG_SENDING_CRC)
    {
        if(readBit(tCrcBuffer, Counter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++Counter) >= 32)
        {
            Counter = 0;
            Flag = FLAG_SENDING_DLC;
        }
    }
    
    /* STEP4. SENDING DLC */
    else if(Flag == FLAG_SENDING_DLC)
    {
        if(readBit(tDlcBuffer, Counter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();
     
        if((++Counter) >= 8)
        {
            Counter = 0;
            Flag = FLAG_SENDING_PAYLOAD;
        }
    }

    /* STEP5. SENDING PAYLOAD */
    else if(Flag == FLAG_SENDING_PAYLOAD)
    {
        if(readBit(tPayloadBuffer, Counter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++Counter) >= *tDlcBuffer)
        {
            Counter = 0;
            Flag = 9999;
        }
    }
}

