#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

extern char D_801995FC[];
extern int func_80071A54(void);
extern void func_800CF3AC(void *, void *, int);

int func_80193FC4(int mode, RoomShakeBurstParticleCallbackView *particle) {
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

/* Wakes the actor's status byte and plays the burst sounds, then for 39
 * frames shakes the camera while scattering particles around the anchor
 * for the first 21; at frame 40 it hands the actor to the scene script. */
int func_80194164(int mode, RoomShakeBurstPoint *anchor) {
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    RoomShakeBurstPoint *next;
    void **soundSlot;
    int volume;
    int time;

    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, (s16 *)anchor);
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(*soundSlot, 0x5F1, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5F2, 0x80, 0x80, volume);
        }
        return func_800CE560(D_800F33E0->pool, 8, 0x14, func_80193FC4);
    case 1:
        if (D_800E27EC < 0x27)
            func_80020D50();
        if (D_800E27EC < 0x15) {
            next = func_800CE610(D_800F33E0->pool);
            if (next != 0) {
                next->x = anchor->x + (func_80071A54() & 0x1FF) - 0x100;
                next->y = anchor->y + (func_80071A54() & 0x1FF) - 0x100;
                next->z = anchor->z + (func_80071A54() & 0x1FF) - 0x100;
            }
        }
        if (D_800E27EC == 0x27)
            func_80020DD0();
        if (D_800E27EC == 0x28 && D_800E2368->active) {
            channel = D_800F32D0;
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*D_8009D254)->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (D_800E27EC < 0x35)
            break;
        return 1;
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
            D_800F3368.depth = 0x10;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
