extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80193A80(int event, void **object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int sample = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00));
        int size;
        int kind;
        int palette;
        int handle;
        register int special asm("$3");

        /* Keep the table sample, palette kind and scaled size in retail order. */
        asm("" : "=r"(sample) : "0"(sample) : "memory");
        kind = D_800F336C;
        asm("" : "=r"(sample), "=r"(kind) : "0"(sample), "1"(kind));
        size = sample * 3;
        asm("" : "=r"(size) : "0"(size), "r"(kind));
        special = 4;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) {
            palette += 8;
        } else {
            palette += 4;
        }
        handle = GetClut(0, palette);
        func_800CEE20(*object, 0, size, size, D_800F336A * 6 + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5, 0);
    }
    return 0;
}
