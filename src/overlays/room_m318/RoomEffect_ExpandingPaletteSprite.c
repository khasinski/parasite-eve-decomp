#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM318Particle {
    u16 x;
    u16 y;
    u16 z;
} RoomM318Particle;

extern s32 D_800E27EC, D_800F3428;
extern s16 D_800F336A;
extern u16 D_800F336C, D_800E1204[];
extern char D_801995FC[];
extern int func_80071A54(void);
extern void func_800CF3AC(void *, void *, int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_80193FC4(int mode, RoomM318Particle *particle) {
    int color[2];
    GteShortVector position;
    int kind, palette, scale, frame, random, value;
    u16 clut;
    switch (mode) {
    case 1:
        random = func_80071A54();
        value = particle->x - 3;
        value += (random & 7);
        particle->x = value;
        random = func_80071A54();
        value = particle->z - 3;
        value += (random & 7);
        particle->z = value;
        random = func_80071A54();
        particle->y += random & 3;
        if (D_800E27EC >= 32) return 1;
        break;
    case 2:
        position.x = 0;
        position.y = 0;
        position.z = D_800E27EC * 12;
        position.pad = 0;
        func_800CF3AC(D_801995FC, color, D_800E27EC);
        kind = D_800F336C;
        scale = D_800E27EC * 128 + 4096;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        frame = D_800F336A * (D_800E27EC / 6 + 2) + 160;
        func_800CEE20(particle, &position, scale, scale, frame, clut, 1, 128, color);
        break;
    }
    return 0;
}
