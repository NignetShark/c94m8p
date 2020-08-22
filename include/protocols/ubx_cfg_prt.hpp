#ifndef __UBX_CFG_PRT_H__
#define __UBX_CFG_PRT_H__

#include <stdint.h>

#define UBX_CFG_PRT         0x00
#define UBX_CFG_PRT_LENGTH  20

#define PROTO_MASK_RTCM_3   0b100000
#define PROTO_MASK_NMEA     0b10
#define PROTO_MASK_UBX      0b1

#define MODE_CHAR_LEN_8     0b11
#define MODE_PARITY_NONE    0b100
#define MODE_NSTOP_BITS_1   0b00

#define MODE_NSTOP_BITS_OFFSET  12
#define MODE_PARITY_OFFSET      9
#define MODE_CHAR_LEN_OFFSET    6

typedef struct ubx_cfg_prt
{   
    uint8_t portID;
    uint8_t reserved0;
    uint16_t txReady;
    uint32_t mode;
    uint32_t baudRate;
    uint16_t inProtoMask;
    uint16_t outProtoMask;
    uint16_t flags;
    uint16_t reserved2;
} ubx_cfg_prt_t;

#endif