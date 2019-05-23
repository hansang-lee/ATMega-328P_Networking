#include <stdlib.h>

typedef struct
{
    uint8_t dst[];
    uint8_t src[];
    uint8_t crc[];
    uint8_t dlc[];
    uint8_t payload[];
} frame_t;
