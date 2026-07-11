#include "pe1/card_obj.h"

void CardObj_EmitCommand4B(CardObj *obj) {
    obj->command = 0x4B;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
