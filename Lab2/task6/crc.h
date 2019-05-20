#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NO_ERROR        0
#define ERROR_OCCUR     1
#define TRUE            0
#define FALSE           1
#define PREAMBLE        0x7e

/* Data Frame */
typedef struct data_frame_t
{
    uint32_t crc32;
    uint8_t size;
    uint8_t* payload;
} frame_t;

/* Print log-messages */
void printMsg(const uint8_t* msg, const uint8_t length)
{
    for(int i=0; i<length; i++)
    {
        uart_transmit(msg[i]);
    }
}

/* Read a specific bit from a bitstring */
uint8_t readBit(const uint8_t* bitstring, const uint32_t pos)
{
    if((bitstring[(pos/8)] & (0b10000000 >> (pos%8))))
        return 1;
    else
        return 0;
}

/* Write a bit on a specific position of a bitstring */
void writeBit(uint8_t* bitstring, const uint32_t pos, const uint8_t data)
{
    if(data==1)
        bitstring[(pos/8)] |= ((0b10000000) >> (pos%8));
    else
    {
        int tmp=1;
        for(int i=0; i<(7-(pos%8)); i++)
            tmp *= 2;
        bitstring[(pos/8)] &= (0b11111111 - tmp);
    }
}

/* Returns the current data signal */
uint8_t receiveData()
{
    if((PIND & (1 << PD4)))
        return 0x01;
    else
        return 0x00;
}

/* Updates LSB from a 8-bits-string */
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data)
{
    int i = (pos/8);

    buffer[i] &= 0b01111111;
    buffer[i] <<= 1;
    buffer[i] += data;
}

/* Prints the 8-bits-string */
void printBit(const uint8_t* buffer, const uint32_t bits)
{
    for(int i=0; i<bits; i++)
    {
        if((i%8)==0)
            uart_transmit(' ');

        if(buffer[(i/8)] & (0b10000000 >> (i%8)))
            uart_transmit('1');
        else
            uart_transmit('0');
    }
}

/* Check the 8-bits-data is equivalent to the Preamble */
uint16_t checkPreamble(const uint8_t preambleBuffer)
{
    if((preambleBuffer ^ PREAMBLE) == 0)
        return TRUE;
    return FALSE;
}

/* Generates CRC from source and copies the result to destination */
void generateCrc(uint8_t* dst, const uint8_t* src, const uint32_t src_size, const uint8_t* pln)
{
    /* This payload will be XOR with polynomial */
    uint32_t payload_size = (src_size + SIZE_OF_CRC);
    uint32_t payload_byteSize = (payload_size/8);
    uint32_t src_byteSize = (src_size/8);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    for(int i=0; i<src_byteSize; i++)
        payload[i] = src[i];
    for(int i=src_byteSize; i<payload_byteSize; i++)
        payload[i] = 0x00;

    /* CRC Calculation */
    uint32_t iterator = 0;
    uint32_t loop_upperbound = src_size;
    while(iterator < loop_upperbound)
    {
        /* Payload MSB is 0 : Left-Shift */
        if(!(readBit(payload, 0)))
        {
            /* Every Element does Left-Shift byte-by-byte */
            for(int i=0; i<payload_byteSize; i++)
            {
                if(readBit(&payload[i], 0))
                {
                    (!((i-1)<0))?(payload[i-1]+=0x01):(0);
                }
                payload[i] &= 0b01111111;
                payload[i] <<= 1;
            }
            iterator++;
        }

        /* Payload MSB is 1 : XOR */
        else
        {
            for(int i=0; i<SIZE_OF_POLYNOMIAL; i++)
            {
                writeBit(
                        payload, 
                        i, 
                        (readBit(payload,i)^(readBit(pln,i))));
            }
        }
    }

    /* Debugging */
    //for(int i=0; i<SIZE_OF_CRC; i++)
    //{
    //    if((i%8)==0) uart_transmit(' ');
    //    if(readBit(payload, i)) uart_transmit('1');
    //    else uart_transmit('0');
    //}
    //uart_changeLine();

    /* Copies the generated CRC to the destination */
    for(int i=0; i<(SIZE_OF_CRC/8); i++)
    {
        dst[i] = payload[i];
    }
}

int8_t checkCrc(uint8_t* dst, const uint8_t* src, const uint32_t src_size, const uint8_t* pln, const uint8_t* crc)
{
    /* This payload will be XOR with polynomial */
    uint32_t payload_size = (src_size + SIZE_OF_CRC);
    uint32_t payload_byteSize = (payload_size/8);
    uint32_t src_byteSize = (src_size/8);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    for(int i=0; i<src_byteSize; i++)
        payload[i] = src[i];
    for(int i=src_byteSize; i<payload_byteSize; i++)
        payload[i] = crc[i];

    /* CRC Calculation */
    uint32_t iterator = 0;
    uint32_t loop_upperbound = src_size;
    while(iterator < loop_upperbound)
    {
        /* Payload MSB is 0 : Left-Shift */
        if(!(readBit(payload, 0)))
        {
            /* Every Element does Left-Shift byte-by-byte */
            for(int i=0; i<payload_byteSize; i++)
            {
                if(readBit(&payload[i], 0))
                {
                    (!((i-1)<0))?(payload[i-1]+=0x01):(0);
                }
                payload[i] &= 0b01111111;
                payload[i] <<= 1;
            }
            iterator++;
        }

        /* Payload MSB is 1 : XOR */
        else
        {
            for(int i=0; i<SIZE_OF_POLYNOMIAL; i++)
            {
                writeBit(
                        payload, 
                        i, 
                        (readBit(payload,i)^(readBit(pln,i))));
            }
        }
    }

    /* Debugging */
    //for(int i=0; i<SIZE_OF_CRC; i++)
    //{
    //    if((i%8)==0) uart_transmit(' ');
    //    if(readBit(payload, i)) uart_transmit('1');
    //    else uart_transmit('0');
    //}
    //uart_changeLine();

    /* Copies the generated CRC to the destination */
    for(int i=0; i<(SIZE_OF_CRC/8); i++)
    {
        dst[i] = payload[i];
    }
    
    return TRUE;
}













