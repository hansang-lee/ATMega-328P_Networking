#pragma once
#include <stdlib.h>
#include "layer3.h"
#include "calc.c"

uint8_t checkAddress(const frame_t* frame)
{
    uint8_t result = 0;
    uint8_t dst = frame->payload[0];
    uint8_t src = frame->payload[1];

    if(src == MY_ID)
    {
        if(dst == BROADCAST_ID) result = MESSAGE_BROADCAST_FROM_ME;
        else result = MESSAGE_TURNED_BACK;
    }

    else
    {
        // Broadcast Message
        if(frame->payload[0] == BROADCAST_ID)
        {
            result = MESSAGE_BROADCAST;
        }

        // Message To Me
        else if(frame->payload[0] == MY_ID)
        {
            result = MESSAGE_TO_ME;
        }
 
        // Message To Another Nodes
        else
        {
            result = MESSAGE_TO_ANOTHER;
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
