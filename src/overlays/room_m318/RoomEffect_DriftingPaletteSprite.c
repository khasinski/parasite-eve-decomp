#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM318Spark {
    u16 x, y, z, speed;
} RoomM318Spark;

extern s32 D_800E27EC, D_800F3428;
extern s16 D_800F336A;
extern u16 D_800F336C, D_800E1204[];
extern int func_80077CF4(int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_801939AC(int mode, RoomM318Spark *spark) {
    GteShortVector origin;
    GteShortVector position;
    /* Preserve the retail register assignment for the angle calculation. */
    register int frame asm("$4");
    register int numerator asm("$5");
    int offset, color, kind, palette, brightness;
    u16 clut;
    switch (mode) {
    case 1:
        spark->y += spark->speed;
        if (D_800E27EC < 19) spark->speed--;
        if (D_800E27EC >= 24) return 1;
        break;
    case 2:
        frame = D_800E27EC;
        offset = frame << 5;
        numerator = frame << 11;
        origin.x = spark->x;
        origin.y = spark->y;
        origin.z = spark->z;
        position.x = 0;
        position.y = 0;
        position.z = spark->y + spark->x + offset;
        position.pad = 0;
        color = func_80077CF4(numerator / 24);
        brightness = color / 128;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&origin, &position, 4096, 4096,
                      D_800F336A * (D_800E27EC / 6) + 96, clut, 1, brightness, 0);
        break;
    }
    return 0;
}
