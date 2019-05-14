#define _SIZE_OF_PAYLOAD_   6

volatile typedef struct data_frame_t
{
    unsigned char preamble;
    unsigned int crc;
    unsigned char payload[_SIZE_OF_PAYLOAD_];
}frame_t;