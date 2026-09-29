#include "room_m318_effects.h"
#include "pe1/gte_types.h"

extern s32 D_800E27EC, D_800F3428;
extern s16 D_800F336A;
extern u16 D_800F336C, D_800E1204[];
extern int func_80077CF4(int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
extern void *D_8009D254;
extern u16 D_800942EC, D_800E11EA, D_800E2850[];
extern u16 D_800F3368, D_800F336E;
extern u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern void func_800CE870(void *, int, void *);
extern int func_800CE560(void *, int, int, void *);
extern RoomM318Spark *func_800CE610(void *);
extern int func_80071A54(void);

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

int func_80193B60(int mode, RoomM318Spark *spark) {
    RoomM318Spark *next;
    register int random asm("$2");
    register int index asm("$3");
    switch (mode) {
    case 0:
        func_800CE870(D_8009D254, 1, spark);
        spark->y = D_800942EC;
        return func_800CE560(D_800F33E0->link, 8, 12, func_801939AC);
    case 1:
        if (D_800E27EC < 40 && (D_800E27EC & 1)) {
            next = func_800CE610(D_800F33E0->link);
            if (next) {
                random = func_80071A54();
                next->x = spark->x + random % 400 - 200;
                next->y = spark->y - 800;
                random = func_80071A54();
                next->z = spark->z + random % 400 - 200;
                next->speed = (func_80071A54() & 7) + 42;
            }
        }
        if (D_800E27EC >= 70) return 1;
        break;
    case 2:
        index = D_800E11EA;
        D_800F3368 = 32;
        D_800F336A = 2;
        D_800F3376 = 32;
        D_800F3378 = 32;
        /* Keep the first four effect settings ahead of the palette lookup. */
        asm("" : : "r"(index) : "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 32;
        D_800F3370 = D_800E2850[index];
        break;
    }
    return 0;
}
