#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define NUM_PINS 12
#define SPEED 50

void init();
void showAnimation(int[]);

int main()
{
    int pins[NUM_PINS] = {PB5, PB4, PB3, PB2, PB1, PB0, PD7, PD6, PD5, PD4, PD3, PD2};

    init();
    showAnimation(pins);
}

void showAnimation(int pins[])
{
    int i;
    for (;;)
    {
        // Port-B High: PB5-PB4-PB3-PB2-PB1-PB0
        i = 0;
        while ((-1 < i) && (i < 6))
        {
            PORTB |= (1 << pins[i]);
            _delay_ms(SPEED);
            i++;
        }

        // Port-D High: PD7-PD6-PD5-PD4-PD3-PD2 
        while ((5 < i) && (i < NUM_PINS))
        {
            PORTD |= (1 << pins[i]);
            _delay_ms(SPEED);
            i++;
        }

        // PORT-D Low: PD2-PD3-PD4-PD5-PD6-PD7
        i = NUM_PINS - 1;
        while ((NUM_PINS > i) && (i > 5))
        {
            PORTD &= ~(1 << pins[i]);
            _delay_ms(SPEED);
            i--;
        }

        // PORT-D Low: PB0-PB1-PB2-PB3-PB4-PB5
        while ((6 > i) && (i > -1))
        {
            PORTB &= ~(1 << pins[i]);
            _delay_ms(SPEED);
            i--;
        }
    }
}

void init()
{
    DDRB = 0xff;
    DDRD = 0xff;
}
