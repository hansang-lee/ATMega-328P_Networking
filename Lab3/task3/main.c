#include "io.c"
#include "interrupt.c"

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
    myFrame->dlc[0]     = 0x06;
    myFrame->payload[1] = 0x0f;
    myFrame->payload[2] = 0x74;
    myFrame->payload[3] = 0x65;
    myFrame->payload[4] = 0x73;
    myFrame->payload[5] = 0x74;

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
                // Test Transmit : To Next
                myFrame->payload[0] = NEXT_ID;
                clearBuffer(myFrame->crc, 32);
                makeCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial, GENERATE);
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;

            case 'v':
                // Test Transmit : To Someone
                myFrame->payload[0] = OTHER_ID;
                clearBuffer(myFrame->crc, 32);
                makeCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial, GENERATE);
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;

            case 'b':
                // Test Transmit : BroadCast
                myFrame->payload[0] = BROADCAST_ID;
                clearBuffer(myFrame->crc, 32);
                makeCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial, GENERATE);
                while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
                pFlag = PRIORITY_SEND;
                *tFrame = *myFrame;
                tFlag = FLAG_SENDING_PREAMBLE;
                break;
        }
        _delay_ms(INTERRUPT_PERIOD);
	}
}
