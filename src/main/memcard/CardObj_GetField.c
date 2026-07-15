/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

int CardObj_GetField(int channel, int mode, int index) {
    CardObj *obj;

    obj = D_8009B738();
    switch (mode) {
    case 1:
        return obj->field_e8;
    case 2:
        return obj->field_e6;
    case 3:
        return obj->field_e4;
    case 4:
        if (index < 0) {
            return obj->field_e3;
        }
        if (index < obj->field_e3) {
            return *(unsigned short *)((index << 1) + (int)obj->field_00);
        }
        goto late_fail;
    case 100:
        return obj->field_4c;
    default:
        return 0;
    }

late_fail:
    return 0;
}
