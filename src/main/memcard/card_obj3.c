#include "include_asm.h"

#include "pe1/card_obj.h"

typedef unsigned char u8;

void CardObj_EmitCommand4C(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand46(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand47(CardObj *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(CardObj *arg0);

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

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/card_obj3", CardObj_BeginReadTransfer);

void CardObj_EmitReadTransferCommand(CardObj *arg0)
{
    int state = arg0->field_46;

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, arg0->field_47);
        break;
    case 3:
        CardObj_EmitCommand46(arg0, arg0->field_47);
        break;
    case 4:
        if (arg0->field_48 == 0) {
            CardObj_EmitCommand47(arg0, arg0->field_47);
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}
