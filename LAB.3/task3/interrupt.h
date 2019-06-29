#pragma once
#include <stdlib.h>

// Turns on LEDs : PB4 and PB5
//#define LED_A_TOGGLE()              (PORTB ^= (1 << PB5))
//#define LED_B_TOGGLE()              (PORTB ^= (1 << PB4))

/// Send Data Signal : PB2 Data-Out
#define SEND_DATA_ONE()             (PORTB |= (1 << PB2))
#define SEND_DATA_ZERO()            (PORTB &= ~(1 << PB2))

/// Send Data Signal : PD4 Data-In
#define RECEIVED_DATA()             (PIND & (1 << PD4))

/// Send Clock Signal for Pin-Change : PB1 Clock-Out
/// Send Clock Signal for Pin-Change : PD3 Clock-Input
#define PIN_CHANGE()                (PORTB ^= (1 << PB1))

/// How often run Interrupts? : 1ms
#define INTERRUPT_PERIOD            1

/// Flags that will be used for relaying packets
#define PRIORITY_IDLE               50
#define PRIORITY_RELAY              51
#define PRIORITY_SEND               52
#define PRIORITY_LOCK               53

/// Flags that will be used at the transmitter interrupt
#define FLAG_IDLE                   100
#define FLAG_WAITING                101
#define FLAG_SENDING_PREAMBLE       102
#define FLAG_SENDING_DESTINATION    103
#define FLAG_SENDING_SOURCE         104
#define FLAG_SENDING_CRC            105
#define FLAG_SENDING_DLC            106
#define FLAG_SENDING_PAYLOAD        107

/// Flags that will be used at the receiver interrupt
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

/// Pre-defined variables
const uint8_t _polynomial[5] = { 0x82, 0x60, 0x8e, 0xdb, 0x80 };
const uint8_t _preamble[1] = { 0x7e };

/// Timer variables for the periodical running of interrupts
volatile uint32_t timerA = (INTERRUPT_PERIOD/2);
volatile uint32_t timerB = 0;

/// Flag variables
volatile uint32_t pFlag = FLAG_IDLE;
volatile uint32_t tFlag = FLAG_IDLE;
volatile uint32_t rFlag = FLAG_DETECTING_PREAMBLE;

/// Counter variables for transmitting and receiving packets
volatile uint32_t tCounter = 0;
volatile uint32_t rCounter = 0;

/// A queue variable for checking the preamble
uint8_t rQueue[1] = { 0 };

/// Packets
frame_t* rFrame; frame_t _rFrame;       /// A Packet for receiving 
frame_t* tFrame; frame_t _tFrame;       /// A Packet for transmitting
frame_t* myFrame; frame_t _myFrame;     /// A Packet for user-input
frame_t* sFrame; frame_t _sFrame;       /// A Packet for storing
