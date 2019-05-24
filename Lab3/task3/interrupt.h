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

/* Flags at Transmitter Part */
#define FLAG_GENERATING_CRC         100
#define FLAG_SENDING_PREAMBLE       101
#define FLAG_SENDING_DESTINATION    102
#define FLAG_SENDING_SOURCE         103
#define FLAG_SENDING_CRC            104
#define FLAG_SENDING_DLC            105
#define FLAG_SENDING_PAYLOAD        106

/* Flags of Receiver Part */
#define FLAG_DETECTING_PREAMBLE     200
#define FLAG_RECEIVING_DESTINATION  201
#define FLAG_RECEIVING_SOURCE       202
#define FLAG_RECEIVING_CRC          203
#define FLAG_RECEIVING_DLC          204
#define FLAG_RECEIVING_PAYLOAD      205
#define FLAG_CHECKING_CRC           206
#define FLAG_PROCESSING_DATA        207

/* Size of Buffer */
#define SIZE_OF_PREAMBLE            8
#define SIZE_OF_CRC                 32
#define SIZE_OF_DLC                 8
#define SIZE_OF_POLYNOMIAL          33
#define SIZE_OF_PAYLOAD             2008
#define SIZE_OF_ADDRESS             8

/* Packet Format */
typedef struct
{
    uint8_t crc[(SIZE_OF_CRC/8)];          // 4
    uint8_t dlc[(SIZE_OF_DLC/8)];          // 1
    uint8_t payload[(SIZE_OF_PAYLOAD/8)];  // 2008
} frame_t;

/* Global Variables */
uint8_t _polynomial[5]  = { 0x82, 0x60, 0x8e, 0xdb, 0x80 };
uint8_t _preamble[1]    = { 0x7e };

/* TRANSMITTER */
volatile uint32_t timerA = (INTERRUPT_PERIOD/2);
volatile uint32_t tFlag = FLAG_GENERATING_CRC;
volatile uint32_t tCounter = 0;
uint8_t tDestination[(SIZE_OF_ADDRESS/8)]     = { 0x0f };
const uint8_t tSource[(SIZE_OF_ADDRESS/8)]    = { 0x0f };
uint8_t tCrcBuffer[(SIZE_OF_CRC/8)]           = { 0 };
uint8_t tDlcBuffer[(SIZE_OF_DLC/8)]           = { 0x30 };
uint8_t tPayloadBuffer[(SIZE_OF_PAYLOAD/8)]   = { 0x74, 0x65, 0x73, 0x74 };

/* RECEIVER */
volatile uint32_t timerB = 0;
volatile uint32_t rFlag = FLAG_DETECTING_PREAMBLE;
volatile uint32_t rCounter = 0;
uint8_t rQueue[(SIZE_OF_PREAMBLE/8)] = { 0 };
frame_t* rFrame;
frame_t _rFrame;

/* Messages */
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
