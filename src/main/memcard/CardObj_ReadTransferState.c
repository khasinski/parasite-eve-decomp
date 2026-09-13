/* GCC_VERSION: 2.8.1 */

#include "pe1/card_obj.h"

int CardObj_CalcReadPayloadSize(CardObj *obj);
int CardObj_StartReadTransfer(CardObj *obj, unsigned char *dst);

#define CARD_RESPONSE(obj) \
    (*(unsigned char * volatile *)&(obj)->response_3c)

int CardObj_AdvanceReadLayout(CardObj *obj) {
    unsigned int chunk;
    register unsigned int next asm("$2");
    unsigned int responseValue;
    switch (obj->field_46) {
    case 2:
        obj->field_e3 = CARD_RESPONSE(obj)[3];
        obj->field_e4 = CARD_RESPONSE(obj)[4];
        obj->field_e6 = 0;
        obj->field_e9 = CARD_RESPONSE(obj)[5];
        obj->field_ea = CARD_RESPONSE(obj)[6];
        obj->field_ec = 0;
        break;

    case 3:
        responseValue = obj->response_3c[4];
        next = obj->response_3c[5];
        obj->field_47 = 0;
        obj->field_e6 = (responseValue << 8) + next;
        break;

    case 4:
        chunk = obj->field_ec;
        next = obj->field_47;
        responseValue = obj->response_3c[4];
        next++;
        obj->field_47 = next;
        chunk += 8;
        chunk += (responseValue + 3) & 0x1FC;
        obj->field_ec = chunk;
        if ((next & 0xFF) < obj->field_ea) {
return_zero:
            return 0;
        }

        if (CardObj_CalcReadPayloadSize(obj) >= 0x81) {
            obj->field_46 = 0xFE;
            obj->field_49 = 2;
            goto return_zero;
        }

        obj->field_46 = 0xFF;
        CardObj_StartReadTransfer(obj, (unsigned char *)obj + 0x63);
        obj->field_46 = 2;
        goto return_zero;
    }

    return 1;
}

int CardObj_CalcReadPayloadSize(CardObj *arg0) {
    int first;
    int second;
    int base;
    int raw_first;
    int raw_second;

    raw_first = arg0->field_e3;
    raw_second = arg0->field_e9;
    base = arg0->field_ec;

    first = raw_first + 1;
    asm volatile("" : "=r"(first) : "0"(first));
    first >>= 1;
    first <<= 2;

    second = (raw_second << 2) + raw_second;
    second = (second + 3) & 0xFFC;
    second += 4;

    first += second;
    return first + base;
}

extern int (*g_MemCardIsTransferActiveFn)(void);

void CardObj_EmitReadTransferCommand(CardObj *obj);
void LIBPAD_PADCMD_text_3A0(CardObj *obj);

int CardObj_StartReadTransfer(CardObj *obj, unsigned char *buffer) {
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
    processFn = LIBPAD_PADCMD_text_3A0;
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
