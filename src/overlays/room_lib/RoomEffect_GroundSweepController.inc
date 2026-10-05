#include "pe1/room_spark.h"
#include "pe1/gte.h"
#include "pe1/room_ground_sweep.h"
#include "pe1/room_sound_slot.h"
#ifndef ROOMEFFECT_GROUND_SWEEP_FUNC
#error "ROOMEFFECT_GROUND_SWEEP_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_GROUND_SWEEP_CALLBACK
#error "ROOMEFFECT_GROUND_SWEEP_CALLBACK must name the scorch mark callback"
#endif
#ifndef ROOMEFFECT_GROUND_SWEEP_OFFSET
#error "ROOMEFFECT_GROUND_SWEEP_OFFSET must name the hand offset vector"
#endif
#ifndef ROOMEFFECT_GROUND_SWEEP_COLOR_A
#error "ROOMEFFECT_GROUND_SWEEP_COLOR_A must name the first ribbon colour"
#endif
#ifndef ROOMEFFECT_GROUND_SWEEP_COLOR_B
#error "ROOMEFFECT_GROUND_SWEEP_COLOR_B must name the second ribbon colour"
#endif
#ifndef ROOMEFFECT_GROUND_SWEEP_SIDE
#error "ROOMEFFECT_GROUND_SWEEP_SIDE must name the alternating side counter"
#endif

typedef RoomSoundSlot RoomGroundSweepSoundSlot;

extern RoomGroundSweepSoundSlot D_800B0E64_slot __asm__("D_800B0E64");
extern int func_8006DF50_handle(void *channel, int id, int value, int volume,
                                int pan) __asm__("func_8006DF50");
extern RoomGroundSweepBattleEntity *D_8009D254;
extern s32 D_800E27EC;
extern s32 ROOMEFFECT_GROUND_SWEEP_SIDE;
extern GteShortVector ROOMEFFECT_GROUND_SWEEP_OFFSET;
extern RenderColor ROOMEFFECT_GROUND_SWEEP_COLOR_A;
extern RenderColor ROOMEFFECT_GROUND_SWEEP_COLOR_B;
extern int ROOMEFFECT_GROUND_SWEEP_CALLBACK(int mode, RoomGroundSweepMark *mark);
extern int func_800C6B90(void *position, int radius);
extern void func_800866A4(int handle, int arg);
extern void func_800CFAA8(void *from, void *to, void *angles);
extern void func_800CFD50(void *angles, void *out, int speed);
extern void func_800783E4(void *from, void *to, int weightFrom, int weightTo,
                          void *out);
extern void func_800D2B58(void *, void *, void *, void *, int, int, int);
extern void func_800D1384(void *from, void *to, int width, void *color0,
                          void *color1, int alpha, void *trail, int mode);

/* Controller that sweeps a ribbon of light along the ground from the
 * actor's hand: it rises, lunges out and back a few times leaving scorch
 * marks and hitting the player in reach, then fades out. */
