#pragma once
#include "layer3.h"
#include "calc.c"

uint8_t checkAddress(const frame_t* frame)
{
    uint8_t result = 0;
    uint8_t dst = frame->payload[0];
    uint8_t src = frame->payload[1];

    if(src == MY_ID)
    {
        if(dst == BROADCAST_ID)
            result = MY_BROADCAST;       
        else 
            result = RETURNED;
    }
    else
    {
        if(dst == BROADCAST_ID) 
            result = BROADCAST;
        else if(dst == MY_ID) 
            result = MY_MSG;
        else 
            result = OTHER_MSG;
    }

    return result;
}
