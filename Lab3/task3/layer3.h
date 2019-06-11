#pragma once

#define BROADCAST_ID    0x00
#define MY_ID           0x0f
#define NEXT_ID         0x09

#define RETURNED        1
#define MY_BROADCAST    2
#define BROADCAST       3
#define MY_MSG          4
#define OTHER_MSG       5

uint8_t checkAddress(const frame_t* frame);
