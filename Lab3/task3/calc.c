#pragma once
#include "uart.h"
#include "calc.h"

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
    if(data==1) {bitstring[(pos/8)] |= ((0b10000000) >> (pos%8));}
    else
    {
        int tmp=1;
        for(int i=0; i<(7-(pos%8)); i++) {tmp *= 2;}
        bitstring[(pos/8)] &= (0b11111111 - tmp);
    }
}

/* Returns the current data signal */
uint8_t receiveData()
{
    if((PIND & (1 << PD4))) {return 0x01;}
    else {return 0x00;}
}

/* Updates LSB from a 8-bits-string */
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data)
{
    int tmp = (pos/8);
    
    buffer[tmp] &= 0b01111111;
    buffer[tmp] <<= 1;
    buffer[tmp] += data;
}

/* Prints the 8-bits-string */
void printBit(const uint8_t* buffer, const uint32_t from, const uint32_t to)
{
    for(int i=from; i<to; i++)
    {
        if((i%8)==0) {uart_transmit(' ');}
        if(buffer[(i/8)] & (0b10000000 >> (i%8))) {uart_transmit('1');}
        else {uart_transmit('0');}
    }
}

/* Check the 8-bits-data is equivalent to the Preamble */
uint16_t checkPreamble(const uint8_t preambleBuffer, const uint8_t _preamble)
{
    if((preambleBuffer ^ _preamble) == 0)
        return 1;
    return 0;
}

/* Right-Shift Byte by Byte */
void rightShift(uint8_t* bitstring, uint32_t size, uint8_t times)
{
    for(int t=0; t<times; t++)
    {
        for(int i=((size/8)-1+t); i>=0; i--)
        {
            if((251>(i+1)))
            {
                bitstring[i+1] = 0x00;
                bitstring[i+1] = bitstring[i];
            }
        }
        bitstring[0] = 0x00;
    }
}

/* Generates CRC from source and copies the result to destination */
void generateCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* pln)
{
    /* This payload will be XOR with polynomial */
    uint32_t payload_size = (src_size + 32);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    /* Copies the payload to the temporary variable */
    for(int i=0; i<(src_size/8); i++) {payload[i] = src[i];}
    for(int i=(src_size/8); i<(payload_size/8); i++) {payload[i] = 0x00;}

    /* CRC Calculation */
    uint32_t iterator = 0;
    while(iterator < src_size)
    {
        /* Payload MSB is 0 : Left-Shift */
        if(!(readBit(payload, 0)))
        {
            /* Every Element does Left-Shift byte-by-byte */
            for(int i=0; i<(payload_size/8); i++)
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
            for(int i=0; i<33; i++)
            {
                writeBit(payload, i, (readBit(payload,i)^(readBit(pln,i))));
            }
        }
    }

    /* Copies the generated CRC to the destination */
    for(int i=0; i<(32/8); i++)
    {
        crc[i] = payload[i];
    }

    free(payload);
}

int8_t checkCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* pln)
{
    /* This payload will be XOR with polynomial */
    uint32_t payload_size = (src_size + 32);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    /* Copies the payload to the temporary variable */
    for(int i=0; i<(src_size/8); i++) {payload[i] = src[i];}
    for(int i=(src_size/8); i<(payload_size/8); i++) {payload[i] = crc[i-(src_size/8)];}

    /* CRC Calculation */
    uint32_t iterator = 0;
    while(iterator < src_size)
    {
        /* Payload MSB is 0 : Left-Shift */
        if(!(readBit(payload, 0)))
        {
            /* Every Element does Left-Shift byte-by-byte */
            for(int i=0; i<(payload_size/8); i++)
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
            for(int i=0; i<33; i++)
            {
                writeBit(payload, i, (readBit(payload,i)^(readBit(pln,i))));
            }
        }
    }

    /* Copies the generated CRC to the destination */
    int result=0;
    for(int i=0; i<4; i++)
    {
        crc[i] = payload[i];
        result += payload[i];
    }

    free(payload);

    if(result == 0)
        return 1;
    else
        return 0;
}

void bufferClear(uint8_t* bitstring, uint32_t bit_size)
{
    for(int i=0; i<(bit_size/8); i++)
    {
        bitstring[i] = 0x00;
    }
}

void fillBuffer(frame_t* dst, frame_t* src, const uint8_t* polynomial)
{
    bufferClear(dst->crc, 32);
    *dst = *src;
    dst->dlc[0] += 0x02;
    generateCrc(dst->crc, dst->payload, dst->dlc[0], polynomial);
}
