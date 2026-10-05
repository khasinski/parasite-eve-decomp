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
extern int (*D_8009B728)(void *);

int CardObj_CheckAbortOrDispatch(CardObj *arg0) {
    if (*((unsigned char *)arg0 + 0x53) != 0) {
        if (arg0->field_46 == 2) {
            return 1;
        }

        arg0->field_46 = 0xFE;
        return 0;
    }

    D_8009B728(arg0);
    return 0;
}
void CardObj_EmitCommand43(CardObj *obj, unsigned char value) {
    obj->command = 0x43;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand45(CardObj *obj) {
    obj->command = 0x45;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
void CardObj_EmitCommand4C(CardObj *obj, unsigned char value) {
    obj->command = 0x4C;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand46(CardObj *obj, unsigned char value) {
    obj->command = 0x46;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand47(CardObj *obj, unsigned char value) {
    obj->command = 0x47;
    obj->payload_2c = (unsigned char *)obj + 0x24;
    obj->pad_24[0] = value;
    obj->payload_2c_len = 1;
}
void CardObj_EmitCommand4B(CardObj *obj) {
    obj->command = 0x4B;
    obj->payload_2c = 0;
    obj->payload_2c_len = 0;
}
