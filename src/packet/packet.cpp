#include "packet/packet.hpp"

Packet::Packet(packet_type_t type) {
    this->type = type;
}

Packet::~Packet() {
    
}