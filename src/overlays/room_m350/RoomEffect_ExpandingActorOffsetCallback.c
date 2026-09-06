typedef struct { char reserved[0xF4]; int position[3]; } Transform;
typedef struct { char reserved[0x238]; Transform *transform; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;

extern Actor *D_800F32D0;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A634[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80197A04(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int scale = (4096 - *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 9) & 0x3E00))) * 2;
        int *origin = D_800F32D0->instance->transform->position;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object[i] * scale / 4096 + origin[i];
        }
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(position, 0, 5120, 5120, D_800F336A * 2 + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A634);
    }
    return 0;
}
