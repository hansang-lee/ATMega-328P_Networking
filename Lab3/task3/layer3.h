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

const uint8_t msg_turnBack[27]  = "The Message Has Turned Back";
const uint8_t msg_broadcast[17] = "BroadCast Message";
const uint8_t msg_toHere[13]    = "Message To Me";
const uint8_t msg_toAnother[18] = "Message To Another";

/* *****************************************************
 * Return Value
 *
 *  0 : Message Turned Back
 *  1 : Boardcast Message
 *  2 : Message To Me
 *  3 : Message To Another Nodes
 *
 * *****************************************************/
uint8_t checkAddress(const frame_t* frame);
void insertAddress(frame_t* frame, uint32_t size, uint8_t dst, uint8_t src);
