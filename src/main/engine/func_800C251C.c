#include "common.h"
#include "pe1/field_engine_state.h"
/* CC1_FLAGS: -fno-schedule-insns */

int FieldEng_GetStatus(char *obj);
int func_800C2DA0(u16 slot);
int printf(char *fmt);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F32A8;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20EC[];

int func_800C251C(char *obj, int (**handlers)(char *obj, void *entry, void *data)) {
    int result;
    s16 i;
    char *data;
    char *table;

    result = 0;
    table = *(char **)(obj + 0x78);
    data = *(char **)(obj + 8);

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;
    D_800F33B0 = table;

    obj[3] = obj[0x12];

    if (FieldEng_GetStatus(obj) == 3) {
        if ((u8)obj[1] != 0x24) {
            char *state = *(char **)(**(char ***)(obj + 8) + 0x18);

            if ((u8)state[0] == 1) {
                state[0] = 2;
            }
        }
    }

    for (i = 0; i < 0x40; i++) {
        FieldEngSlot *entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

        if (entry->flag == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers[entry->handler_id];

            if (handler != (void *)-1) {
                handler(obj, entry, D_800F3330 + entry->data_offset);
            } else {
                printf(D_800C20EC);
            }
            {
                FieldEngSlot *counter_entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

                counter_entry->counter += 1;
            }
        }

        {
            int status_offset = i * 6;
            u8 *status_base = D_800F34F4;

            if (((FieldEngSlot *)(status_offset + (int)status_base))->flag == 2) {
                result |= func_800C2DA0((u16)i);
            }
        }
    }

    if (FieldEng_GetStatus(obj) == 3) {
        char *inner = *(char **)data;
        u32 flags = *(u32 *)inner;

        if (*(u8 *)(inner + ((flags >> 17) & 0x70) + 0x1C) == 0 && (flags & 0x180E) != 0) {
            result = -1;
        }
    }

    return result;
}
