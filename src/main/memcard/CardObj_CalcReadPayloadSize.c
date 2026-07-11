#include "pe1/card_obj.h"

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
