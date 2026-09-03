/* GCC_VERSION: 2.8.1 */

#include "pe1/card_obj.h"

int CardObj_CalcReadPayloadSize(CardObj *obj);
void card_obj3(CardObj *obj, unsigned char *dst);

#define CARD_RESPONSE(obj) \
    (*(unsigned char * volatile *)&(obj)->response_3c)

int CardObj_AdvanceReadLayout(CardObj *obj) {
    unsigned int chunk;
    register unsigned int next asm("$2");
    unsigned int responseValue;
    switch (obj->field_46) {
    case 2:
        obj->field_e3 = CARD_RESPONSE(obj)[3];
        obj->field_e4 = CARD_RESPONSE(obj)[4];
        obj->field_e6 = 0;
        obj->field_e9 = CARD_RESPONSE(obj)[5];
        obj->field_ea = CARD_RESPONSE(obj)[6];
        obj->field_ec = 0;
        break;

    case 3:
        responseValue = obj->response_3c[4];
        next = obj->response_3c[5];
        obj->field_47 = 0;
        obj->field_e6 = (responseValue << 8) + next;
        break;

    case 4:
        chunk = obj->field_ec;
        next = obj->field_47;
        responseValue = obj->response_3c[4];
        next++;
        obj->field_47 = next;
        chunk += 8;
        chunk += (responseValue + 3) & 0x1FC;
        obj->field_ec = chunk;
        if ((next & 0xFF) < obj->field_ea) {
return_zero:
            return 0;
        }

        if (CardObj_CalcReadPayloadSize(obj) >= 0x81) {
            obj->field_46 = 0xFE;
            obj->field_49 = 2;
            goto return_zero;
        }

        obj->field_46 = 0xFF;
        card_obj3(obj, (unsigned char *)obj + 0x63);
        obj->field_46 = 2;
        goto return_zero;
    }

    return 1;
}
