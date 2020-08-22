#ifndef __UBX_NAV_SVIN_H__
#define __UBX_NAV_SVIN_H__

#include <stdint.h>

#define UBX_NAV_SVIN  0x3B
#define UBX_NAV_SVIN_LENGTH 40

typedef struct ubx_nav_svin
{   
    uint8_t version;
    uint8_t reserved1[3];
    uint32_t iTOW;
    uint32_t dur;
    int32_t meanX;
    int32_t meanY;
    int32_t meanZ;
    int8_t meanXHP;
    int8_t meanYHP;
    int8_t meanZHP;
    uint8_t reserved2;
    uint32_t meanAcc;
    uint32_t obs;
    uint8_t valid;
    uint8_t active;
    uint8_t reserved3[2];
} ubx_nav_status_t;


#endif