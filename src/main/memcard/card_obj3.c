/* GCC_VERSION: 2.8.1 */

#include "pe1/card_obj.h"

extern int (*g_MemCardIsTransferActiveFn)(void);

void CardObj_EmitReadTransferCommand(CardObj *obj);
void CardObj_ProcessReadPayload(CardObj *obj);

int card_obj3(CardObj *obj, unsigned char *buffer) {
    int cursor;
    int result;
    int state;
    register void (*processFn)(void *) asm("$4");
    int rowCount;
    int columnCount;

    if (buffer == 0) {
        goto return_zero;
    }
    if (obj->field_04 != 0) {
        return 0;
    }
    if (g_MemCardIsTransferActiveFn() == 0) {
        goto initialize;
    }

return_zero:
    return 0;

initialize:
    result = 1;
    asm volatile("" : "+r"(result));
    state = 4;
    cursor = ((int)buffer + 3) >> 2;
    obj->field_49 = state;
    state = 1;
    obj->field_46 = state;
    obj->fn_14 = (void (*)(void *))CardObj_EmitReadTransferCommand;
    asm volatile("" ::: "memory");
    rowCount = obj->field_e3;
    asm volatile("" : "+r"(rowCount) : : "memory");
    processFn = CardObj_ProcessReadPayload;
    obj->fn_18 = processFn;
    asm volatile("" ::: "memory");
    columnCount = obj->field_e9;
    asm volatile("" : "+r"(rowCount), "+r"(columnCount), "+r"(result));

    cursor <<= 2;
    obj->field_00 = (unsigned char *)cursor;
    obj->field_47 = 0;
    asm volatile("" ::: "memory");
    cursor += ((rowCount + 1) >> 1) * 4;
    obj->field_04 = (unsigned char *)cursor;
    cursor += (columnCount * 5 + 3) & 0xFFC;
    obj->field_08 = (unsigned char *)cursor;
    return result;
}
