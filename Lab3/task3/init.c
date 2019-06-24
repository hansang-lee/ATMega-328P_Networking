#pragma once
#include "init.h"

void io_init()
{
	/* LED */
	DDRB |= (1 << DDB5);
	DDRB |= (1 << DDB4);

	/* Clock Signal */
	DDRB |= (1 << DDB1); // Output
	DDRD &= ~(1 << DDD3); // Input

	/* Sending Data */
	DDRB |= (1 << DDB2); // Output(Data Send)
	DDRD &= ~(1 << DDD4); // Input(Data Receive)
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
