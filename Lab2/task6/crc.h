#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NO_ERROR        0
#define ERROR_OCCUR     1
#define TRUE            0
#define FALSE           1
#define PREAMBLE        0x7e
#define POLYNOMIAL      0b100000100110000010001110110110111

///////////////////////////////////////////////////////////////////
// Data Frame
typedef struct data_frame_t
{
    uint32_t crc32;     // 0-31
    uint8_t size;       // 32-39
    uint8_t* payload;   // 40-2048
} frame_t;

///////////////////////////////////////////////////////////////////
// Transmitting / Receiving
void print_msg(const uint8_t* msg, const uint16_t length)
{
    for(int i=0; i<length; i++)
    {
        uart_transmit(msg[i]);
    }
}

uint32_t read_bit(const uint8_t* bitstring, const uint32_t bits, const uint32_t pos)
{
    if((bitstring[(pos/8)] & (0b10000000 >> (pos%8))))
        return 1;
    else
        return 0;
}

uint8_t receive_data()
{
    if(_RECEIVED_DATA_)
        return 0x01;
    else
        return 0x00;
}

void update(uint8_t* buffer, const uint32_t pos, const uint8_t data)
{
    int i = (pos/8);

    buffer[i] &= 0b01111111;
    buffer[i] <<= 1;
    buffer[i] += data;
}

void print(const uint8_t* buffer, const uint32_t bits)
{
    for(int i=0; i<bits; i++)
    {
        if(buffer[(i/8)] & (0b10000000 >> (i%8)))
            uart_transmit('1');
        else
            uart_transmit('0');
    }
}

void update_preamble_buffer(uint8_t* buffer, const uint8_t data)
{
    *buffer &= 0b01111111;
    *buffer <<= 1;
    *buffer += data;
}

uint16_t check_preamble(const uint8_t preambleBuffer)
{
    if((preambleBuffer ^ PREAMBLE) == 0)
        return TRUE;
    return FALSE;
}
