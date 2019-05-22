#include "io.c"
#include "interrupt.c"

/********************************************************************************
 ****** TASK 3-3 : Network Layer 3 (At the different Source Code)
 *
 ****** ID : 001 - 255 (Must be unique in the network)
 *  # ID 0x0f
 *  # ID 0x00 - broadcast address
 *
 ****** Scenario
 *  # Correct ID - Send it over to Layer 4
 *  # Wrong ID - Relay it to the next node
 *  # Ring Topology
 *  # Each node has 2 nodes connections
 *
 ****** Note
 *  Q. What happen when the receiver of a message cannot be found in the network?
 *  Q. What should happen?
 *  Q. How do you realize the demanded prioritization scheme?
 *
 * 1. Payload - 2008
 * 2. receive buffer - frame format
 * 3. layer3
 ********************************************************************************/
int main()
{
    frame_init(rFrame);
    //frame_clear(&rFrame);

	io_init();
	cli();
	uart_init(MYUBRR);
	interrupt_setup();
	pin_change_setup();
	sei();

	for (;;)
	{

	}
}
