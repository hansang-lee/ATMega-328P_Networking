#include "io.c"
#include "interrupt.c"

#define INPUT   'a'
#define ENTER   0x0d

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
    uint8_t input = 0;
    for(;;)
	{
        // Press 'a' : Set Input Mode
        if(uart_receive() == INPUT)
        {
            myFrame->payload[0] = 0x00;
            printMsg("DESTINATION : ", 14);
            
            // Press Numbers : Type Address
            while(1)
            {
                input = uart_receive();
                uart_transmit((char)input);
                if(input == ENTER) break;
                else 
                {
                    // Press 'Backspace' : Initialize Input Mode
                    if((input == 0x7f) || (input == 0x08))
					{
						myFrame->payload[0] = 0x00;
						uart_transmit('\r'); printMsg("DESTINATION :    ", 17);
						uart_transmit('\r'); printMsg("DESTINATION : ", 14);
					}
                    else
                    {
						myFrame->payload[0] = ((myFrame->payload[0] * 10) + (((uint8_t)input)-48));
                    }
                }
            }
			clearBuffer(myFrame->crc, 32);
			makeCrc(myFrame->crc, myFrame->payload, myFrame->dlc[0], _polynomial, GENERATE);
			while(((pFlag == PRIORITY_LOCK) || (pFlag == PRIORITY_SEND) || (pFlag == PRIORITY_RELAY)));
			pFlag = PRIORITY_SEND;
			*tFrame = *myFrame;
			tFlag = FLAG_SENDING_PREAMBLE;
		}
        _delay_ms(INTERRUPT_PERIOD);
	}
}
