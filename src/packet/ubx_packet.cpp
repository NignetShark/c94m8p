#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include "packet/ubx_packet.hpp"

#define CK_APPEND(cka, ckb, data) cka += data; ckb = ckb + cka

UBXPacket::UBXPacket() : Packet(PACKET_UBX)
{
    data.length = 0;
    data.payload = NULL;
}

UBXPacket::~UBXPacket()
{
    if(data.payload != NULL) free(data.payload);
}


uint16_t UBXPacket::compute_ck() {
    uint8_t ck_a = 0, ck_b = 0;

    CK_APPEND(ck_a, ck_b, (uint8_t)(data.class_field));
    CK_APPEND(ck_a, ck_b, (uint8_t)(data.id_field));
    CK_APPEND(ck_a, ck_b, (uint8_t)(data.length & 0xFF));
    CK_APPEND(ck_a, ck_b, (uint8_t)(data.length >> 8));

    for(int i = 0; i < data.length; i++) {
        CK_APPEND(ck_a, ck_b, (uint8_t)data.payload[i]);
    }

    return ck_b << 8 | ck_a;
}

bool UBXPacket::check_integrity() {
    return data.ck == compute_ck();
}

void UBXPacket::dump() {
    printf("Class : %d \t ID : %d\n", data.class_field, data.id_field);
    printf("Length : %d\n", data.length);
    printf("CK A : %d \t CK B : %d\n", data.ck >> 8, data.ck & 0xFF);
    printf("Payload :\n");
    for(int i = 0; i < data.length; i += 10) {
        int max = i + 10 < data.length ? 10 : data.length - i;
        for(int j = 0; j < max; j++) {
            printf("%02X ", data.payload[i + j]);
        }
        printf("\n");
    }
}


void UBXPacket::parse(Serial* serial) {
    data.class_field = serial->read_char();
    data.id_field = serial->read_char();
    data.length = serial->read_uint16();

    if(data.length > MAX_AUTH_LENGH) {
        throw std::out_of_range("Length exceed max length");
    }

    data.payload = (unsigned char*)malloc(data.length);

    for(int i = 0; i < data.length; i++) {
        data.payload[i] = serial->read_char();
    }

    data.ck = serial->read_uint16();
}

void UBXPacket::send(Serial* serial) {
    data.ck = compute_ck();

    serial->write_char(UBX_SYNC_CHAR_1);
    serial->write_char(UBX_SYNC_CHAR_2);
    serial->write_char(data.class_field);
    serial->write_char(data.id_field);
    serial->write_uint16(data.length);
    serial->write_serial(data.payload, data.length);
    serial->write_uint16(data.ck);
}