#pragma once
#include <stdlib.h>

// Turns on LEDs : PB4 and PB5
//#define LED_A_TOGGLE()              (PORTB ^= (1 << PB5))
//#define LED_B_TOGGLE()              (PORTB ^= (1 << PB4))

/// Sends Logical 1 Data-Signal through "PB2 Pin"
#define SEND_DATA_ONE()             (PORTB |= (1 << PB2))

/// Sends Logical 0 Data-Signal through "PB2 Pin"
#define SEND_DATA_ZERO()            (PORTB &= ~(1 << PB2))

/// Receives Data-Signal through "PD4 Pin"
#define RECEIVED_DATA()             (PIND & (1 << PD4))

/// Sends Clock-Signal for Pin-Change Interrupt through "PB1 Pin"
#define PIN_CHANGE()                (PORTB ^= (1 << PB1))

/// Interrupt Interval - per millisecond
#define INTERRUPT_PERIOD            1

#define PRIORITY_IDLE               50
#define PRIORITY_RELAY              51
#define PRIORITY_SEND               52
#define PRIORITY_LOCK               53

#define FLAG_IDLE                   100
#define FLAG_WAITING                101
#define FLAG_SENDING_PREAMBLE       102
#define FLAG_SENDING_DESTINATION    103
#define FLAG_SENDING_SOURCE         104
#define FLAG_SENDING_CRC            105
#define FLAG_SENDING_DLC            106
#define FLAG_SENDING_PAYLOAD        107

#define FLAG_DETECTING_PREAMBLE     150
#define FLAG_RECEIVING_DESTINATION  151
#define FLAG_RECEIVING_SOURCE       152
#define FLAG_RECEIVING_CRC          153
#define FLAG_RECEIVING_DLC          154
#define FLAG_RECEIVING_PAYLOAD      155
#define FLAG_CHECKING_CRC           156
#define FLAG_LAYER_3                157

/// Packet Format
typedef struct
{
    uint8_t crc[4];
    uint8_t dlc[1];
    uint8_t payload[251];
} frame_t;

const uint8_t _polynomial[5] = { 0x82, 0x60, 0x8e, 0xdb, 0x80 };
const uint8_t _preamble[1] = { 0x7e };

volatile uint32_t timerA = (INTERRUPT_PERIOD/2);
volatile uint32_t timerB = 0;

volatile uint32_t pFlag = FLAG_IDLE;
volatile uint32_t tFlag = FLAG_IDLE;
volatile uint32_t rFlag = FLAG_DETECTING_PREAMBLE;

volatile uint32_t tCounter = 0;
volatile uint32_t rCounter = 0;

uint8_t rQueue[1] = { 0 };

frame_t* rFrame; frame_t _rFrame;
frame_t* tFrame; frame_t _tFrame;
frame_t* myFrame; frame_t _myFrame;
frame_t* sFrame; frame_t _sFrame;
