#include <stdio.h>

#include "c94m8p.hpp"
#include "protocols/ubx_cfg_prt.hpp"
#include "protocols/ubx_nav_status.hpp"
#include "protocols/ubx_ack.hpp"

C94M8P m = C94M8P("/dev/ttyACM0", B9600);

void dump_packet() {
    Packet* packet = m.get_incoming_packet();
    if(packet != NULL) {
        switch (packet->type)
        {
        case PACKET_NMEA:
            {
                NMEAPacket* nmea = (NMEAPacket*)packet; // Down cast to NMEA packet
                printf("NMEA : %s\n", nmea->raw_data()); // Only raw data is possible for now..
                delete nmea; // DON'T FORGET to delete each packet received.
                break;
            }
        case PACKET_UBX:
            {
                UBXPacket* ubx = (UBXPacket*)packet; // Down cast to UBX packet
                
                ubx->dump(); // See metadata of the UBX packet.

                // Do anything you want ...
                switch (ubx->data.class_field) // Sort by class
                {
                    case UBX_NAV:
                        if(ubx->data.id_field == UBX_NAV_STATUS) { 

                            // Cast as a struct to get the fields.
                            ubx_nav_status_t* status = (ubx_nav_status_t*)ubx->data.payload;
                            printf("iTOW %d\n", status->iTOW);
                            printf("GPS fix %d\n", status->gpsFix);

                        } else {
                            printf("Unknown UBX NAV id\n");
                        }
                        break;

                    case UBX_ACK:
                        if(ubx->data.id_field == UBX_ACK_ACK) { // Sort by field
                            printf("Ack !\n");
                        } else if(ubx->data.id_field == UBX_ACK_NACK) {
                            printf("Nack !\n");
                        } else {
                            printf("Unknown UBX ACK id\n");
                        }
                        break;

                    // Add other sections if needed.

                    default:
                        printf("Unknown UBX class\n");
                        break;
                }
                delete ubx; // DON'T FORGET to delete each packet received.
                break;
            }
        default:
            printf("Unknown UBX class\n");
            break;
        }
    } else {
        // It can fail for several reasons :
        // * unknown packet
        // * checksum invalid
        // * unable to read data
        // * serial not available
        printf("Failed to get a packet\n");
    }
}

int main(int argc, char** argv) {
    // Set port configuration
    m.setPrtConfig(1, 9600, PROTO_MASK_UBX | PROTO_MASK_NMEA, PROTO_MASK_UBX | PROTO_MASK_NMEA);
    m.waitAck(UBX_CFG, UBX_CFG_PRT); // Set port do not automaticaly check ack.

    // Disable NMEA messages (only GPS content. TXT, THS, GPQ cannot be disabled)
    m.disable_NMEA(); 

    // Subscrive to NAV STATUS messages
    m.subscribe(UBX_NAV, UBX_NAV_STATUS, 2);

    // Asking to get a NAV STATUS messages
    m.poll(UBX_NAV, UBX_NAV_STATUS);
    
    // Get the 10 next packets
    for(int i = 0; i < 10; i++) {
        dump_packet();
    }
}


