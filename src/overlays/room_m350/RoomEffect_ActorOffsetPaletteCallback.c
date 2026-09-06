typedef struct { char reserved[0x434]; int position[3]; } Transform;
typedef struct { char reserved[0x238]; Transform *transform; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;

extern Actor *D_800F32D0;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A56C[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

int func_80195B64(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int entry = D_800966EC[((unsigned int)D_800E27EC << 7) & 0xF80];
        int scale = entry >> 16;
        int shade = (short)entry >> 5;
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
        func_800CEE20(position, 0, object[3], object[3], D_800F336A + 216,
            (unsigned short)handle, 1, shade, D_8019A56C);
    }
    return 0;
}
