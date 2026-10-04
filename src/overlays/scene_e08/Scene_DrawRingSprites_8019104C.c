#include "pe1/scene_ring_sprites.h"

/* Draw the flash sprite while the state is younger than 12 ticks, then the
 * glow sprite and the two shaded rings, all at the state's position. The
 * untyped data argument is read through two typed views and then reused for
 * the matrix pointer of the second half, as retail's register use shows
 * (the incoming copy serves the flash block, a second copy the rest). */
void func_8019104C(void *object, void *timer, void *data)
{
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    GteVector ringScale;
    GteVector source;
    SceneRingSpritesOwner *owner;
    SceneRingSpritesState *ring;
    SceneRingSpritesState *state;

    owner = func_800C2B50();
    rotation = D_8018F018;
    ring = data;
    state = data;
    func_800C2EAC(owner->mode);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C2FF0(0x20, 0x20);
    if (ring->ticks < 12) {
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;
        scale = D_8018F020;
        func_80078CC4(&matrix, &scale);
        matrix.t[0] = ring->x;
        matrix.t[1] = ring->y;
        matrix.t[2] = ring->z;
        D_801995A8.depth = ring->depth;
        D_801995A8.texture = (ring->ticks % 12 & 0xFE) - 0x5C;
        func_800C42A4(&D_801995A8, &matrix, 1);
    }
    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;
    scale = D_8018F030;
    data = &matrix;
    func_80078CC4(data, &scale);
    matrix.t[0] = state->x;
    matrix.t[1] = state->y;
    matrix.t[2] = state->z;
    D_801995B8.depth = state->depth >> 2;
    func_800C42A4(&D_801995B8, data, 1);
    func_800794C4(&rotation, data);
    func_80071A44(&source, 0, 0x10);
    source.x = state->size;
    source.y = state->size;
    source.z = state->size;
    ringScale = source;
    func_80078CC4(data, &ringScale);
    matrix.t[0] = state->x;
    matrix.t[1] = state->y;
    matrix.t[2] = state->z;
    D_801995C8[0].depth = state->depth;
    func_800C4FC4(&D_801995C8[0], data, 0);
    D_801995C8[1].depth = state->depth;
    func_800C4FC4(&D_801995C8[1], data, 0);
}
