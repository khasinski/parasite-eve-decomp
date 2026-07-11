#include "pe1/card_obj.h"

int CardObj_IsTransferActive(CardObj *obj) {
    int value;
    int field;

    if (obj->field_e6 != 0) {
        value = 0xFF;
        field = obj->field_46;
        if (field == value) {
            goto ret_zero;
        }
    }

    return 1;

ret_zero:
    return 0;
}
