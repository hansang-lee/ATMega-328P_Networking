#include "interrupt.h"
#include "crc.h"

/* Log Messages */
//volatile const unsigned char preambleMsg[11] = "Preamble OK";

/* Data Messages */
//unsigned char preamble_buffer[1] = {0};
//volatile const unsigned char messages = 0b01111110;

/* Buffers */
//volatile unsigned char uart_buffer = {0};

/* Interrupt A - Data Transmitter */
ISR(TIMER0_COMPA_vect)
{
    static volatile unsigned int timerA = (_PERIOD_ / 2);
	static volatile unsigned int i = 0;
	timerA++;
	if (timerA > _PERIOD_)
	{
		_LED_A_TOGGLE_;
		timerA = 0;
        
        ////////////////////////////////////////////////////////
        /* DATA TRANSMIT */
        static volatile const unsigned char messages = 0b01111110;
        if(read_bit(messages, i++))
            _SEND_LOGICAL_1_;
        else
            _SEND_LOGICAL_0_;
        if(i == 8) i=0;
        ////////////////////////////////////////////////////////
	}
}

/* Interrupt B - Clock Signal */
ISR(TIMER0_COMPB_vect)
{
	static volatile unsigned int timerB = 0;
	timerB++;
	if (timerB > _PERIOD_)
	{
		_LED_B_TOGGLE_;
		timerB = 0;
		
        ////////////////////////////////////////////////////////
		/* CLOCK SIGNAL TRANSMIT */
		_PIN_CHANGE_;
        ////////////////////////////////////////////////////////
	}
}

/* Pin Change Interrupt */
volatile unsigned int status_flag = FLAG_DETECTING PREAMBLE;
ISR(PCINT2_vect)
{
    ////////////////////////////////////////////////////////
    /* PREAMBLE & CRC CHECKING */
    
    ////////////////////////////////////////////////////////
    /* STEP1. DETECTING PREAMBLE
     * Constantly receives messages for checking preamble */
    static unsigned char preamble_buffer[1] = {0};
    static volatile const unsigned char preambleReadyMsg[17] = "Detected Preamble";
    if(status_flag == FLAG_DETECTING_PREAMBLE)
    {
        update_preamble_buffer(preamble_buffer);
        print_preamble_buffer(*preamble_buffer);
        
        if(check_preamble(*preamble_buffer) == TRUE)
        {
            uart_changeLine();
            for(int i=0; i<17; i++)
                uart_transmit(preambleReadyMsg[i]);
            uart_changeLine();

            status_flag = FLAG_CHECKING_CRC;
        }
    }

    ////////////////////////////////////////////////////////
    /* STEP2. CHECKING CRC
     * When detected PREAMBLE */
    static unsigned long int crc32_buffer[1] = {0};
    static unsigned int buffer_filling_counter = 0;
    //volatile const unsigned char crc32NoErrorMsg[16] = "CRC Has No Error";
    if(status_flag == FLAG_CHECKING_CRC)
    {
        /* Receiving CRC data */
        if(buffer_filling_counter < 32)
        {
            update_crc32_buffer(crc32_buffer);
            counter_for_crc++;
        }

        /* Checking CRC */
        else
        {
            /* Message + #32 Zeros */
            unsigned long long int input &= ((*crc32_buffer) << 32);
            unsigned long long int divisor &= (0x0000000104c11db7 << 31)
            unsigned long long int remainder = 0;

            /* Processing */
            for(;;)
            {
                // XOR
                remainder = (input ^ divisor);


            }
        }
    }
    ////////////////////////////////////////////////////////
}





