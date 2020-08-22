#ifndef __UBX_NAV_STATUS_H__
#define __UBX_NAV_STATUS_H__

#include <stdint.h>
#include <stdio.h>

#define UBX_NAV_STATUS  0x03

typedef struct ubx_nav_status
{   
    uint32_t iTOW;
    uint8_t gpsFix;
    uint8_t flags;
    uint8_t fixStat;
    uint8_t flags2;
    uint32_t ttff;
    uint32_t msss;
} ubx_nav_status_t;

#endif