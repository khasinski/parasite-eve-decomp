/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/card_obj.h"

extern CardObj *(*g_MemCardObjLookupFn)(void);

int func_80082778(int channel, int index, int field) {
    CardObj *obj;
    unsigned char *entry;

    obj = g_MemCardObjLookupFn();
    if (index < 0) {
        return obj->field_e9;
    }
    if (index >= obj->field_e9) {
        return 0;
    }

    entry = obj->field_04 + index * 5;
    switch (field) {
    case 1:
        return entry[0];
    case 2:
        return entry[1];
    case 3:
        return entry[2];
    case 4:
        return entry[3];
    case 5:
        return entry[4];
    default:
        return 0;
    }
}
