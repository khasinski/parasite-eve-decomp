#include "pe1/room_shake_burst.h"

extern int func_80193FC4(int mode, RoomShakeBurstPoint *particle);

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
