#pragma once
#include "init.h"

void io_setup()
{
	// Sets output ports of LEDs
	//DDRB |= (1 << DDB5);
	//DDRB |= (1 << DDB4);

	/// Sets output and input ports of Clock-Signal
	DDRB |= (1 << DDB1); /// Output
	DDRD &= ~(1 << DDD3); /// Input

	/// Sets output and input ports of Data-Signal
	DDRB |= (1 << DDB2); /// Output
	DDRD &= ~(1 << DDD4); /// Input
}

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
