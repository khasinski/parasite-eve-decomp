extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A4E8[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);

typedef struct { short x, y, z, pad; } Position;
typedef struct { Position *position; short offset; } Effect;

int func_80194F04(int event, Effect *object)
{
    Position position;
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
        object->offset -= 16;
    } else if (event == 2) {
        int frame;
        int kind;
        int palette;
        int handle;
        register int special asm("$3");

        position = *object->position;
        position.y += object->offset;
        kind = D_800F336C;
        /* Preserve the counter read after the local position update. */
        asm("" : "=m"(D_800E27EC) : "m"(D_800E27EC), "m"(position.y));
        frame = D_800E27EC - 1;
        asm("" : "=r"(kind) : "0"(kind), "r"(frame));
        special = 4;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) {
            palette += 6;
        } else {
            palette += 2;
        }
        handle = GetClut(0, palette);
        func_800CEE20(&position, 0, 8192, 8192, D_800F336A * (frame >> 1),
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)frame << 7) & 0xF80] >> 5,
            D_8019A4E8);
    }
    return 0;
}
