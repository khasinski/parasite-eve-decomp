typedef unsigned char u8;
typedef signed short s16;

int FieldEng_GetStatus(char *obj);

extern char *D_800E2248;
extern u8 *D_800F34F4;
extern char *D_800F32A8;
extern char *D_800F3330;

void **FieldEng_GetSlot(char *obj) {
    char *p;
    unsigned int i;
    int offset;

    for (p = obj + 0xC; p < obj + 0xA0C; p++) {
        *p = 0;
    }

    obj[2] = 0;
    obj[3] = 0;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;

    *(s16 *)(obj + 0xC) = 0;
    *(s16 *)(obj + 0xE) = 0;
    *(s16 *)(obj + 0x10) = 0;

    offset = 0;
    for (i = 0; i < 0x40; i++) {
        D_800F34F4[offset] = 0;
        D_800F34F4[offset + 1] = 0;
        *(s16 *)(D_800F34F4 + offset + 2) = 0;
        *(s16 *)(D_800F34F4 + offset + 4) = 0;
        offset += 6;
    }

    if (FieldEng_GetStatus(obj) == 3) {
        char *inner = **(char ***)(obj + 8);

        *(int *)inner = (*(int *)inner & 0xC0FFFFFF) | 0x01000000;
    }

    return (void **)(D_800E2248 + 0x6C);
}
