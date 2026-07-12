typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;

extern u8 *D_800F34F4;
extern char *D_800E2248;

void func_800C2D0C(u16 slot, u8 id, u16 size) {
    u8 *entry = D_800F34F4 + slot * 6;
    s16 offset = *(s16 *)(D_800E2248 + 4);

    entry[1] = 1;
    entry[0] = id;
    *(u16 *)(entry + 2) = 0;

    if (offset + size >= 0x80C) {
        offset = 0;
    }

    *(s16 *)(entry + 4) = offset;
    *(s16 *)(D_800E2248 + 4) = offset + size;
    D_800E2248[6]++;
}
