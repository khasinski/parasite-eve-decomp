#include "pe1/room_spark.h"
#include "pe1/room_sound_slot.h"
#ifndef ROOMEFFECT_RISING_SPRAY_FUNC
#error "ROOMEFFECT_RISING_SPRAY_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_RISING_SPRAY_CALLBACK
#error "ROOMEFFECT_RISING_SPRAY_CALLBACK must name the spark callback"
#endif
#ifndef ROOMEFFECT_RISING_SPRAY_OFFSET
#error "ROOMEFFECT_RISING_SPRAY_OFFSET must name the anchor offset vector"
#endif

/* Spray parameters: base launch speed and the heading spread. */
typedef struct RoomRisingSprayParams {
    s32 speed;
    s32 spread;
} RoomRisingSprayParams;

typedef RoomSoundSlot RoomRisingSpraySoundSlot;

extern RoomRisingSpraySoundSlot D_800B0E64_slot __asm__("D_800B0E64");
extern s32 D_800E27EC;
extern GteShortVector ROOMEFFECT_RISING_SPRAY_OFFSET;
extern int ROOMEFFECT_RISING_SPRAY_CALLBACK(int mode, RoomDampedSpark *spark);

/* Controller that sprays sparks from above the primary actor: a wide burst
 * on odd frames for the first second, then a stream along the actor's
 * heading, with two sound cues at frames 1 and 16. */
int ROOMEFFECT_RISING_SPRAY_FUNC(int mode, void *state,
                                 RoomRisingSprayParams *params) {
    GteShortVector offset = ROOMEFFECT_RISING_SPRAY_OFFSET;
    GteShortVector position;
    GteShortVector heading;
    GteShortVector angles;
    RoomDampedSpark *child;

    switch (mode) {
    case 0:
        if (D_800E2368->active) {
            RoomSparkNode **slot = (RoomSparkNode **)D_800F32D0->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x37,
                             ROOMEFFECT_RISING_SPRAY_CALLBACK);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 0, &offset, &position);
        position.y -= 0x50;
        func_800CE9D4((void *)D_800F32D0->pool, 0, &heading);
        heading.z = 0;
        heading.x = 0;
        if ((D_800E27EC & 1) && D_800E27EC < 0x3C) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                angles.x = heading.x;
                angles.y = heading.y;
                angles.z = heading.z;
                angles.x += (func_80071A54() & 0xFF) - 0x80;
                angles.y += (func_80071A54() & 0x3FF) - 0x200;
                func_800CFB7C(&angles, (func_80071A54() & 0xF) | 0x10,
                              (GteShortVector *)&child->vx);
                child->state = 2;
                child->timer = 0;
            }
        }
        if (D_800E27EC == 1) {
            RoomRisingSpraySoundSlot *sound = &D_800B0E64_slot;
            if (sound->channel) {
                func_8006DF50(sound->channel, 0x599, func_800D3FD8(), 0x80, 0x7F);
                if (sound->channel) func_8006DF50(sound->channel, 0x5B5, 0x80, 0x80, 0x7F);
            }
        }
        if (D_800E27EC == 0x10) {
            RoomRisingSpraySoundSlot *sound = &D_800B0E64_slot;
            if (sound->channel) {
                func_8006DF50(sound->channel, 0x5B6, func_800D3FD8(), 0x80, 0x7F);
                if (sound->channel) func_8006DF50(sound->channel, 0x5B7, 0x80, 0x80, 0x7F);
            }
        }
        if ((u32)(D_800E27EC - 0x10) < 0x2D) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                heading.y += func_80071A54() % params->spread - params->spread / 2;
                func_800CFB7C(&heading, (s16)(params->speed + (func_80071A54() & 0xF)),
                              (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_RISING_SPRAY_FUNC
#undef ROOMEFFECT_RISING_SPRAY_CALLBACK
#undef ROOMEFFECT_RISING_SPRAY_OFFSET
