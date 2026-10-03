#include "pe1/room_shake_burst.h"

/* Plays the sweep sounds and wakes the actor's status byte, then for 43
 * frames shakes the camera while handing a turning sweep angle to a new
 * particle every sixth frame; at frame 44 it hands the actor to the scene
 * script, and every draw publishes the anchor as the sweep centre. */
int func_80193638(int mode, RoomShakeSweepAnchor *anchor) {
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    RoomShakeSweepParticle *next;
    void **soundSlot;
    int volume;
    int time;

    switch (mode) {
    case 0:
        anchor->angle = func_80071A54();
        func_800CE870((char *)D_8009D254, 1, (s16 *)anchor);
        anchor->y = D_800942EC;
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(*soundSlot, 0x5EF, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5F0, 0x80, 0x80, volume);
        }
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        func_800C6D5C(D_800F32D8, 0, 0);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x18, func_8019326C);
    case 1:
        if (D_800E27EC < 0x2B)
            func_80020D50();
        if (D_800E27EC < 0x28 && D_800E27EC % 6 == 0) {
            next = (RoomShakeSweepParticle *)func_800CE610(D_800F33E0->pool);
            if (next != 0) {
                next->angle = anchor->angle;
                next->state = 0;
                next->timer = 0;
                {
                    int jitter = func_80071A54() & 0xFF;
                    int angle = anchor->angle - 0x555;
                    anchor->angle = angle - jitter;
                }
            }
        }
        if (D_800E27EC == 0x2B)
            func_80020DD0();
        if (D_800E27EC == 0x2C && D_800E2368->active) {
            channel = D_800F32D0;
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*D_8009D254)->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (D_800E27EC < 0x40)
            break;
        return 1;
    case 2:
        D_80199904.x = anchor->x;
        D_80199904.y = anchor->y;
        D_80199904.z = anchor->z;
        break;
    }
    return 0;
}
