#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Sweep particle: while it rides the sweep centre it spins up, pulses its
 * size and drops a decaying copy every other frame; the draw turns and
 * stretches the shared model at its position. */
int func_8019326C(int mode, RoomShakeSweepParticle *p) {
    GteShortVector rotation;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomShakeSweepParticle *child;
    int page;
    int spin;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 1:
        if (p->state == 0) {
            p->x = D_80199904.x;
            p->z = D_80199904.z;
            p->y = D_80199904.y;
            spin = p->angle + 0x20;
            p->angle = spin + D_800E27EC * 4;
            p->size = func_80077CF4((D_800E27EC << 11) / 24) / 32;
            p->stretch = (D_800E27EC << 12) / 24;
            if (D_800E27EC & 1) {
                child = (RoomShakeSweepParticle *)func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->state = 1;
                    child->x = p->x;
                    child->y = p->y;
                    child->z = p->z;
                    child->size = p->size;
                    child->stretch = p->stretch;
                }
            }
            if (D_800E27EC >= 0x18)
                return 1;
        } else {
            p->angle += 3;
            p->size = p->size * 3 / 4;
            if (D_800E27EC >= 4)
                return 1;
        }
        p->timer++;
        break;
    case 2:
        scale.z = 0x400;
        scale.x = 0x400;
        scale.y = p->stretch / 4;
        rotation.x = 0;
        rotation.y = p->angle;
        rotation.z = 0;
        func_80079754(&rotation, &matrix);
        func_80078CC4(&matrix, &scale);
        matrix.t[0] = p->x;
        matrix.t[1] = p->y;
        matrix.t[2] = p->z;
        index = &D_800E11FA;
        tpages = D_800E2850;
        {
            int tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.tpage = tpage;
        }
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        {
            int kind;
            int palette;
            int rawPage = func_80077A64(0, 1, 0, 0);
            page = (u16)(tpages[*index] | rawPage);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            palette = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8 : palette + 4);
            func_800C6EC0(page, (u16)palette);
        }
        func_800C6ED8(1);
        func_800C6EF8(D_800F32D8);
        func_800C7098(D_800F32D8, (u8)(p->size / 3), 0, (u8)(p->size / 2));
        func_800C71E4(D_800F32D8, &matrix);
        func_800C6F4C(D_800F32D8);
        break;
    }
    return 0;
}


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
        anchor->y = D_800942EC.y;
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
