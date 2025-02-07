/*!
  * \mainpage   RaspNet-Networking through ATMega Microcontroller
  * \author     Hansang Lee
  * \date       8 July 2019 */

#include "init.c"
#include "interrupt.c"

int main()
{
    /// Initializes Frame Packets
    rFrame = &_rFrame;
    tFrame = &_tFrame;
    myFrame = &_myFrame;
    sFrame = &_sFrame;

    /// Initializes flag variables
    tFlag = FLAG_IDLE;
    rFlag = FLAG_DETECTING_PREAMBLE;
    pFlag = PRIORITY_IDLE;

    /// Initializes Packets
    clearFrame(tFrame);
    clearFrame(myFrame);
    clearFrame(rFrame);

    /// Pre-defined Packet without Destination-Address
    myFrame->dlc[0]     = 0x06;
    myFrame->payload[1] = 0x0f;
    myFrame->payload[2] = 0x74;
    myFrame->payload[3] = 0x65;
    myFrame->payload[4] = 0x73;
    myFrame->payload[5] = 0x74;

    /// Initializes Interrupts
	io_setup();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

    /// User-Input
    uint8_t input = 0;
    for(;;)
	{
        /// Sets Input Mode by pressing alphabet 'a'
        if(uart_receive() == 'a')
        {
            myFrame->payload[0] = 0x00;
            printMsg("DESTINATION : ", 14);
            
            while(1)
            {
                input = uart_receive();
                uart_transmit((char)input);

                /// Finalizes the Destination-Address through user-input by pressing 'Enter'
                if(input == 0x0d) 
                    break;

                else 
                {
                    /// Initializes the written numbers by pressing 'Backspace'
                    if((input == 0x7f) || (input == 0x08))
					{
						myFrame->payload[0] = 0x00;
						uart_transmit('\r'); printMsg("DESTINATION :       ", 20);
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
