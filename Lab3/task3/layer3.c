#pragma once
#include <stdlib.h>
#include "layer3.h"
#include "calc.c"

uint8_t checkAddress(const frame_t* frame)
{
    uint8_t result = 4;

    switch(frame->payload[1])
    {
        /* Transmitter of Message is ME */
        case MY_ID:
            result = 0;
            break;
        
        /* Transmitter of Message is Pre-Node */
        case PRE_NODE_ID:
 
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
            break;
        
        /* Transmitter of Message is Next-Node */
        //case NEXT_NODE_ID:
        //    break;
    }

    return result;
}

void insertAddress(frame_t* frame, uint32_t size, uint8_t dst, uint8_t src)
{
    rightShift(frame->payload, size, 2);
    frame->payload[0] = dst;
    frame->payload[1] = src;
}
