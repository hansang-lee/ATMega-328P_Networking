#pragma once
#include <avr/interrupt.h>
#include "calc.h"
#include "uart.h"

uint8_t tCrcBuffer[4]       = { 0 };
uint8_t tDlcBuffer[1]       = { 0x30 };
uint8_t tPayloadBuffer[251] = { 0x74, 0x65, 0x73, 0x74 };
uint8_t tDestination[1]     = { 0x0f };
uint8_t tSource[1]          = { 0x0f };
uint32_t tFlag = FLAG_GENERATING_CRC;
uint8_t tCounter = 0;

void dummyTransmit()
{
    /* STEP1. GENERATING CRC */
    if(tFlag == FLAG_GENERATING_CRC)
    {
        bufferClear(tCrcBuffer, 32);
        rightShift(tPayloadBuffer, *tDlcBuffer, 2);
        tPayloadBuffer[0] = *tDestination;
        tPayloadBuffer[1] = *tSource;
        generateCrc(tCrcBuffer, tPayloadBuffer, *tDlcBuffer, _polynomial);
       
        tCounter = 0;
        tFlag = FLAG_SENDING_PREAMBLE;
    }

    /* STEP2. SENDING PREAMBLE */
    else if(tFlag == FLAG_SENDING_PREAMBLE)
    {
        if(readBit(_preamble, tCounter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++tCounter) >= 8)
        {
            tCounter = 0;
            tFlag = FLAG_SENDING_CRC;
        }
    }
   
    /* STEP3. SENDING CRC */
    else if(tFlag == FLAG_SENDING_CRC)
    {
        if(readBit(tCrcBuffer, tCounter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++tCounter) >= 32)
        {
            tCounter = 0;
            tFlag = FLAG_SENDING_DLC;
        }
    }
    
    /* STEP4. SENDING DLC */
    else if(tFlag == FLAG_SENDING_DLC)
    {
        if(readBit(tDlcBuffer, tCounter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();
     
        if((++tCounter) >= 8)
        {
            tCounter = 0;
            tFlag = FLAG_SENDING_PAYLOAD;
        }
    }

    /* STEP5. SENDING PAYLOAD */
    else if(tFlag == FLAG_SENDING_PAYLOAD)
    {
        if(readBit(tPayloadBuffer, tCounter)) SEND_DATA_ONE();
        else SEND_DATA_ZERO();

        if((++tCounter) >= *tDlcBuffer)
        {
            tCounter = 0;
            tFlag = 9999;
        }
    }
}

