#pragma once
#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/setbaud.h>
#include <util/delay.h>

void uart_init(unsigned long);
void uart_transmit(unsigned char);
unsigned char uart_receive(void);
void uart_changeLine();
