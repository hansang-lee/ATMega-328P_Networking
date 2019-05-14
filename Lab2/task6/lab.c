#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define NO_ERROR        0
#define ERROR_OCCUR     1

#define TRUE            0
#define FALSE           1

#define PREAMBLE        0x7e        // 8 Bits
#define DIVIDER         0b1011 //0x104C11DB7  // 33 Bits
#define MESSAGE         0b11010011101100 //0xffff      // 8 Bits

/******************************************************/
/* Data Frame */
/******************************************************/
typedef struct data_frame_t
{
    unsigned int crc32;             // 0-31
    unsigned char size;             // 32-39
    unsigned char payload[2008];    // 40-2048
} frame_t;

/******************************************************/
/* Transmitter Functions */
/******************************************************/
unsigned char* generate_crc32(unsigned char* message, unsigned char size)
{
    /* STEP0. REMAINDER = DATA << 32 */
    

    //unsigned char* tmp_buffer = (unsigned char*) malloc(size+32);

    /* STEP1. Fits the size of "REMAINDER" to "DATA" */
    
    // LOOP
    //
    //    /* STEP2 - for breaking the loop
    //     * 1- if "REMAINDER" is equal to 0
    //     * 2- if "REMAINDER" is less than degree of the POLYNOMIAL */
    //    // HERE the REMAINDER is CRC Result    
    //
    //    // LOOP
    //    //
    //    //    /* STEP3. Checks the MSB of the "DATA" -
    //    //     * if the MSB is 0, then "POLYNOMIAL" Right Shift 
    //    //     * if not 0, break the loop */
    //    //
    //
    //    //    
}

/******************************************************/
/* Receiver Functions */
/******************************************************/
void update_preamble_buffer(unsigned char* preambleBuffer)
{
    /* STEP1. Discards the MSB */
    *preambleBuffer &= 0x80;

    /* STEP2. Left Shift */
    *preambleBuffer <<= 1;

    /* STEP3. Inserts a new LSB */
    //*preambleBuffer += ((0x01) & (PIND & (1 << PD4))); 
}

void read_crc32(unsigned char* crcBuffer)
{
    /* crcBuffer = 0xABCDEFGH
     * 
     * crcBuffer[3] = 0xAB
     * crcBuffer[2] = 0xCD
     * crcBuffer[1] = 0xEF
     * crcBuffer[0] = 0xGH */

    static volatile unsigned int i = 0;
    static volatile unsigned int j = 0;

    for(int i=3; i<=0; i--)
    {
        for(int j=0; j<4; j++)
        {
            crcBuffer[i] <<= 1;
            crcBuffer[i] += ((0x01) & (PIND & (1 << PD4)));
        }
    }
}

unsigned int check_preamble(unsigned char* preambleBuffer)
{
    if(!(*preambleBuffer ^ PREAMBLE))
        return NO_ERROR;
    return ERROR_OCCUR;
}

unsigned int check_MSB(unsigned char bitstring[], unsigned char size)
{
    if((bitstring[size-1] & 0x80))
        return 1;
    return 0;
}

unsigned int check_crc32(unsigned char payload[], unsigned char size)
{
    /* STEP0. REMAINDER = DATA << 32 */
    unsigned int crc_remainder = 0;
    //unsigned char* tmp_buffer = (unsigned char*) malloc(size+32);

    /* STEP1. Fits the size of "REMAINDER" to "DATA" */
    
    // LOOP
    //
    //    /* STEP2 - for breaking the loop
    //     * 1- if "REMAINDER" is equal to 0
    //     * 2- if "REMAINDER" is less than degree of the POLYNOMIAL */
    //    // HERE the REMAINDER is CRC Result    
    //
    //    // LOOP
    //    //
    //    //    /* STEP3. Checks the MSB of the "DATA" -
    //    //     * if the MSB is 0, then "POLYNOMIAL" Right Shift 
    //    //     * if not 0, break the loop */
    //    //
    //
    //    // XOR
    //



    return crc_remainder;
}

/******************************************************/
/* Main Function */
/******************************************************/
int main()
{
    //unsigned char* preambleBuffer;
    //preambleBuffer = (unsigned char*) malloc(sizeof(unsigned char));
    //*preambleBuffer = 0b10111110;
    //*preambleBuffer &= 0x7f;            // STEP1. Discards the LSB
    //*preambleBuffer <<= 1;              // STEP2. Left Shift
    //*preambleBuffer += 0;               // STEP3. Add new value
    //printf("%d", check_preamble(preambleBuffer));
    

    unsigned char size = 0x02;
    //unsigned char* bitstring = (unsigned char*) malloc(2);
    //bitstring[0] = 0b10101010;
    //bitstring[1] = 0b01010101;

    unsigned char* bitstring = (unsigned char*) malloc(size);
    bitstring[0] = 0b01010101;
    bitstring[1] = 0b00101010;

    printf("%d", check_MSB(bitstring, size));

}
