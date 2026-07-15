/* MASPSX_FLAGS: --stack-return-delay */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

int func_80082778(int channel, int index0, int selector) {
    CardObj *obj;
    int entry;
    int sel;

    obj = D_8009B738();
    if (index0 < 0) {
        return obj->field_e9;
    }
    if (index0 >= obj->field_e9) {
        return 0;
    }
    entry = (int)obj->field_04 + ((index0 << 2) + index0);
    sel = selector - 1;
    if ((unsigned int)sel < 5) {
        switch (sel) {
        case 0:
            return *(unsigned char *)(entry + 0);
        case 1:
            return *(unsigned char *)(entry + 1);
        case 2:
            return *(unsigned char *)(entry + 2);
        case 3:
            return *(unsigned char *)(entry + 3);
        case 4:
            return *(unsigned char *)(entry + 4);
        }
    }
    return 0;
}
