#include "packet/nmea_packet.hpp"

NMEAPacket::NMEAPacket() : Packet(PACKET_NMEA) {
    size = 0;
}

NMEAPacket::~NMEAPacket() {
    
}


char* NMEAPacket::raw_data() {
    data[size >= MAX_NMEA_SIZE ? MAX_NMEA_SIZE - 1 : size] = 0;
    return data;
}

void NMEAPacket::parse(Serial* serial) {
    int index = 0;
    int last_symbol = 0;
    char symbol = serial->read_char();
   
    while(true)
    {   
        size = size + 1;
        if(index < MAX_NMEA_SIZE) data[index++] = symbol;
        
        if(last_symbol == 0x0D && symbol == 0x0A) return; // End of message

        last_symbol = symbol;
        symbol = serial->read_char();
    }
}


