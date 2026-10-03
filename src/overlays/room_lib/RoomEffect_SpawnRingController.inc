#include "pe1/room_sound_burst.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_SPAWN_RING_FUNC
#error "ROOMEFFECT_SPAWN_RING_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_SPAWN_RING_ROTATION
#error "ROOMEFFECT_SPAWN_RING_ROTATION must name the rotation seed"
#endif
#ifndef ROOMEFFECT_SPAWN_RING_SHADE
#error "ROOMEFFECT_SPAWN_RING_SHADE must name the ring shade"
#endif
#ifndef ROOMEFFECT_SPAWN_RING_CALLBACK
#error "ROOMEFFECT_SPAWN_RING_CALLBACK must name the particle callback"
#endif
#ifndef ROOMEFFECT_SPAWN_RING_COLOR_TABLE
#error "ROOMEFFECT_SPAWN_RING_COLOR_TABLE must name the color ramp table"
#endif

extern GteRotation ROOMEFFECT_SPAWN_RING_ROTATION;
extern RoomSoundBurstParams D_800F3368;
extern RoomSoundBurstColor ROOMEFFECT_SPAWN_RING_SHADE;
extern void ROOMEFFECT_SPAWN_RING_CALLBACK(void);
extern u8 ROOMEFFECT_SPAWN_RING_COLOR_TABLE[];
extern u16 D_800E11EC;

typedef struct RoomSpawnRingState {
    s16 state;                    /* 0x00 */
    s16 timer;                    /* 0x02 */
    GteShortVector target;        /* 0x04 */
} RoomSpawnRingState;

typedef struct RoomSpawnRingChild {
    s16 x, y, z, reserved06;
    s16 dx, dy, dz, reserved0E;
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
} RoomSpawnRingChild;

/* Controller that plays a sound at the actor, waits twenty frames, then
 * emits randomized particles around the actor for thirty-two frames while
 * drawing a pulsing sprite with two expanding rings. */
int ROOMEFFECT_SPAWN_RING_FUNC(int mode, RoomSpawnRingState *state) {
    GteRotation rotation = ROOMEFFECT_SPAWN_RING_ROTATION;
    GteShortVector target;
    RoomSoundBurstColor color;
    RoomSoundBurstColor shade = ROOMEFFECT_SPAWN_RING_SHADE;
    RoomSpawnRingChild *child;
    RoomSoundBurstMatrixSlot *matrixSlot;
    char *pool;
    int handle;
    int kind;
    int palette;
    u16 clut;
    int width;
    int height;
    int intensity;
    int idx;
    int unit;
    u16 *tpageSlot;

    switch (mode) {
    case 0:
        func_800CE870(D_8009D254, 0, &state->target);
        handle = func_800D3FD8();
        func_800D3F64(0x59E, handle);
        state->state = 0;
        state->timer = 0;
        if (D_800E2368->active) {
            RoomSoundBurstNode **slot = (RoomSoundBurstNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 24, ROOMEFFECT_SPAWN_RING_CALLBACK);
    case 1:
        switch (state->state) {
        case 0:
            state->timer++;
            if ((s16)state->timer < 20) break;
            state->state = 1;
            state->timer = 0;
            break;
        case 1:
            state->timer++;
            pool = D_800F33E0->pool;
            child = (RoomSpawnRingChild *)func_800CE610(pool);
            if (child) {
                child->x = state->target.x;
                child->y = state->target.y;
                child->z = state->target.z;
                child->x += (func_80071A54() & 0xFF) - 0x80;
                child->z += (func_80071A54() & 0xFF) - 0x80;
                child->y += (func_80071A54() & 0x1FF) - 0x100;
                child->dx = func_80071A54() % 32 - 16;
                child->dy = func_80071A54() % 32 - 16;
                child->dz = func_80071A54() % 32 - 16;
                child->state = 0;
                child->timer = 0;
            }
            if (func_800C6B90(&state->target, 180)) {
                if (D_800E2368->active) {
                    RoomSoundBurstChannel *channel = D_800F32D0;
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x21000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
            }
            if (state->timer < 32) break;
            state->state = 2;
            break;
        case 2:
            return 2;
        }
        break;
    case 2:
        {
            int index;
            int tpage;
            unit = 0x20;
            index = D_800E11EC;
            D_800F3368.parameter02 = 2;
            D_800F3368.parameter00 = unit;
            D_800F3368.extent_x = unit;
            D_800F3368.extent_y = unit;
            D_800F3368.extent_x = 0x100;
            D_800F3368.extent_y = unit;
            tpage = D_800E2850[index];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = unit;
            D_800F3368.tpage = tpage;
        }
        switch (state->state) {
        case 0:
            pool = D_800F32D0->pool;
            func_800CE8F0(pool, 16, &rotation, &target);
            width = rsin((state->timer << 10) / 20);
            height = rcos((state->timer << 10) / 20) / 2;
            func_800CF3AC(ROOMEFFECT_SPAWN_RING_COLOR_TABLE, &color, state->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20(&target, 0, width, height, 0, clut, 1, 0x80, &color);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            intensity = rcos((state->timer << 10) / 20) / 64;
            func_800D004C(&target, 0x44C, 0x44C, 16, 0, 0x1000, 0x1000, &shade,
                          0, intensity, 1);
            func_800D004C(&target, 0x1F4, 0x1F4, 8, 0, 0x1000, 0x1000, &shade,
                          0, intensity, 1);
            break;
        case 1:
            break;
        case 2:
            break;
        }
        /* Retail addresses the closing stores and the unit read through a
         * register holding the tpage slot; the empty constraint keeps the
         * compiler from folding it back into absolute addresses. */
        tpageSlot = &D_800F3368.tpage;
        asm("" : "=r"(tpageSlot) : "0"(tpageSlot));
        idx = D_800E11EC;
        unit = tpageSlot[-4];
        palette = D_800E2850[idx];
        D_800F3368.palette = 4;
        D_800F3368.parameter06 = 0;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        tpageSlot[1] = 0;
        tpageSlot[2] = 0x10;
        tpageSlot[0] = palette;
        break;
    }
    return 0;
}

#undef ROOMEFFECT_SPAWN_RING_FUNC
#undef ROOMEFFECT_SPAWN_RING_ROTATION
#undef ROOMEFFECT_SPAWN_RING_SHADE
#undef ROOMEFFECT_SPAWN_RING_CALLBACK
#undef ROOMEFFECT_SPAWN_RING_COLOR_TABLE
