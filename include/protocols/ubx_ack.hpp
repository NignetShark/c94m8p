#ifndef __UBX_ACK_H__
#define __UBX_ACK_H__

#include <stdint.h>

#define UBX_ACK_ACK  0x01
#define UBX_ACK_NACK  0x00
#define UBX_ACK_LENGTH 2

typedef struct ubx_ack
{   
    uint8_t clsId;
    uint8_t msgId;
} ubx_ack_t;

#endif