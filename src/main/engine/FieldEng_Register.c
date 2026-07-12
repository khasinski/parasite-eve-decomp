typedef unsigned char u8;
typedef signed short s16;

int printf(char *fmt);

extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20C8[];

int FieldEng_Register(char *obj, int (**handlers)(char *obj, void *entry, void *data)) {
    int i;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F3330 = obj + 0x200;
    D_800F33B0 = *(char **)(obj + 0x78);

    for (i = 0; (s16)i < 0x40; i++) {
        u8 *entry = D_800F34F4 + (s16)i * 6;

        if ((signed char)entry[1] == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers[entry[0]];

            if (handler != (void *)-1) {
                handler(obj, entry, D_800F3330 + *(s16 *)(entry + 4));
            } else {
                printf(D_800C20C8);
            }
        }
    }

    return 0;
}
