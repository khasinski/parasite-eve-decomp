#include "pe1/card_obj.h"

typedef unsigned char u8;

int CardObj_ReadPayloadByte(CardObj *arg0) {
    int index;
    int mode;
    u8 *ptr;

    index = arg0->payload_index - 3;
    mode = arg0->command;
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 0x4D) {
        goto mode4D;
    }
    goto other;

mode0:
    if (index < 6) {
        if (arg0->field_57[index] == 0) {
            return 0;
        }
    }

    if (index >= arg0->payload_28_len) {
        return 0;
    }
    ptr = arg0->payload_28 + index;

load:
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    return *ptr;

mode4D:
    if (index < arg0->payload_2c_len) {
        ptr = arg0->payload_2c + index;
        goto load;
    }
    return 0xFF;

other:
    if (index >= arg0->payload_2c_len) {
        return 0;
    }
    return arg0->payload_2c[index];
}
