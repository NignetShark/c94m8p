#ifndef __C94M8P_H__
#define __C94M8P_H__

#define DEBUG

#include "utils/serial.hpp"
#include "packet/ubx_packet.hpp"
#include "packet/nmea_packet.hpp"
#include "protocols/ubx_common.hpp"

#define RETRY_FOR_ACK 3 // Number of retry to get the acknolegment.

class C94M8P {
    
    private:
    Serial* serial;
    
    packet_type_t parse_packet_type();

    public:
    C94M8P(char* com, int baudrate);
    ~C94M8P();

    /**
     * Get the next packet (message) available
     **/
    Packet* get_incoming_packet();

    /**
     * Requesting a specific message
     * See Ublox protocol specification to get class and id numbers.
     **/
    void poll(uint8_t class_field, uint8_t id_field);

    /**
     * Subscribe to a specific message
     * See Ublox protocol specification to get class and id numbers.
     * @return true for a ack and false for a nack
     * @throw a logic exception if no ack was given.
     **/
    bool subscribe(uint8_t class_field, uint8_t id_field, uint8_t rate);

    /**
     * Unsubscribe to a specific message
     * See subscribe method.
     **/
    bool unsubscribe(uint8_t class_field, uint8_t id_field);

    /**
     * Wait for a specific ack.
     * @return 0 for a ack, 1 for a nack and -1 for no ack.
     **/
    int waitAck(uint8_t clsId, uint8_t msgId);

    /**
     * Disable NMEA message by unscribing to each NMEA messages
     * @return true for a ack and false for a nack
     * @throw a logic exception if no ack was given.
     **/
    bool disable_NMEA();

    /**
     * Set port configuration (send a UBX_CFG_PRT message)
     * @param port_id typically 1 = UART1
     * @param baudrate baudrate (WARNING: there is no auto-reconnection)
     * @param proto_in mask used to select input protocols (see UBlox protocol specification)
     * @param proto_out mask used to select output protocols (see UBlox protocol specification)
     * WARNING: there is no check for acknowledgement (due to potential disconnection).
     **/
    void setPrtConfig(uint8_t port_id, uint32_t baudrate, uint16_t proto_in, uint16_t proto_out);
};

#endif