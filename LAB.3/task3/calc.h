#pragma once
#define GENERATE    0
#define CHECK       1

/// Prints a character string on Minicom
void printMsg(const char* msg, const uint8_t length);

/// Reads a specific bit from a character string
uint8_t readBit(const uint8_t* bitstring, const uint32_t pos);

/// Writes a bit on a specific position of a bit string
void writeBit(uint8_t* bitstring, const uint32_t pos, const uint8_t data);

/// Updates Least-Significant-Bit from a 8 bits bit string
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data);

/// Prints bits from a bit string
void printBit(const uint8_t* buffer, const uint32_t from, const uint32_t to);

/// Checks whether the 8 bits string is equal to the pre-defined preamble
uint8_t checkPreamble(const uint8_t preambleBuffer, const uint8_t _preamble);

/// Implements the 32-CRC Calculation
uint8_t makeCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* pln, const uint8_t flag);

/// Clears the 8 bits string buffer with 0
void clearBuffer(uint8_t* bitstring, const uint32_t bit_size);

/// Clears the packet
void clearFrame(frame_t* frame);

/// Prints the packet
void printFrame(const frame_t* frame);

/// Returns the received data
uint8_t receiveData();
