#pragma once

#define GENERATE    0
#define CHECK       1

void printMsg(const char* msg, const uint8_t length);
void printFrame(frame_t* frame);
uint8_t readBit(const uint8_t* bitstring, const uint32_t pos);
void writeBit(uint8_t* bitstring, const uint32_t pos, const uint8_t data);
uint8_t receiveData();
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data);
void printBit(const uint8_t* buffer, const uint32_t from, const uint32_t to);
uint16_t checkPreamble(const uint8_t preambleBuffer, const uint8_t _preamble);
uint8_t makeCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* pln, const uint8_t flag);
void clearBuffer(uint8_t* bitstring, uint32_t bit_size);
void clearFrame(frame_t* frame);
