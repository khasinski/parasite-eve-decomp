/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBPAD PADENTRY, part 2 of 2: PadInfoMode, PadInfoAct, PadInfoComb,
 * PadSetActAlign, PadSetMainMode, PadSetAct. */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

int PadInfoMode(int channel, int mode, int index) {
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

extern CardObj *(*g_MemCardObjLookupFn)(void);

int PadInfoAct(int channel, int index, int field) {
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

int PadInfoComb(int channel, int index0, int index1) {
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
int _padSetActAlign(CardObj *obj, int command);

void PadSetActAlign(int channel, int command) {
    _padSetActAlign(D_8009B738(), command);
}
void _padSetMainMode(CardObj *obj, int byte1, int byte2);

void PadSetMainMode(int channel, unsigned char byte1, unsigned char byte2) {
    _padSetMainMode(D_8009B738(), byte1, byte2);
}
void func_800835A4(CardObj *obj, int payload, int size);

void PadSetAct(int channel, int payload, int size) {
    func_800835A4(D_8009B738(), payload, size);
}

unsigned int gap_memcard_card_obj_tail_731BC[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
