/* CC1_FLAGS: -fno-schedule-insns */
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;

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
        u8 *entry = (u8 *)(i * 6 + (int)D_800F34F4);

        if ((signed char)entry[1] == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers[entry[0]];

            if (handler != (void *)-1) {
                handler(obj, entry, D_800F3330 + *(short *)(entry + 4));
            } else {
                printf(D_800C20EC);
            }
            {
                register u8 *counter_entry asm("$3") = (u8 *)(i * 6 + (int)D_800F34F4);

                *(u16 *)(counter_entry + 2) += 1;
            }
        }

        {
            int status_offset = i * 6;
            register u8 *status_base asm("$3") = D_800F34F4;

            if ((signed char)((u8 *)(status_offset + (int)status_base))[1] == 2) {
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
