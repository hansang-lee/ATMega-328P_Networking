#pragma once

#define BROADCAST_ID    0x00
#define MY_ID           0x0f
#define PRE_NODE_ID     0x09
#define NEXT_NODE_ID    0x09

#define MESSAGE_TURNED_BACK         0
#define MESSAGE_BROADCAST_FROM_ME   1
#define MESSAGE_BROADCAST           2
#define MESSAGE_TO_ME               3
#define MESSAGE_TO_ANOTHER          4

uint8_t checkAddress(const frame_t* frame);
void insertAddress(frame_t* frame, uint32_t size, uint8_t dst, uint8_t src);
