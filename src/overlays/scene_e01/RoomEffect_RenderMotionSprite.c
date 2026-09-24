/* Scene e01 motion sprite callback, retail offset 0x818. */
#include "common.h"
#include "pe1/room_fx.h"

typedef struct {
    u16 x;
    u16 y;
    u16 z;
    u16 pad;
    s16 blocked;
    u16 frame;
} MotionSpriteState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MotionSpriteState, blocked) == 0x08,
                  motion_sprite_blocked_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MotionSpriteState, frame) == 0x0A,
                  motion_sprite_frame_offset);

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} SpritePosition;

extern RoomFxSeed8 D_8018F004;
extern s32 D_800E27EC;
extern s32 D_800F3428;
extern u16 D_800F336C;
extern s16 D_800F336A;
extern u16 D_800E1204[];

u16 func_80077AA4(s32 x, s32 y);
void func_800CEE20(SpritePosition *position, void *rotation, s32 scale_x,
                   s32 scale_y, s32 height, s32 clut, s32 mode, s32 shade,
                   void *color);

int func_8018F800(int mode, MotionSpriteState *state) {
    SpritePosition position;
    RoomFxSeed8 seed = D_8018F004;
    int shade;
    int palette;
    int kind;
    u16 clut;

    switch (mode) {
    case 1: {
        int next;
        int signed_next;
        if (state->blocked) return 0;
        next = state->frame + 1;
        state->frame = next;
        /* Keep the halfword store before the signed range check. */
        asm volatile("" : "=r"(next) : "0"(next) : "memory");
        signed_next = (s16)next;
        /* Preserve the separate sign extension used by the retail build. */
        asm volatile("" : "=r"(signed_next) : "0"(signed_next));
        if (signed_next < 16) return 0;
        /* The two return paths must stay distinct. */
        asm volatile("" ::: "memory");
        return 1;
    }
    case 2:
        break;
    default:
        return 0;
    }
    if (state->blocked) return 0;

    shade = (D_800E27EC & 1) ? 128 : 100;
    position.x = state->x;
    position.y = state->y;
    position.z = state->z;

    if ((s16)state->frame < 8) {
        /* PIN-DEBT: the first palette selector remains in $v0. */
        register int first_kind asm("$2");
        first_kind = D_800F336C;
        palette = D_800E1204[first_kind];
        if (first_kind == 4 && D_800F3428) palette += 4;
        clut = func_80077AA4(16, palette);
        func_800CEE20(&position, &seed, 4096, 4096,
                        D_800F336A * (s16)state->frame + 32,
                        clut, 1, (unsigned)shade >> 1, 0);
    }

    position.y -= 40;
    kind = D_800F336C;
    palette = D_800E1204[kind];
    if (kind == 4 && D_800F3428) palette += 4;
    clut = func_80077AA4(48, palette);
    func_800CEE20(&position, 0, 4096, 4096,
                    D_800F336A * ((s16)state->frame / 2) + 96,
                    clut, 1, (unsigned)shade >> 1, 0);
    return 0;
}
