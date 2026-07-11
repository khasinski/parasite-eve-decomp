#include "pe1/card_obj.h"

void CardObj_EmitReadIdCommand(CardObj *arg0) {
    int state = arg0->field_46;

    switch (state) {
    case 2:
        arg0->command = 0x44;
        arg0->payload_2c = (unsigned char *)arg0 + 0x51;
        arg0->payload_2c_len = state;
        break;
    case 3:
        arg0->command = 0x4D;
        arg0->payload_2c = (unsigned char *)arg0 + 0x5D;
        arg0->payload_2c_len = 6;
        break;
    }
}
