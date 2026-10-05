#include "room_m123_effects.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/room_sound_slot.h"

extern GteShortVector D_8018F1F0;
extern GteMatrix *D_800BCFA4;
extern int D_800E27EC;
extern void func_800CF3AC(void *, void *, int);
extern int func_80077CF4(int);
extern void func_800D0728(void *, int, int, int, void *, int, int, int, void *, int, int);

int func_80194A70(int mode, RoomPulseParticle *particle, int *reference)
{
    int color[2];
    GteShortVector position = D_8018F1F0;
    GteShortVector *positionPtr;
    GteMatrix **matrixSlot;
    int wave;

    positionPtr = &position;
    switch (mode) {
    case 1:
        if (particle->state != 0)
            break;
        particle->frame++;
        if (particle->frame >= 24)
            return 1;
        break;
    case 2:
        if (particle->state != 0)
            break;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800CF3AC((void *)*reference, color, particle->frame);
        wave = func_80077CF4((D_800E27EC << 10) / 24);
        func_800D0728(particle, 1000, 1500, 24, positionPtr, wave, wave, 0, color, 128, 1);
        break;
    }
    return 0;
}

typedef struct RoomPulseObject RoomPulseObject;
typedef struct RoomPulsePool {
    RoomPulseObject *object;
} RoomPulsePool;

struct RoomPulseObject {
    u32 flags;
    u8 pad[0x14];
    u8 *status;
};

typedef struct RoomPulseGlobal {
    u8 pad[0x4C];
    u32 flags;
} RoomPulseGlobal;

extern RoomPulseGlobal **D_8009D254;
extern u8 *D_800E2368;
extern int D_800E27EC;
/* Frame counter read as a one-field record so the copy stores stay ahead. */
typedef struct RoomM123FrameCounter {
    u16 count;
} RoomM123FrameCounter;
extern RoomM123FrameCounter D_800942EC;
extern u16 D_800E11EA, D_800E2850[];
/* The sprite parameter block at 0x800F3368 (RenderEffectParameters in
 * pe1/render_object.h, whose prototypes conflict with this unit's). */
typedef struct RoomM123EffectParameters {
    u16 parameter00;
    u16 parameter02;
    u16 palette;
    u16 parameter06;
    u16 tpage;
    u16 parameter0A;
    s16 depth;
    u16 extent_x;
    u16 extent_y;
} RoomM123EffectParameters;
extern RoomM123EffectParameters D_800F3368;
extern int func_80194A70(int, RoomPulseParticle *, int *);
extern void func_800CE870(void *, int, void *);
extern int func_800CE560(void *, int, int, int (*)(int, RoomPulseParticle *, int *));
extern RoomPulseParticle *func_800CE610(void *);
extern int func_800D3FD8(void);
extern void func_8006DF50(void *, int, int, int, int);

int func_80194C04(int mode, RoomPulseParticle *particle)
{
    RoomPulseParticle *next;
    RoomPulsePool *pool;
    RoomPulseObject *object;
    RoomM123Pool *context;
    RoomSoundSlot *soundSlot;
    int time;
    int volume;
    u16 palette;

    switch (mode) {
    case 0:
        func_800CE870(D_800F32D0->pool, 1, &particle->z);
        particle->x = 0;
        particle->y = 0;
        if (D_800E2368[13]) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = &D_800B0E64_slot;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(soundSlot->channel, 0x582, time, 0x80, volume);
            if (soundSlot->channel != 0)
                func_8006DF50(soundSlot->channel, 0x5B1, 0x80, 0x80, volume);
        }
        return func_800CE560(D_800F33E0->pool, 12, 8, func_80194A70);
    case 1:
        if (D_800E27EC < 32 && D_800E27EC % 6 == 0) {
            next = func_800CE610(D_800F33E0->pool);
            if (next != 0) {
                next->x = particle->z;
                next->y = particle->pad;
                next->z = particle->state;
                next->y = D_800942EC.count;
                next->state = 0;
                next->frame = 0;
            }
        }
        if (D_800E27EC == 32 && D_800E2368[13]) {
            context = D_800F32D0;
            if ((((RoomPulsePool *)context->pool)->object->flags & 0x3F000000) == 0x01000000) {
                (*D_8009D254)->flags |= 0x4000;
                ((RoomPulsePool *)context->pool)->object->flags =
                    (((RoomPulsePool *)context->pool)->object->flags & 0xC0FFFFFF) | 0x21000000;
                ((RoomPulsePool *)context->pool)->object->flags |= 0x80000000;
            }
        }
        if (D_800E27EC < 8) goto ret0;
        return 2;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
ret0:
    return 0;
}
