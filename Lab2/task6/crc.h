#include <stdbool.h>

#define _SIZE_OF_PAYLOAD_   4
#define _PREAMBLE_          0b01111110
#define _POLINOMIAL_        0b100000100110000010001110110110111

//********************************************************************
/* Data Frame Format */
//********************************************************************
volatile typedef struct data_frame_t
{
    unsigned char preamble;
    unsigned char crc32[4];
    unsigned char payload[_SIZE_OF_PAYLOAD_];
}frame_t;

//********************************************************************
/* CRC Functions */
//********************************************************************
// Transmitter
void set_preamble(frame_t* frame);
void set_payload(frame_t* frame, char payload[], int length);
void set_crc32(frame_t* frame);

// Receiver
bool check_preamble(frame_t* frame);
bool check_crc(frame_t* frame);

//********************************************************************
/* Implementations */
//********************************************************************
void set_preamble(frame_t* frame)
{
	frame->preamble = _PREAMBLE_;
}

void set_payload(frame_t* frame, char payload[], int length)
{
	for(int i=0; i<length; i++)
	{
		frame->payload[i] = payload[i];
	}
}

void set_crc32(frame_t* frame)
{
	int upperbound = ((_SIZE_OF_PAYLOAD_ * 8) -1);
	
}

bool check_preamble(frame_t frame)
{
    if(_PREAMBLE_ & preamble)
        return true;
    return false;
}

bool check_crc(frame_t* frame)
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
