#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NO_ERROR        0
#define ERROR_OCCUR     1
#define TRUE            0
#define FALSE           1

#define PREAMBLE        0x7e        // 8 Bits
#define POLYNOMIAL      0b100000100110000010001110110110111
//#define DIVIDER         0x104C11DB7  // 33 Bits
//#define MESSAGE         0b11010011101100 //0xffff      // 8 Bits

/******************************************************/
/* Data Frame */
/******************************************************/
typedef struct data_frame_t
{
    uint16_t crc32;     // 0-31
    uint8_t size;       // 32-39
    uint8_t* payload;   // 40-2048
} frame_t;

/******************************************************/
/* Receiver Functions */
/******************************************************/
uint16_t read_bit(const uint8_t bitstring, const uint16_t pos)
{
    if((bitstring & (0b10000000 >> pos)))
        return 1;
    else
        return 0;
}

void update_preamble_buffer(uint8_t* buffer)
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

void update_crc32_buffer(uint32_t* buffer)
{
    /* STEP1. Discards the MSB */
    *buffer &= 0x7fffffff;

    /* STEP2. Left Shift */
    *buffer <<= 1;

    /* STEP3. Inserts a new LSB */
    if(_RECEIVED_DATA_)
        *buffer += 1;
}

uint16_t check_preamble(const uint8_t preambleBuffer)
{
    if((preambleBuffer ^ PREAMBLE) == 0)
        return TRUE;
    return FALSE;
}

// check_crc32

void print_preamble_buffer(const uint8_t buffer)
{
    for(int i=0; i<8; i++)
    {
        if(buffer & (0b10000000 >> i))
            uart_transmit('1');
        else
            uart_transmit('0');
    }
}

void print_crc32_buffer(const uint32_t buffer)
{
    for(int i=0; i<32; i++)
    {
        if(buffer & (0x80000000 >> i ))
            uart_transmit('1');
        else
            uart_transmit('0');
    }
}

void print_msg(const uint8_t* msg, const uint16_t length)
{
    for(int i=0; i<length; i++)
    {
        uart_transmit(msg[i]);
    }
}






