#include "pe1/card_obj.h"

void CardObj_EmitCommand4D(CardObj *arg0) {
    int value = arg0->field_20;

    arg0->command = 0x4D;
    arg0->payload_2c_len = 6;
    arg0->payload_2c = (unsigned char *)value;
}
