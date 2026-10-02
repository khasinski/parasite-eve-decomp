#include "pe1/room_sound_burst.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_FLASH_SPRITE_FUNC
#error "ROOMEFFECT_FLASH_SPRITE_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_FLASH_SPRITE_ROTATION
#error "ROOMEFFECT_FLASH_SPRITE_ROTATION must name the rotation seed"
#endif
#ifndef ROOMEFFECT_FLASH_SPRITE_CALLBACK
#error "ROOMEFFECT_FLASH_SPRITE_CALLBACK must name the particle callback"
#endif
#ifndef ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE
#error "ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE must name the color ramp table"
#endif

extern GteRotation ROOMEFFECT_FLASH_SPRITE_ROTATION;
extern void ROOMEFFECT_FLASH_SPRITE_CALLBACK(void);
extern u8 ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE[];
extern RoomSoundBurstParams D_800F3368;
extern volatile u16 D_800E11EC;

typedef struct RoomFlashSpriteState {
    s16 x, y, z, reserved06;      /* 0x00: drawn position */
    s16 ax;                       /* 0x08: anchor */
    u16 ay;                       /* 0x0A */
    s16 az;                       /* 0x0C */
    s16 reserved0E;
    s16 state;                    /* 0x10 */
    u16 frame;                    /* 0x12 */
    s16 attachment;               /* 0x14 */
    s16 soundHandle;              /* 0x16 */
} RoomFlashSpriteState;

typedef struct RoomFlashSpriteSpawn {
    s32 reserved[2];
    s32 lift;                     /* 0x08 */
} RoomFlashSpriteSpawn;

typedef struct RoomFlashSpriteChild {
    s16 x, y, z, reserved06;
    s16 wx, wy, wz, reserved0E;
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
    s16 soundHandle;              /* 0x14 */
} RoomFlashSpriteChild;

/* Controller that anchors a flash to the actor, plays a sound, waits sixteen
 * frames, releases two particles, then draws a pulsing sprite that blends
 * into the actor position. */
int ROOMEFFECT_FLASH_SPRITE_FUNC(int mode, RoomFlashSpriteState *state,
                                 RoomFlashSpriteSpawn *spawn) {
    GteRotation rotation = ROOMEFFECT_FLASH_SPRITE_ROTATION;
    GteShortVector target;
    GteShortVector output;
    RoomSoundBurstColor color;
    RoomFlashSpriteChild *child;
    RoomSoundBurstMatrixSlot *matrixSlot;
    char *pool;
    int handle;
    int kind;
    int palette;
    u16 clut;
    int scale;
    int blend;
    int opacity;
    int frame;
    int unit;

    switch (mode) {
    case 0:
        pool = D_800F32D0->pool;
        func_800CE9D4(pool, 0, &state->ax);
        state->soundHandle = -1;
        switch (D_800E2368->phase) {
        case 0:
            state->attachment = 0x20;
            state->ay -= spawn->lift;
            handle = func_800D3FD8();
            handle = func_800D3F64(0x58D, handle);
            state->soundHandle = handle;
            break;
        case 1:
            state->attachment = 0x16;
            break;
        case 2:
            state->attachment = 0x1B;
            state->ay += spawn->lift;
            break;
        }
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, state->attachment, &rotation, state);
        state->y = D_800942EC;
        func_800CFB7C(&state->ax, 0x12C, &target);
        state->x += target.x;
        state->y += target.y;
        state->z += target.z;
        if (D_800E2368->active) {
            RoomSoundBurstNode **slot = (RoomSoundBurstNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->state = 0;
        state->frame = 0;
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 24, 16, ROOMEFFECT_FLASH_SPRITE_CALLBACK);
    case 1:
        switch (state->state) {
        case 0:
            state->frame++;
            if ((s16)state->frame < 16) break;
            state->state = 1;
            state->frame = 0;
            break;
        case 1:
            state->frame++;
            if ((s16)state->frame < 4) break;
            state->state = 2;
            state->frame = 0;
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = state->x;
                child->y = state->y;
                child->z = state->z;
                child->wx = state->ax;
                child->wy = state->ay;
                child->wz = state->az;
                child->wx = 0;
                child->state = 0;
                child->timer = 0;
                child->soundHandle = state->soundHandle;
            }
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = state->x;
                child->y = state->y;
                child->z = state->z;
                child->state = 1;
                child->timer = 0;
            }
            break;
        case 2:
            state->frame++;
            if ((s16)state->frame < 4) break;
            return 2;
        }
        break;
    case 2:
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, state->attachment, &rotation, &target);
        {
            int index;
            int tpage;
            unit = 0x20;
            index = D_800E11EC;
            D_800F3368.parameter00 = unit;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = unit;
            D_800F3368.extent_y = unit;
            tpage = D_800E2850[index];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0xC;
            D_800F3368.tpage = tpage;
        }
        switch (state->state) {
        case 0:
            func_80077CF4((s16)state->frame << 6);
            opacity = (state->frame & 1) ? 0x40 : 0x80;
            scale = func_80077CF4((s16)state->frame << 6);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20(&target, 0, scale, scale, 0x20, clut, 1, opacity, 0);
            break;
        case 1:
            blend = (s16)state->frame << 10;
            func_800783E4(&target, state, 0x1000 - blend, blend, &output);
            func_800CF3AC(ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE, &color,
                          (s16)state->frame);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2B58(&target, &output, &color, &color, 0, 0x80, 1);
            break;
        case 2:
            frame = (s16)state->frame;
            if (frame >= 5) break;
            func_800CF3AC(ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE, &color, frame + 4);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2B58(&target, state, &color, &color, 0, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_FLASH_SPRITE_FUNC
#undef ROOMEFFECT_FLASH_SPRITE_ROTATION
#undef ROOMEFFECT_FLASH_SPRITE_CALLBACK
#undef ROOMEFFECT_FLASH_SPRITE_COLOR_TABLE
