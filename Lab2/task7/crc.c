#include "crc.h"
#include <stdbool.h>

void set_preamble(frame_t* frame, char preamble)
{
    frame->preamble = preamble;
}

void set_payload(frame_t* frame, char payload[], int payloadSize)
{
    for(int i=0; i<payloadSize; i++)
    {
        frame->payload[i] = payload[i];
    }
}

void set_crc(frame_t* frame, char payload[], int payloadSize)
{
    // STEP. DIVIDOR = _POLINOMIAL_
    // STEP. DIVIDEE = (DATA << 32)
    //
    // STEP. LOOP INITIAL VALUE = (_POLINOMIAL_ << ((_SIZE_OF_PAYLOAD_ * 8) - 1))
    // STEP. LOOP UPPERBOUND = ((_SIZE_OF_PAYLOAD_ * 8) - 1))
    // STEP. LOOP CONDITION =
    // 
    // UPPERBOUND = ((_SIZE_OF_PAYLOAD_ * 8) - 1);
    // DIVIDOR = (_POLINOMIAL_ << UPPERBOUND);
    // DIVIDEE = (PAYLOAD << 32);
    // for(int i=0; i<UPPERBOUND; i++)
    // {
    //     DIVIDEE ^= DIVIDOR;
    //     DIVIDOR >> 1;
    // }
    // TRANSMIT_DATA = (DATA << 31) -
    //
    // STEP. Generate Remainder
}

/* Receiver */
bool compare_preamble(frame_t* frame, char preamble);
bool compare_crc(frame_t* frame);

bool compare_preamble(frame_t* frame, char preamble)
{
    if(frame->preamble & preamble)
        return true;
    return false;
}

bool compare_crc(frame_t* frame, )
{

}