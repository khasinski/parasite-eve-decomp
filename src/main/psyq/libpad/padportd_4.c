/* ASSEMBLER: GNU */
/* PSY-Q LIBPAD PADPORTD, part 4 of 4: CardObj_SwapByteField,
 * CardObj_ReadPayloadByte, LIBPAD_PADPORTD_text_388, CardObj_GetChannelId,
 * CardObj_LookupByChannelId. */
#include "common.h"
#include "pe1/card_obj.h"

void CardObj_SwapByteField(CardObj *obj) {
    u8 command;

    command = obj->command;
    obj->command = 0;
    obj->saved_command = command;
}

int CardObj_ReadPayloadByte(CardObj *arg0) {
    int index;
    int mode;
    u8 *ptr;

    index = arg0->payload_index - 3;
    mode = arg0->command;
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 0x4D) {
        goto mode4D;
    }
    goto other;

mode0:
    if (index < 6) {
        if (arg0->field_57[index] == 0) {
            return 0;
        }
    }

    if (index >= arg0->payload_28_len) {
        return 0;
    }
    ptr = arg0->payload_28 + index;

load:
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    return *ptr;

mode4D:
    if (index < arg0->payload_2c_len) {
        ptr = arg0->payload_2c + index;
        goto load;
    }
    return 0xFF;

other:
    if (index >= arg0->payload_2c_len) {
        return 0;
    }
    return arg0->payload_2c[index];
}

void bzero(void *, int);
extern int D_8009B76C;
void LIBPAD_PADPORTD_text_388(CardObj *port) {
    register int limit, i, j, mask asm("$7"), active asm("$6");
    register int count;
    register int offset;
    register int one;
    register u8 *map, *data;
    bzero(port->field_57, 6);
    if (port->field_e6 && port->payload_28) {
        limit = 6;
        if (port->payload_28_len < 7) {
            asm("" ::: "memory");
            limit = port->payload_28_len;
        }
        i = 0;
        if (port->field_e9) {
            one = 1;
            offset = 0;
            do {
                active = 0;
                mask = 1;
                if (((PadCapabilityRecord *)(offset + (u32)port->capabilities))
                        ->payloadBytes)
                    mask = 255;
                map = port->field_5d;
                data = port->payload_28;
                j = 0;
                if (limit) {
                search:
                    if (*map != i || !(*data & mask)) {
                        map++;
                        j++;
                        data++;
                        if (j < limit)
                            goto search;
                    } else
                        goto selected;
                }
            search_done:
                if (active) {
                    int total =
                        D_8009B76C +
                        ((PadCapabilityRecord *)(offset + (u32)port->capabilities))
                            ->activationCost;
                    if (total < 61)
                        D_8009B76C = total;
                    else {
                        goto deny;
                    selected:
                        active = 1;
                        goto search_done;
                    deny:
                        active = 0;
                    }
                    if (active) {
                        map = port->field_5d;
                        data = port->field_57;
                        j = 0;
                        if (limit)
                            do {
                                int same = *map == i;
                                map++;
                                if (same)
                                    *data = one;
                                j++;
                                data++;
                            } while (j < limit);
                    }
                }
                count = port->field_e9;
                asm("" : "=r"(i) : "0"(i), "r"(count));
                i++;
                offset += 5;
            } while (i < count);
        }
    } else {
        if (((unsigned)(port->field_e8 - 4) < 2 || port->field_e8 == 7) &&
            !port->field_e6 && port->payload_28_len >= 2) {
            if ((port->payload_28[0] & 192) == 64 && (port->payload_28[1] & 1) &&
                D_8009B76C + 10 < 61) {
                port->field_57[1] = 1;
                port->field_57[0] = 1;
                D_8009B76C += 10;
            }
        } else {
            asm("" ::: "memory");
            if (port->field_e8 == 3)
                port->field_57[0] = 1;
            else if (!port->field_e6) {
                register int value = 1;
                register int k asm("$3") = 5;
                register u8 *cursor = (u8 *)port + 5;
                do {
                    cursor[0x57] = value;
                    k--;
                    cursor--;
                } while (k >= 0);
            }
        }
    }
}

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
        candidate += 1;
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
