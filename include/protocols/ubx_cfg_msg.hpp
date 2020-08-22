#ifndef __UBX_CFG_MSG_H__
#define __UBX_CFG_MSG_H__

#include <stdint.h>

#define UBX_CFG_MSG  0x01
#define UBX_CFG_MSG_LENGTH 3

typedef struct ubx_cfg_msg_rate
{   
    uint8_t msgClass;
    uint8_t msgId;
    uint8_t rate;
} ubx_cfg_msg_rate_t;

#endif