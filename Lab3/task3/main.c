#include "io.c"
#include "interrupt.c"

#define ENTER 0x0d

int main()
{
    // FRAME
    rFrame = &_rFrame;
    tFrame = &_tFrame;
    myFrame = &_myFrame;
    sFrame = &_sFrame;

    tFlag = FLAG_IDLE;
    rFlag = FLAG_DETECTING_PREAMBLE;
    pFlag = PRIORITY_IDLE;

    // Filled Buffer
    tFrame->crc[0] = 0x00;
    tFrame->crc[1] = 0x00;
    tFrame->crc[2] = 0x00;
    tFrame->crc[3] = 0x00;

    tFrame->dlc[0] = 0x30; // 0011 0000

    tFrame->payload[0] = 0x09; // Dst : 0000 1001
    tFrame->payload[1] = 0x0f; // Src : 0000 1111
    tFrame->payload[2] = 0x74; // 0111 0100
    tFrame->payload[3] = 0x65; // 0110 0101
    tFrame->payload[4] = 0x73; // 0111 0011
    tFrame->payload[5] = 0x74; // 0111 0100

    // INTERRUPT
	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

    // INPUT
	for (;;)
	{
        if(uart_receive() == ENTER)
        {
            tFlag = FLAG_GENERATING_CRC;
        }
        _delay_ms(INTERRUPT_PERIOD);
	}
}
