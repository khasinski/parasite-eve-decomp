/* GCC_VERSION: 2.8.1 */
#include "pe1/card_obj.h"

extern CardObj *(*D_8009B738)(void);

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
