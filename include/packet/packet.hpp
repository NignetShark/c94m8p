#ifndef __PACKET_H__
#define __PACKET_H__

#include "utils/serial.hpp"

typedef enum packet_type {
    PACKET_NMEA,
    PACKET_UBX,
    PACKET_UNKNOWN
} packet_type_t;

class Packet
{
private:
    /* data */

public:
    packet_type_t type;

    Packet(packet_type_t type);
    ~Packet();
};

#endif