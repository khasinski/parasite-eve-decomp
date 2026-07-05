typedef unsigned char u8;

extern int D_800A804C;
extern int D_800A8054;

extern int D_800A8044;

u8 *Tbl_FindNthNonEmpty(int arg0) {
    u8 *end;
    u8 *base;
    u8 *ptr;
    int offset;

    end = (u8 *)&D_800A8054;
    offset = *(int *)end;
    base = end - 0x2C;
    ptr = base + offset;
    if (arg0 >= 0) {
        arg0 = base[arg0 + D_800A804C];
    } else {
        arg0 = 0xF;
    }

    if (arg0 == 0) {
        return 0;
    }

    do {
        if (*ptr++ == 0) {
            arg0--;
        }
    } while (arg0 > 0);

    return ptr;
}

void *Str_ResolveDataPtr(void) {
    register char *base = (char *)&D_800A8044;
    register int offset = *(int *)base;

    base -= 0x1C;
    return (void *)(offset + base);
}
