#include "pe1/card_obj.h"

void CardObj_EmitCommand43(CardObj *obj, unsigned char value) {
    obj->command = 0x43;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
