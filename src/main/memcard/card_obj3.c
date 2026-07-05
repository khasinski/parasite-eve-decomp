#include "include_asm.h"

typedef unsigned char u8;

void CardObj_EmitCommand4C(unsigned char *arg0, unsigned char arg1);
void CardObj_EmitCommand46(unsigned char *arg0, unsigned char arg1);
void CardObj_EmitCommand47(unsigned char *arg0, unsigned char arg1);
void CardObj_EmitCommand4B(unsigned char *arg0);

int CardObj_CalcReadPayloadSize(void *arg0) {
    int first;
    int second;
    int base;
    int raw_first;
    int raw_second;

    raw_first = *(u8 *)((char *)arg0 + 0xE3);
    raw_second = *(u8 *)((char *)arg0 + 0xE9);
    base = *(int *)((char *)arg0 + 0xEC);

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

void CardObj_EmitReadTransferCommand(void *arg0)
{
    int state = *((unsigned char *)arg0 + 0x46);

    switch (state) {
    case 2:
        CardObj_EmitCommand4C(arg0, *((unsigned char *)arg0 + 0x47));
        break;
    case 3:
        CardObj_EmitCommand46(arg0, *((unsigned char *)arg0 + 0x47));
        break;
    case 4:
        if (*((unsigned char *)arg0 + 0x48) == 0) {
            CardObj_EmitCommand47(arg0, *((unsigned char *)arg0 + 0x47));
        } else {
            CardObj_EmitCommand4B(arg0);
        }
        break;
    }
}
