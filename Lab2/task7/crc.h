#include "data_frame.h"

#define _PREAMBLE_          0b01111110
#define _POLINOMIAL_        0b100000100110000010001110110110111

/* Transmitter */
void set_preamble(frame_t* frame, char preamble);
void set_payload(frame_t* frame, char payload[], int payloadSize);
void set_crc(frame_t* frame, char payload[], int payloadSize);