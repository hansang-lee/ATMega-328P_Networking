#pragma once
#include <stdlib.h>
#include "layer3.h"
#include "calc.c"

uint8_t checkAddress(const frame_t* frame)
{
    // CASE 0. Turn the message back
    if(frame->payload[1] == MY_ID)
    {
        return 0;
    }

    // CASE 1. Broadcast ID 
    else if(frame->payload[0] == BROADCAST_ID)
    {
        return 1;
    }

    // CASE 2. My ID
    else if(frame->payload[0] == MY_ID)
    {
        return 2;
    }

    // CASE 3. Another IDs
    else
    {
        return 3;
    }
}

void insertAddress(frame_t* frame, uint32_t size, uint8_t dst, uint8_t src)
{
    rightShift(frame->payload, size, 2);
    frame->payload[0] = dst;
    frame->payload[1] = src;
}
