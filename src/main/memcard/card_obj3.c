/* MASPSX_FLAGS: --stack-return-delay */

#include "include_asm.h"

#include "pe1/card_obj.h"

extern int (*D_8009B740)(void);
int CardObj_ProcessReadPayload(CardObj *arg0);
void CardObj_EmitReadTransferCommand(CardObj *arg0);

int CardObj_BeginReadTransfer(CardObj *obj, int size) {
    int total;
    int bytes;
    int extra;
    int raw_e3;
    int raw_e9;
    register int ret asm("$2");
    int four;

    if (size == 0) {
        return 0;
    }
    if (obj->field_04 != 0) {
        return 0;
    }
    if (D_8009B740() == 0) {
        ret = 1;
        four = 4;
        total = (size + 3) >> 2;
        asm volatile("" : "=r"(four) : "0"(four));
        obj->field_49 = four;
        obj->field_46 = 1;
        *(void **)((unsigned char *)obj + 0x14) = CardObj_EmitReadTransferCommand;
        raw_e3 = obj->field_e3;
        *(void **)((unsigned char *)obj + 0x18) = CardObj_ProcessReadPayload;
        raw_e9 = obj->field_e9;
        bytes = total << 2;
        obj->field_00 = (unsigned char *)bytes;
        obj->field_47 = 0;

        bytes += ((raw_e3 + 1) >> 1) << 2;
        extra = ((raw_e9 << 2) + raw_e9 + 3) & 0xFFC;
        obj->field_04 = (unsigned char *)bytes;
        obj->field_08 = (unsigned char *)(bytes + extra);
        return ret;
    }
    return 0;
}
