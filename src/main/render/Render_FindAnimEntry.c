typedef unsigned char u8;
typedef short s16;
typedef int s32;

int Render_FindAnimEntry(void *arg0, int arg1, s32 *out) {
    char unused[8];
    char *base;
    register char *entry asm("$3");
    s32 i;
    s32 tmp;

    tmp = *(u8 *)(*(char **)arg0 + 3);
    base = *(char **)((char *)arg0 + 0x80);
    i = 0;
    if (tmp > 0) {
        tmp = arg1 << 16;
        arg1 = tmp >> 16;
        entry = base + 4;
        do {
            tmp = *(s16 *)(entry + 2);
            i++;
            if (tmp == arg1) {
                out[0] = *(s16 *)(base + 0);
                out[1] = *(s16 *)(entry - 2);
                out[2] = *(s16 *)(entry + 0);
                return 1;
            }
            entry += 0xC;
            tmp = *(u8 *)(*(char **)arg0 + 3);
            base += 0xC;
        } while (i < tmp);
    }

    return 0;
}
