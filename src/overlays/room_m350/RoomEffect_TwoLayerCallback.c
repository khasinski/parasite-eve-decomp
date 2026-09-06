extern int D_800E27EC, D_800966EC[], D_8019A3C8[], D_8019A4DC[], D_8019A4E0[];
extern void func_800D004C(void *, int, int, int, short *, int, int, void *, void *, int, int);
extern void func_800D0728(void *, int, int, int, short *, int, int, void *, void *, int, int);

typedef struct { signed int size : 16; signed int shade : 16; } TableEntry;

int func_801944F0(int event, void **object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int *base = D_800966EC;
        int counter = D_800E27EC;
        TableEntry *entry = (TableEntry *)((char *)base +
            (((unsigned int)(counter - 1) << 9) & 0x3E00));
        int size = entry->size;

        vector[0] = 0;
        vector[1] = 0;
        vector[2] = counter * 128;
        vector[3] = 0;
        func_800D004C(*object, 320, 320, 16, vector, size, size,
            D_8019A4DC, D_8019A3C8, entry->shade >> 5, 1);
        {
            int nextCounter = D_800E27EC;
            size = nextCounter * 512 + 2048;
            func_800D0728(*object, 256, 512, 16, vector, size, size,
                D_8019A3C8, D_8019A4E0,
                (short)base[((unsigned int)(nextCounter - 1) << 8) & 0xF00] >> 5, 1);
        }
    }
    return 0;
}
