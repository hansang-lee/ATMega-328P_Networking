#include "io.c"
#include "interrupt.c"

#define ENTER   0x0d
#define WRITE   'w'
#define READ    'r'
#define QUIT    'q'

int main()
{
    /* Frame Packets Initializing */
    rFrame = &_rFrame;
    tFrame = &_tFrame;
    myFrame = &_myFrame;
    sFrame = &_sFrame;

    /* Flags Initializing */
    tFlag = FLAG_IDLE;
    rFlag = FLAG_DETECTING_PREAMBLE;
    pFlag = PRIORITY_READ;

    /* Already Filled Buffer */
    myFrame->crc[0] = 0x00;
    myFrame->crc[1] = 0x00;
    myFrame->crc[2] = 0x00;
    myFrame->crc[3] = 0x00;
    myFrame->dlc[0] = 0x30;     // 0011 0000
    myFrame->payload[0] = 0x00; // Dst : 0000 0000
    myFrame->payload[1] = 0x0f; // Src : 0000 1111
    myFrame->payload[2] = 0x74; // 0111 0100
    myFrame->payload[3] = 0x65; // 0110 0101
    myFrame->payload[4] = 0x73; // 0111 0011
    myFrame->payload[5] = 0x74; // 0111 0100

    /* Interrupts Initializing */
	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

    /* User Input */
    uint8_t typed = '\0';
	for (;;)
	{
        typed = uart_receive();
        switch(typed)
        {
            case ENTER:
                fillBuffer(tFrame, myFrame, _polynomial);
                tFlag = FLAG_SENDING_PREAMBLE;
                _delay_ms(INTERRUPT_PERIOD);
                break;

            default:
                break;
        }
	}
}
