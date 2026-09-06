extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A3D0[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

typedef struct {
    void *position;
    short scale;
} Effect;

int func_80192ADC(int event, Effect *object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int size = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)) * object->scale / 4096;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;

        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(object->position, 0, size, size, D_800F336A + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A3D0);
    }
    return 0;
}
