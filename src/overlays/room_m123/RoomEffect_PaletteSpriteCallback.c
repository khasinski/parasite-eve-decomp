#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM123Particle {
    u16 frame;
    u16 offset;
    s16 scale;
} RoomM123Particle;

extern s16 D_801956A4, D_801956A0, D_800F336A;
extern u16 D_800F336C, D_800E1204[];
extern int D_800F3428;
extern char D_801956B0[], D_801956A8[];
extern void func_800CF844(void *, void *, int, void *, int, int);
extern u16 GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_801945EC(int mode, RoomM123Particle *particle) {
    GteShortVector position;
    int textureFrame, scale, kind, palette;
    u16 clut;

    switch (mode) {
    case 1: {
        int frame = particle->frame;
        int speed = D_801956A4;
        int shift = speed * 24;

        frame++;
        particle->frame = frame;
        particle->offset += shift / 4096;
        if ((s16)particle->frame >= 24) return 1;
    } break;
    case 2: {
        int sampleFrame;

        scale = D_801956A4 * particle->scale;
        sampleFrame = (s16)particle->frame;
        func_800CF844(D_801956B0, &position, sampleFrame << 6, D_801956A8,
                      scale / 4096, (s16)particle->offset);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0x20, palette);
        textureFrame = D_800F336A * (particle->frame & 7) + 0x40;
        func_800CEE20(&position, 0, 0x555, 0x555, textureFrame, clut, 1,
                      D_801956A0, 0);
    } break;
    }
    return 0;
}
