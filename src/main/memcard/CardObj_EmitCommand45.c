#include "pe1/card_obj.h"

void CardObj_EmitCommand45(CardObj *obj) {
    obj->command = 0x45;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
