#include "pe1/card_obj.h"

extern CardObj D_800A5B70[];

int CardObj_GetChannelId(CardObj *entry) {
    CardObj *candidate;
    int index;
    int value;

    index = 0;
    value = 0x10;
    candidate = D_800A5B70;
    for (; index < 2; index++) {
        if (entry == candidate) {
            return value;
        }
        value += 0x10;
        do {
            candidate += 1;
        } while (0);
    }

    return 0xFF;
}

CardObj *CardObj_LookupByChannelId(int value) {
    CardObj *entry = D_800A5B70;

    if ((value & 0xF0) != 0) {
        entry += 1;
    }
    return entry;
}
