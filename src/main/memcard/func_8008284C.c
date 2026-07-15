/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

int func_8008284C(int channel, int index0, int index1) {
    CardObj *obj;
    int entry;

    obj = D_8009B738();
    if (index0 < 0) {
        return obj->field_ea;
    }
    if (index0 >= obj->field_ea) {
        return 0;
    }
    entry = (int)obj->field_08 + (index0 << 3);
    if (index1 < 0) {
        return *(unsigned char *)entry;
    }
    if (index1 >= *(unsigned char *)entry) {
        return 0;
    }
    return *(unsigned char *)(*(int *)(entry + 4) + index1);
}