int ROOMEFFECT_GROUND_SWEEP_FUNC(int mode, RoomGroundSweep *sweep,
                                 RoomGroundSweepParams *params) {
    GteShortVector offset = ROOMEFFECT_GROUND_SWEEP_OFFSET;
    GteShortVector hand;
    GteShortVector step;
    GteShortVector work;          /* hand rotation, then the blended tip */
    GteShortVector angles;
    RenderColor colorA = ROOMEFFECT_GROUND_SWEEP_COLOR_A;
    RenderColor colorB = ROOMEFFECT_GROUND_SWEEP_COLOR_B;
    RoomGroundSweepMark *mark;
    int scale;

    switch (mode) {
    case 0:
        ROOMEFFECT_GROUND_SWEEP_SIDE = 2 - ROOMEFFECT_GROUND_SWEEP_SIDE;
        work.y = 0;
        work.x = (ROOMEFFECT_GROUND_SWEEP_SIDE - 1) * 350;
        work.z = -(func_80071A54() & 0xFF) - 400;
        func_800CE8F0(D_800F32D0->pool, 4, &work, sweep);
        sweep->y = D_800942EC.count;
        sweep->x += (func_80071A54() & 0x1F) - 0x10;
        sweep->z += (func_80071A54() & 0x1F) - 0x10;
        sweep->heading.x = 0;
        sweep->heading.y = func_80071A54() & 0xFFF;
        sweep->heading.z = 0;
        func_800CE8F0(D_800F32D0->pool, 4, &offset, &hand);
        func_800D1384(&hand, sweep, 0x3F0, &colorA, &colorA, 0x8C, sweep->trail, 1);
        if (D_800E2368->active) {
            RoomSparkNode **slot = (RoomSparkNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        sweep->phase = 0;
        sweep->timer = 0;
        sweep->sweeps = 8;
        return func_800CE560(D_800F33E0->pool, 0xC, 0x10,
                             ROOMEFFECT_GROUND_SWEEP_CALLBACK);
    case 1:
        func_800CE870((char *)D_8009D254, 1, (s16 *)&hand);
        hand.y = D_800942EC.count;
        switch (sweep->phase) {
        case 0:
            sweep->timer++;
            if ((s16)sweep->timer == 2) {
                RoomGroundSweepSoundSlot *sound = &D_800B0E64_slot;
                sweep->soundHandle = sound->channel
                    ? func_8006DF50_handle(sound->channel, 0x598, func_800D3FD8(), 0x80, 0x7F)
                    : 0;
            }
            if ((s16)sweep->timer < 4) break;
            sweep->phase = 1;
            sweep->timer = 0;
            break;
        case 1: {
            int half;
            sweep->timer++;
            scale = func_80077CF4(((s16)sweep->timer << 11) / 12);
            half = params->distance / 2;
            scale = scale * half / 4096;
            scale += half;
            func_800CFB7C(&sweep->heading, (s16)scale, &step);
            sweep->x += step.x;
            sweep->y += step.y;
            sweep->z += step.z;
            sweep->y = D_800942EC.count;
            mark = (RoomGroundSweepMark *)func_800CE610(D_800F33E0->pool);
            if (mark) {
                mark->x = sweep->x;
                mark->y = sweep->y;
                mark->z = sweep->z;
                mark->blocked = 0;
                mark->frame = 0;
            }
            if (func_800C6B90(sweep, 50)) {
                if (D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if ((((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags =
                            (((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x3D000000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
            }
            if ((s16)sweep->timer < 12) break;
            sweep->phase = 2;
            if (--sweep->sweeps <= 0) sweep->phase = 3;
            sweep->timer = 0;
            break;
        }
        case 2:
            sweep->timer++;
            scale = (s16)sweep->timer * 650;
            func_800CFAA8(sweep, &hand, &angles);
            angles.x = 0;
            func_800CFD50(&angles, &sweep->heading, (u16)scale);
            func_800CFB7C(&sweep->heading, (s16)(params->distance / 2), &step);
            sweep->x += step.x;
            sweep->y += step.y;
            sweep->z += step.z;
            sweep->y = D_800942EC.count;
            mark = (RoomGroundSweepMark *)func_800CE610(D_800F33E0->pool);
            if (mark) {
                mark->x = sweep->x;
                mark->y = sweep->y;
                mark->z = sweep->z;
                mark->blocked = 0;
                mark->frame = 0;
            }
            if (func_800C6B90(sweep, 50)) {
                if (D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if ((((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags =
                            (((RoomGroundSweepHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x3D000000;
                        ((RoomGroundSweepHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
            }
            if ((s16)sweep->timer < 4) break;
            sweep->phase = 1;
            if (--sweep->sweeps <= 0) sweep->phase = 3;
            sweep->timer = 0;
            break;
        case 3:
            sweep->timer++;
            if ((s16)sweep->timer < 8) break;
            sweep->phase = 4;
            sweep->timer = 0;
            if (sweep->soundHandle != -1) func_800866A4(sweep->soundHandle, 0);
            break;
        case 4:
            sweep->timer++;
            if ((s16)sweep->timer < 0x39) break;
            return 2;
        }
        break;
    case 2: {
        RenderColor *color;
        func_800CE8F0(D_800F32D0->pool, 4, &offset, &hand);
        if (D_800E27EC & 1) color = &colorA;
        else color = &colorB;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.depth = 0xC;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (sweep->phase) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            scale = (s16)sweep->timer << 10;
            func_800783E4(&hand, sweep, 0x1000 - scale, scale, &work);
            func_800D2B58(&hand, &work, color, color, 0x80, 0, 1);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&hand, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            func_800D2B58(&hand, sweep, color, color, 0x8C, 0x8C, 1);
            func_800D1384(&hand, sweep, 8, color, &colorA, 0x8C, sweep->trail, 1);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&hand, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            func_800D2B58(&hand, sweep, color, color, 0x60, 0x60, 1);
            func_800D1384(&hand, sweep, 8, color, &colorA, 0x8C, sweep->trail, 1);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&hand, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, 0x80, 0);
            break;
        }
        case 3: {
            u16 clut;
            int kind;
            int palette;
            int fade = func_80077DC4((s16)sweep->timer << 7) / 32;
            func_800D2B58(&hand, sweep, color, color, fade, fade, 1);
            func_800D1384(&hand, sweep, 8, color, &colorA, 0x8C, sweep->trail, 1);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&hand, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (sweep->timer & 1) + 0x40,
                          clut, 1, fade, 0);
            break;
        }
        case 4:
            break;
        }
        break;
    }
    }
    return 0;
}

#undef ROOMEFFECT_GROUND_SWEEP_FUNC
#undef ROOMEFFECT_GROUND_SWEEP_CALLBACK
#undef ROOMEFFECT_GROUND_SWEEP_OFFSET
#undef ROOMEFFECT_GROUND_SWEEP_COLOR_A
#undef ROOMEFFECT_GROUND_SWEEP_COLOR_B
#undef ROOMEFFECT_GROUND_SWEEP_SIDE
