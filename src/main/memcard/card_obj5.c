#include "pe1/card_obj.h"

void CardObj_EmitCommand43(CardObj *arg0, unsigned char arg1) {
    arg0->command = 0x43;
    arg0->payload_2c = (unsigned char *)arg0 + 0x24;
    arg0->pad_24[0] = arg1;
    arg0->payload_2c_len = 1;
}

void CardObj_EmitCommand45(CardObj *arg0) {
    arg0->command = 0x45;
    arg0->payload_2c = 0;
    arg0->payload_2c_len = 0;
}

void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1) {
    arg0->command = 0x4C;
    arg0->payload_2c = (unsigned char *)arg0 + 0x24;
    arg0->pad_24[0] = arg1;
    arg0->payload_2c_len = 1;
}

void CardObj_EmitCommand46(CardObj *arg0, unsigned char arg1) {
    arg0->command = 0x46;
    arg0->payload_2c = (unsigned char *)arg0 + 0x24;
    arg0->pad_24[0] = arg1;
    arg0->payload_2c_len = 1;
}

void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1) {
    arg0->command = 0x47;
    arg0->payload_2c = (unsigned char *)arg0 + 0x24;
    arg0->pad_24[0] = arg1;
    arg0->payload_2c_len = 1;
}

void CardObj_EmitCommand4B(CardObj *arg0) {
    arg0->command = 0x4B;
    arg0->payload_2c = 0;
    arg0->payload_2c_len = 0;
}
