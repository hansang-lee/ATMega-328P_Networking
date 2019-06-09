#include "io.c"
#include "interrupt.c"

//#define ENTER           0x0d

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
    pFlag = PRIORITY_IDLE;

    /* Pre-Filled Buffer */
    clearFrame(tFrame);
    clearFrame(myFrame);
    clearFrame(rFrame);
    myFrame->dlc[0]     = 0x06; // Payload Size : 0000 0110
    myFrame->payload[1] = 0x0f; // Source       : 0000 1111
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
	for(;;)
	{
        switch(uart_receive())
        {
            case 'n':
                myFrame->payload[0] = 0x09;
                clearBuffer(myFrame->crc, 32);
                generateCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial);                
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;

            case 'v':
                myFrame->payload[0] = 0x01;
                clearBuffer(myFrame->crc, 32);
                generateCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial);
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;

            case 'b':
                myFrame->payload[0] = 0x00;
                clearBuffer(myFrame->crc, 32);
                generateCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial);
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;
        }
        _delay_ms(INTERRUPT_PERIOD);
	}
}
