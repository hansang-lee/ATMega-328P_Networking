#pragma once
#include <avr/io.h>
#define F_CPU 12000000UL
#define BAUD 19200UL
#define MYUBRR (F_CPU/(16*BAUD)-1)
#include <util/setbaud.h>
#include <util/delay.h>


/*! \brief  Setup for the uart serial communication
  * \param  ubrr
  * \return void */
void uart_init(unsigned long ubrr);


/*! \brief  Prints any character on Minicom
  * \param  data - A character to be printed out on Minicom
  * \return void */
void uart_transmit(unsigned char data);


/*! \brief  Reads user-typed input data
  * \return void */
unsigned char uart_receive();


/*! \brief  Changes to the new line
  * \return void */
void uart_changeLine();
