#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NO_ERROR        0
#define ERROR_OCCUR     1

#define TRUE            0
#define FALSE           1

//#define PREAMBLE        0x7e        // 8 Bits
//#define DIVIDER         0x104C11DB7  // 33 Bits
//#define MESSAGE         0b11010011101100 //0xffff      // 8 Bits

/******************************************************/
/* Data Frame */
/******************************************************/
typedef struct data_frame_t
{
    unsigned int crc32;             // 0-31
    unsigned char size;             // 32-39
    unsigned char payload[2008];    // 40-2048
} frame_t;

/******************************************************/
/* Receiver Functions */
/******************************************************/
unsigned int read_bit(const unsigned char bitstring, const int pos)
{
    if((bitstring & (0b10000000 >> pos)))
        return 1;
    else
        return 0;
}

void update_preamble_buffer(unsigned char* buffer)
{
    /* STEP1. Discards the MSB */
    *buffer &= 0x7f;

    /* STEP2. Left Shift */
    *buffer <<= 1;

    /* STEP3. Inserts a new LSB */
    //preambleBuffer += ((0x01) & (PIND & (1 << PD4)));
    if(_RECEIVED_DATA_)
        *buffer += 1;
    else
        *buffer += 0;
}

unsigned int check_preamble(const unsigned char preambleBuffer)
{
    if((preambleBuffer ^ PREAMBLE) == 0)
        return TRUE;
    return TRUE;
}

void print_preamble_buffer(const unsigned char buffer)
{
    for(int i=0; i<8; i++)
    {
        if(buffer & (0b10000000 >> i))
            uart_transmit('1');
        else
            uart_transmit('0');
    }
    uart_transmit('\r');
    uart_transmit('\n');
}

unsigned int update_crc32_buffer(unsigned char* buffer)
{
    /* STEP1. Discards the MSB */
    *buffer &= 0x7f;

    /* STEP2. Left Shift */
    *buffer <<= 1;

    /* STEP3. Inserts a new LSB */
    if(_RECEIVED_DATA_)
        *buffer += 1;
}

unsigned int check_crc32()
{

}







