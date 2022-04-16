#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bkv.h"
#include "bkv_plus.h"

/**
 * key value table:
 * 0x01 [uint8]         frame_type
 * 0x02 [uint32]        time
 * 0x03 [uint32]        sn
 * 0x04 [15 byte]       device_id(imei)
 * 0x05 [16 byte]       device_key
 * 0x0A [uint8]         state
 * 0x0B [int8]          temperature
 * 0x0C [uint16]        battery_voltage
 * 0x0D [uint8]         battery_level
 * 0x0E [uint16]        boot_time
 * 0x11 [int16]         conn_signal_strength
 * 0x12 [int16]         conn_link_quality
 * 0x13 [int8]          conn_signal_ecl
 * 0x14 [int32]         conn_signal_cell_id
 * 0x15 [int16]         conn_signal_pci
 * 0x16 [int8]          conn_signal_snr
 * 0x17 [int16]         conn_signal_ear
 * 0x18 [int16]         event_type
 *
 * login frame payload frame_type=0x01 : [ frame_type[0x01], time[uint32], sn[0x03], device_id[0x04], device_key[0x05] ]
 */

// packing login payload
void pack_login_payload() {
    int size = 60;
    uint8_t data[size];
    memset(data, 0, size);

    int offset = 0;

    // append frame_type
    uint8_t value_frame_type[1] = { 0x01 };
    offset += bkv_append_by_number_key(data + offset, size - offset, 0x01, value_frame_type, 1);

    // append time
    uint32_t time = 1648181602;
    uint8_t value_time[4] = { time >> 24, time >> 16, time >> 8, time };
    offset += bkv_append_by_number_key(data + offset, size - offset, 0x02, value_time, 4);

    // append sn
    uint32_t sn = 1;
    uint8_t value_sn[4] = { sn >> 24, sn >> 16, sn >> 8, sn };
    offset += bkv_append_by_number_key(data + offset, size - offset, 0x03, value_sn, 4);

    // append device_id
    char device_id[] = "82211011002320";
    offset += bkv_append_by_number_key(data + offset, size - offset, 0x04, (uint8_t *)device_id, 15);

    // append device_key
    uint8_t device_key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    offset += bkv_append_by_number_key(data + offset, size - offset, 0x05, device_key, 16);

    bkv_dump(data, offset);
}

// pack heartbeat frame
void pack_heartbeat_payload() {

}

int main() {
    pack_login_payload();

    return 0;
}

