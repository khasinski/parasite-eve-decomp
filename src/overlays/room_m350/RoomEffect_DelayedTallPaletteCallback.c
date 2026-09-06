extern int D_800E27EC, D_800F3428, D_8019A3D0[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern short D_800966EE[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);

/* Signed high-half view preserves the retail LH instead of LHU plus extension. */
typedef struct { signed int unused : 16; signed int value : 16; } SignedHalf;

int func_80195218(int event, short *object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int frame = D_800E27EC - 2;
        if (frame >= 0) {
            int size;
            int kind;
            int palette;
            int handle;

            vector[0] = 1024;
            vector[1] = object[3];
            vector[2] = 1024;
            vector[3] = 1;
            size = D_800966EE[((unsigned int)frame << 7) & 0x1F80];
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) {
                palette += 4;
            }
            handle = GetClut(0, palette);
            func_800CEE20(object, vector, size, size * 2, D_800F336A * object[4] + 64,
                (unsigned short)handle, 1,
                ((SignedHalf *)((char *)D_800966EE - 2 +
                    (((unsigned int)frame << 8) & 0x3F00)))->value >> 5, D_8019A3D0);
        }
    }
    return 0;
}
