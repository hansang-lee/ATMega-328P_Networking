#include <stdlib.h>

/* Turns on LEDs : PB4 and PB5 */
#define LED_A_TOGGLE()              (PORTB ^= (1 << PB5))
#define LED_B_TOGGLE()              (PORTB ^= (1 << PB4))

/* Send Data Signal : PB2 > PD4 */
#define SEND_DATA_ONE()             (PORTB |= (1 << PB2))
#define SEND_DATA_ZERO()            (PORTB &= ~(1 << PB2))

/* Send Clock Signal for Pin-Change : PB1 > PD3 */
#define PIN_CHANGE()                (PORTB ^= (1 << PB1))
#define RECEIVED_DATA()             (PIND & (1 << PD4))

/* How often run Interrups? : 1000 = 1s */
#define INTERRUPT_PERIOD            10

/* Priority */
#define PRIORITY_SEND_MESSAGE       50
#define PRIORITY_RELAY_MESSAGE      51
#define PRIORITY_IDLE               52

/* Flags at Transmitter Part */
#define FLAG_IDLE                   100
#define FLAG_WAITING                101
#define FLAG_GENERATING_CRC         102
#define FLAG_SENDING_PREAMBLE       103
#define FLAG_SENDING_DESTINATION    104
#define FLAG_SENDING_SOURCE         105
#define FLAG_SENDING_CRC            106
#define FLAG_SENDING_DLC            107
#define FLAG_SENDING_PAYLOAD        108

/* Flags of Receiver Part */
#define FLAG_DETECTING_PREAMBLE     150
#define FLAG_RECEIVING_DESTINATION  151
#define FLAG_RECEIVING_SOURCE       152
#define FLAG_RECEIVING_CRC          153
#define FLAG_RECEIVING_DLC          154
#define FLAG_RECEIVING_PAYLOAD      155
#define FLAG_CHECKING_CRC           156
#define FLAG_LAYER_3                157

/* Layer 3 */
#define RETURNED_MESSGE             200
#define BROADCAST_MESSAGE           201
#define MESSAGE_TO_ME               202
#define MESSAGE_TO_ANOTHER          203

/* Packet Format */
typedef struct
{
    uint8_t crc[4];
    uint8_t dlc[1];
    uint8_t payload[251];
} frame_t;

/* Fixed Variables for Checking */
const uint8_t _polynomial[5]  = { 0x82, 0x60, 0x8e, 0xdb, 0x80 };
const uint8_t _preamble[1]    = { 0x7e };

/* Timers for Interrupts */
volatile uint32_t timerA    = (INTERRUPT_PERIOD/2);
volatile uint32_t timerB    = 0;

/* Flags */
volatile uint32_t pFlag     = FLAG_IDLE;
volatile uint32_t tFlag     = FLAG_IDLE;
volatile uint32_t rFlag     = FLAG_DETECTING_PREAMBLE;

/* Counters */
volatile uint32_t tCounter  = 0;
volatile uint32_t rCounter  = 0;

/* RECEIVER */
uint8_t rQueue[1]           = { 0 };

/* Receiver's- and Transmitter's- Packet */
frame_t* rFrame; frame_t _rFrame;
frame_t* tFrame; frame_t _tFrame;
frame_t* myFrame; frame_t _myFrame;
frame_t* sFrame; frame_t _sFrame;

/* Messages to print on Minicom */
const uint8_t logMsg_preamble[18]  = "Preamble Detected";
const uint8_t logMsg_dst[20]       = "Destination Received";
const uint8_t logMsg_src[15]       = "Source Received";
const uint8_t logMsg_crc[13]       = "CRC Received";
const uint8_t logMsg_crc_true[14]  = "CRC is correct";
const uint8_t logMsg_crc_false[16] = "CRC is incorrect";
const uint8_t logMsg_dlc[13]       = "DLC Received";
const uint8_t logMsg_payload[17]   = "Payload Received";

/* Implementations */
void interrupt_setup()
{
	TIMSK0 |= (1 << OCIE0A);
	TIMSK0 |= (1 << OCIE0B);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02);
    OCR0A = 0x2f;
}

void pin_change_setup()
{
	PCMSK2 |= (1 << PCINT19);
	PCICR |= (1 << PCIE2);
}

void clearFrame(frame_t* frame)
{
    for(int i=0; i<4; i++) {frame->crc[i] = 0x00;}
    frame->dlc[0] = 0x00;
    for(int i=0; i<251; i++) {frame->payload[i] = 0x00;}
}
