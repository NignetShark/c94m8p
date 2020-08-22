// Open, read, write, close.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Debug purposes
#include <cerrno>
#include <stdio.h>
#include <string.h>

// malloc
#include <stdlib.h>

// errors
#include <stdexcept>

// subscribe
#include "protocols/ubx_common.hpp"
#include "protocols/nmea_common.hpp"
#include "protocols/ubx_cfg_msg.hpp"
#include "protocols/ubx_cfg_prt.hpp"
#include "protocols/ubx_ack.hpp"
#include "c94m8p.hpp"

C94M8P::C94M8P(char* com, int baudrate) {
    serial = new Serial();
    serial->connect(com, baudrate);
}

C94M8P::~C94M8P() {
    if(serial != NULL) delete serial;
}

packet_type_t C94M8P::parse_packet_type() {
    unsigned char symbol = serial->read_char();
    if(symbol == NMEA_SYNC_CHAR) {
        return PACKET_NMEA;
    } else if(symbol == UBX_SYNC_CHAR_1) {
        if(serial->read_char() == UBX_SYNC_CHAR_2) {
            return PACKET_UBX;
        }
    }
    return PACKET_UNKNOWN;
}

Packet* C94M8P::get_incoming_packet() {
    packet_type_t packet_type = parse_packet_type();

    switch (packet_type)
    {
        case PACKET_NMEA:
        {
            NMEAPacket* nmea_packet = new NMEAPacket();
            nmea_packet->parse(serial);
            return (Packet*)nmea_packet;
            break;
        }
        case PACKET_UBX:
        {
            UBXPacket* ubx_packet = new UBXPacket();
            ubx_packet->parse(serial);
            if(ubx_packet->check_integrity() == false) return NULL;
            return (Packet*)ubx_packet;
            break;
        }
        default:
            printf("Unknown.\n");
            break;
    }
    return NULL;
}

void C94M8P::poll(uint8_t class_field, uint8_t id_field){
    UBXPacket packet = UBXPacket();
    packet.data.class_field = class_field;
    packet.data.id_field = id_field;
    packet.data.payload = NULL;
    packet.data.length = 0;
    packet.send(serial);
}

int C94M8P::waitAck(uint8_t clsId, uint8_t msgId) {
    int retry = RETRY_FOR_ACK;

    while(retry > 0) {
        Packet* ack_packet = get_incoming_packet();
        if(ack_packet != NULL) {
            if(ack_packet->type == PACKET_UBX) {
                UBXPacket* ubx = (UBXPacket*)ack_packet;
                if(ubx->data.class_field == UBX_ACK) {
                    ubx_ack_t* payload = (ubx_ack_t*)ubx->data.payload;
                    if(ubx->data.id_field == UBX_ACK_ACK) {
                        if(payload->clsId == clsId && payload->msgId == msgId) {
                            delete ubx;
                            return 0;
                        }
                    } else if(ubx->data.id_field == UBX_ACK_NACK) {
                        if(payload->clsId == clsId && payload->msgId == msgId) {
                            delete ubx;
                            return 1;
                        }
                    }
                }
            }
            delete ack_packet;
        }
        retry--;
    }

    return -1;
}

bool C94M8P::subscribe(uint8_t class_field, uint8_t id_field, uint8_t rate){
    ubx_cfg_msg_rate_t* payload = (ubx_cfg_msg_rate_t*)malloc(UBX_CFG_MSG_LENGTH);
    payload->msgClass = class_field;
    payload->msgId = id_field;
    payload->rate = rate;

    UBXPacket packet = UBXPacket();
    packet.data.class_field = UBX_CFG;
    packet.data.id_field = UBX_CFG_MSG;
    packet.data.payload = (unsigned char*)payload;
    packet.data.length = UBX_CFG_MSG_LENGTH;
    packet.send(serial);

    // check ack
    int status = waitAck(UBX_CFG, UBX_CFG_MSG);

    if(status < 0) throw std::logic_error("Message not acknoledged.");
    return !(bool)status;
}

bool C94M8P::unsubscribe(uint8_t class_field, uint8_t id_field) {
    return subscribe(class_field, id_field, 0);
}

bool C94M8P::disable_NMEA() {
    bool result = true;
    result &= unsubscribe(NMEA_CLASS, NMEA_GGA);
    result &= unsubscribe(NMEA_CLASS, NMEA_GLL);
    result &= unsubscribe(NMEA_CLASS, NMEA_GSA);
    result &= unsubscribe(NMEA_CLASS, NMEA_GSV);
    result &= unsubscribe(NMEA_CLASS, NMEA_RMC);
    result &= unsubscribe(NMEA_CLASS, NMEA_VTG);
    result &= unsubscribe(NMEA_CLASS, NMEA_GRS);
    result &= unsubscribe(NMEA_CLASS, NMEA_GST);
    result &= unsubscribe(NMEA_CLASS, NMEA_ZDA);
    result &= unsubscribe(NMEA_CLASS, NMEA_DTM);

    // Cannot unsubscribe to the following NMEA messages
    //result &= unsubscribe(NMEA_CLASS, NMEA_THS);
    //result &= unsubscribe(NMEA_CLASS, NMEA_GPQ);
    //result &= unsubscribe(NMEA_CLASS, NMEA_TXT);
    return result;
}

void C94M8P::setPrtConfig(uint8_t port_id, uint32_t baudrate, uint16_t proto_in, uint16_t proto_out) {
    ubx_cfg_prt_t* payload = (ubx_cfg_prt_t*)malloc(UBX_CFG_PRT_LENGTH);
    payload->portID = port_id;
    payload->txReady = 0;
    payload->reserved0 = 0;
    payload->mode = MODE_CHAR_LEN_8 << MODE_CHAR_LEN_OFFSET | MODE_PARITY_NONE << MODE_PARITY_OFFSET | MODE_NSTOP_BITS_1 << MODE_NSTOP_BITS_OFFSET;
    payload->baudRate = baudrate;
    payload->inProtoMask = proto_in;
    payload->outProtoMask = proto_out;
    payload->flags = 0;
    payload->reserved2 = 0;

    UBXPacket ubx = UBXPacket();
    ubx.data.class_field = UBX_CFG;
    ubx.data.id_field = UBX_CFG_PRT;
    ubx.data.length = UBX_CFG_PRT_LENGTH;
    ubx.data.payload = (unsigned char*)payload;
    
    ubx.send(serial);
}