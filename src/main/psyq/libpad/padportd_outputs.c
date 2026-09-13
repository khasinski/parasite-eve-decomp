/* ASSEMBLER: GNU */
#include "pe1/card_obj.h"
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
                if (((PadCapabilityRecord *)((u32)offset + (u32)port->field_04))
                        ->bytes[2])
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
                        ((PadCapabilityRecord *)((u32)offset + (u32)port->field_04))
                            ->bytes[3];
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
        asm("" ::: "memory");
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
