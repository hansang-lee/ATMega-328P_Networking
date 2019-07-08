#pragma once
#include "init.h"

/*! \brief      Setup for the I/O ports of interrupt signals
  * \details    PB1 - Clock-Signal Output
  * \details    PD3 - Clock-Signal Input
  * \details    PB2 - Clock-Signal Output
  * \details    PD4 - Clock-Signal Input
  * \return     void */
void io_setup()
{
	// Sets output ports of LEDs
	//DDRB |= (1 << DDB5);
	//DDRB |= (1 << DDB4);

	// Sets output and input ports for Clock-Signal
	DDRB |= (1 << DDB1); // Output
	DDRD &= ~(1 << DDD3); // Input

	// Sets output and input ports for Data-Signal
	DDRB |= (1 << DDB2); // Output
	DDRD &= ~(1 << DDD4); // Input
}


/*! \brief  Setup for registers of interrupts
  * \return void */
void interrupt_setup()
{
	TIMSK0 |= (1 << OCIE0A);
	TIMSK0 |= (1 << OCIE0B);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02);
    OCR0A = 0x2f;
}


/*! \brief  Setup for registers of pin-change interrupts 
  * \return void */
void pin_change_setup()
{
	PCMSK2 |= (1 << PCINT19);
	PCICR |= (1 << PCIE2);
}
