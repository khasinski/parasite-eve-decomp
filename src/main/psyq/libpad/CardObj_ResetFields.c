#include "pe1/card_obj.h"

void CardObj_ResetFields(CardObj *arg0) {
    int count;
    int value;
    char *ptr;

    if (arg0->field_49 != 0) {
        ptr = (char *)arg0 + 0x5D;
        value = 0xFF;
        count = 5;
        arg0->field_49 = 0;
        arg0->field_46 = 0;
        arg0->field_e6 = 0;
        arg0->fn_14 = 0;
        arg0->fn_18 = 0;
        arg0->field_e3 = 0;
        arg0->field_e4 = 0;
        arg0->field_e6 = 0;
        arg0->field_e9 = 0;
        arg0->field_ea = 0;
        arg0->field_00 = 0;
        arg0->field_04 = 0;
        arg0->field_08 = 0;

        do {
            *ptr = value;
            count -= 1;
            ptr += 1;
        } while (count >= 0);
    }
}
