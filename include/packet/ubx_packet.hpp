#ifndef __UBX_PACKET_H__
#define __UBX_PACKET_H__

#include <stdint.h>
#include "packet/packet.hpp"

#define MAX_AUTH_LENGH 1000
#define UBX_SYNC_CHAR_1 0xB5
#define UBX_SYNC_CHAR_2 0x62


typedef struct ubx_data
{
    uint8_t class_field;
    uint8_t id_field;
    uint16_t length;
    unsigned char* payload;
    uint16_t ck;
} ubx_data_t;


class UBXPacket : public Packet
{
private:
    uint16_t compute_ck();

public:
    ubx_data_t data;

    UBXPacket();
    ~UBXPacket();

    bool check_integrity();
    void parse(Serial* serial);
    void send(Serial* serial);

    void dump();
};

#endif