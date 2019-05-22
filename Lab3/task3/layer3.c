#pragma once
#include "uart.h"

#define BROADCAST_ID    0x00
#define MY_ID           0x0f
#define PRE_NODE_ID     0x09
#define NEXT_NODE_ID    0x09

void layer3(frame_t* frame)
{
    /* CASE 1. Broadcast ID 
     *  1. Receive the packet
     *  2. Relay the packet to the next node */
    if(frame->dst[0] == BROADCAST_ID)
    {
        uart_transmit('A');
    }

    /* CASE 2. My ID
     * 1. Receive the packet
     * 2. Send the ACK back to the source address */
    else if(frame->dst[0] == MY_ID)
    {
        uart_transmit('B');
    }

    /* CASE 3. Another IDs
     * 1. Relay the packet to the next node */
    else
    {
        uart_transmit('C');
    }
}
