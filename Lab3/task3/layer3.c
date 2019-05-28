#pragma once
#include <stdlib.h>
#include "layer3.h"
#include "calc.c"

uint8_t checkAddress(const frame_t* frame)
{
    uint8_t result = 4;

    if(frame->payload[1] == MY_ID)
    {
        /* Transmitter of Message is ME */
        result = 0;
    }

    else
    {
        // Broadcast Message
        if(frame->payload[0] == BROADCAST_ID)
        {
            result = 1;
        }

        // Message To Me
        else if(frame->payload[0] == MY_ID)
        {
            result = 2;
        }
 
        // Message To Another Nodes
        else
        {
            result = 3;
        } 
    }

    return result;
}

void insertAddress(frame_t* frame, uint32_t size, uint8_t dst, uint8_t src)
{
    rightShift(frame->payload, size, 2);
    frame->payload[0] = dst;
    frame->payload[1] = src;
}
