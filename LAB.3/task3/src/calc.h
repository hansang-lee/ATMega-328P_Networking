#pragma once
#define GENERATE    0
#define CHECK       1

/*! \brief      Prints a character string on Minicom
  * \param      msg     - Set of characters to be printed out
  * \param      length  - Number of characters
  * \return     void */
void printMsg(const char* msg, const uint8_t length);


/*! \brief      Reads a specific bit at the specified position on the bits 
  * \param      buffer  - Set of bits to be read
  * \param      pos     - Position of the bit, which counts from left to right
  * \return     unsigned 8-bits data - 1 or 0 */
uint8_t readBit(const uint8_t* buffer, const uint32_t pos);


/*! \brief      Overwrites a bit at the corresponding position with a new bit
  * \param      buffer  - Set of bits
  * \param      pos     - A specific position of a bit, which counts from left to right
  * \param      data    - 1 or 0 bit data to be over-written on the existing bit
  * \return     void */
void writeBit(uint8_t* buffer, const uint32_t pos, const uint8_t data);


/*! \brief      Updates the right most bit with a new bit, then implements left-shift
  * \param      buffer  - Set of bits
  * \param      pos     - A specific position of a bit, which counts from left to right
  * \param      data    - 1 or 0 bit data to replace the existing right most bit
  * \return     void */
void updateBit(uint8_t* buffer, const uint32_t pos, const uint8_t data);


/*! \brief      Prints the bits from the start position to end position
  * \param      buffer  - Set of bits to be printed on Minicom
  * \param      from    - Start position
  * \param      to      - End position
  * \return     void */
void printBit(const uint8_t* buffer, const uint32_t start, const uint32_t end);


/*! \brief      Checks whether the 
  * \param      buffer      - 8-bits buffer to be compared to the preamble
  * \param      preamble    - Pre-defined preamble
  * \return     unsigned 8-bits data - 1(true) or 0(false) */
uint8_t checkPreamble(const uint8_t buffer, const uint8_t preamble);


/*! \brief      Implements 32-CRC Computation
  * \brief      If is the flag is "GENERATE", it implements crc computation and store the result in "crc" parameter.
  * \brief      when the flag is "CHECK", it just implements crc computation and if the result is 0, then return 1, else 0
  * \param      crc         - 32-bits buffer for storing the computed crc result
  * \param      src         - Payload which will be done XOR with the pre-defined polynomial
  * \param      src_size    - Bit size of the payload
  * \param      pln         - Polynomial
  * \param      flag        - You can set flag with "GENERATE" for generating crc or "CHECK" for checking crc
  * \return     unsigned 8-bits data - 1(true) or 0(false) */
uint8_t makeCrc(uint8_t* crc, const uint8_t* src, const uint32_t src_size, const uint8_t* polynomial, const uint8_t flag);


/** \brief      Clears the bit string with the given length from the MSB
  * \param      buffer      - Buffer to be initialized
  * \param      bit_size    - Amount of bits to be initialized
  * \return     void */
void clearBuffer(uint8_t* buffer, const uint32_t bit_size);


/** \brief      Initializes the packet buffer with 0
  * \param      frame   - Packet buffer to be initialized
  * \return     void */
void clearFrame(frame_t* frame);


/** \brief      Prints all data of the given data packet
  * \param      frame   - Packet buffer to be printed out
  * \return     void */
void printFrame(const frame_t* frame);


/** \brief      Returns the electrical signal which has been gained thourgh the i/o pins.
  * \brief      If it received an electrical signal on a time slot, then it returns 1, else returns 0
  * \return     unsigned 8-bits data - 1 or 0 */
uint8_t receiveData();
