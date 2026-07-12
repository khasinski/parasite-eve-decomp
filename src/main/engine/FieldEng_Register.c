/* CC1_FLAGS: -fno-schedule-insns */
typedef unsigned char u8;
typedef signed short s16;

int printf(char *fmt);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20C8[];

int FieldEng_Register(char *obj, int (**handlers)(char *obj, void *entry, void *data)) {
    register char *obj_s0 asm("$16") = obj;
    register int (**handlers_s2)(char *obj, void *entry, void *data) asm("$18") = handlers;
    s16 i;
    char *table;

    i = 0;
    table = *(char **)(obj_s0 + 0x78);
    D_800E2248 = obj_s0 + 0xC;
    D_800F34F4 = (u8 *)(obj_s0 + 0x80);
    D_800F3330 = obj_s0 + 0x200;
    D_800F33B0 = table;

    for (; i < 0x40; i++) {
        u8 *entry = (u8 *)((s16)i * 6 + (int)D_800F34F4);

        if ((signed char)entry[1] == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers_s2[entry[0]];

            if (handler != (void *)-1) {
                handler(obj_s0, entry, D_800F3330 + *(s16 *)(entry + 4));
            } else {
                printf(D_800C20C8);
            }
        }
    }

    return 0;
}
