#pragma once
#include "uart.h"
#include "calc.h"

/// Prints Messages on Minicom
void printMsg(const char* msg, const uint8_t length)
{
    for(int i=0; i<length; i++)
        uart_transmit(msg[i]);
}

/// Reads a specific bit from a bitstring
uint8_t readBit(const uint8_t* bitstring, const uint32_t pos)
{
    if((bitstring[(pos/8)] & (0b10000000 >> (pos%8))))
        return 1;
    else
        return 0;
}

/// Write a bit on a specific position of a bitstring
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

/// Updates LSB from a 8-bits-string
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data)
{
    int tmp = (pos/8);
    
    buffer[tmp] &= 0b01111111;
    buffer[tmp] <<= 1;
    buffer[tmp] += data;
}

/// Prints bits
void printBit(const uint8_t* buffer, const uint32_t from, const uint32_t to)
{
    for(int i=from; i<to; i++)
    {
        if((i>0) && ((i%8) == 0))
			uart_transmit(' ');
		
        if(buffer[(i/8)] & (0b10000000 >> (i%8)))
			uart_transmit('1');
		
        else
			uart_transmit('0');
    }
}

/// Check the 8-bits-data is equivalent to the Preamble
uint8_t checkPreamble(const uint8_t preambleBuffer, const uint8_t _preamble)
{
    if((preambleBuffer ^ _preamble) == 0)
        return 1;
	else
		return 0;
}

/** Generates CRC from source and copies the result to destination
  * 
  *
  * */
uint8_t makeCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* pln, const uint8_t flag)
{
    /// This payload will do XOR with polynomial
    uint32_t payload_size = ((src_size*8) + 32);
    uint8_t* payload = (uint8_t*) malloc(payload_size);

    /// Copies the payload to the a temporary variable
    for(int i=0; i<src_size; i++)
		payload[i] = src[i];
    for(int i=src_size; i<(payload_size/8); i++)
		payload[i] = crc[i-src_size];

    /** CRC Calculation
	  * 
	  * When the Payload MSB is equal to 0, then do Left-Shift
	  * When the Payload MSB is equal to 1, then do XOR */
    uint32_t iterator = 0;
    while(iterator < (src_size*8))
    {
        /// Payload MSB is 0 : Left-Shift
        if(!(readBit(payload, 0)))
        {
            /// Every Element does Left-Shift byte-by-byte
            for(int i=0; i<(payload_size/8); i++)
            {
                if(readBit(&payload[i], 0))
                {
                    (!((i-1)<0)) ? (payload[i-1]+=0x01) : (0);
                }
                payload[i] &= 0b01111111;
                payload[i] <<= 1;
            }
            iterator++;
        }

        /// Payload MSB is 1 : XOR
        else
        {
            for(int i=0; i<33; i++)
                writeBit(payload, i, (readBit(payload,i)^(readBit(pln,i))));
        }
    }

    uint8_t result = 0;
    for(int i=0; i<4; i++)
    {
        /// Generate Mode
        if(flag == GENERATE)
            crc[i] = payload[i];
        
        /// Check Mode
        else if(flag == CHECK)
            result += payload[i];
    }

    free(payload);

    if(result == 0)
		return 1;
    else
		return 0;
}

/// Initializes the string with 0
void clearBuffer(uint8_t* bitstring, const uint32_t bit_size)
{
    for(int i=0; i<(bit_size/8); i++)
        bitstring[i] = 0x00;
}

/// Initializes the frame with 0
void clearFrame(frame_t* frame)
{
    for(int i=0; i<4; i++)
		frame->crc[i] = 0x00;
	
    frame->dlc[0] = 0x00;
	
    for(int i=0; i<251; i++)
		frame->payload[i] = 0x00;
}

/// Prints A Frame
void printFrame(const frame_t* frame)
{
    printMsg("CRC ", 4);
    printBit(frame->crc, 0, 32); 
    uart_changeLine();

    printMsg("DLC ", 4);
    printBit(frame->dlc, 0, 8); 
    uart_changeLine();

    printMsg("DST ", 4);
    printBit(frame->payload, 0, 8);
    uart_changeLine();

    printMsg("SRC", 3);
    printBit(frame->payload, 8, 16);
    uart_changeLine();

    printMsg("PAY", 3);
    printBit(frame->payload, 16, ((frame->dlc[0])*8));
}

/// Returns the current data signal
uint8_t receiveData()
{
    if((PIND & (1 << PD4)))
		return 0x01;
    
	else
		return 0x00;
}