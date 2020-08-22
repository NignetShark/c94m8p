#ifndef __PACKET_NMEA_H__
#define __PACKET_NMEA_H__

#include "packet/packet.hpp"

#define NMEA_SYNC_CHAR '$'
#define MAX_NMEA_SIZE 255

class NMEAPacket : public Packet
{
private:
    char data[MAX_NMEA_SIZE];
    unsigned int size;
    /* data */
public:

    NMEAPacket();
    ~NMEAPacket();

    void parse(Serial* serial);
    char* raw_data();
};



#endif