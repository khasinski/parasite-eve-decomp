#include "pe1/room_ember_spiral.h"
#include "pe1/gte.h"

/* Ember spiral: plays its sounds, loads the spiral model and attaches to
 * the actor's hand; for 24 frames it sheds orbiting sparks along its
 * heading, embers on odd frames and smoke, hits the player in reach, and
 * draws a glow plus the spiral model swelling and turning in front of the
 * hand. */
int func_801988F8(int mode, RoomEmberSpiral *spiral) {
    GteShortVector position;
    GteShortVector tip;
    GteShortVector rotation;
    RenderColor color;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomOrbitTrailParticle *child;
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    void **soundSlot;
    u16 *index;
    u16 *tpages;
    int volume;
    int time;
    int glow;
    int width;
    int depth;
    int page;

    switch (mode) {
    case 0:
        spiral->reserved08 = 0;
        spiral->timer = 0;
        spiral->glow = 0;
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(*soundSlot, 0x5ED, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5EE, 0x80, 0x80, volume);
        }
        D_80199944 = (void *)func_8006E498(D_800B0E64, 0xC5509704);
        func_800C6D5C(D_80199944, 0, 0);
        if (D_800E2368->active) {
            pool = D_800F32D0->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0,
                      (GteShortVector *)spiral);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_80198268);
    case 1:
        spiral->timer++;
        func_800CE870((char *)D_800F32D0->pool, 0, (s16 *)&position);
        spiral->glow = func_80077DC4((spiral->timer << 10) / 24) / 32;
        if (spiral->timer < 0x19) {
            child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (func_80071A54() & 0x3F) - 0x20;
                child->y += (func_80071A54() & 0x3F) - 0x20;
                child->z += (func_80071A54() & 0x3F) - 0x20;
                child->radius = (func_80071A54() & 0x1FF) + 0x200;
                child->heading.x = spiral->x;
                child->heading.y = spiral->y;
                child->heading.z = spiral->z;
                child->heading.pad = func_80071A54();
                child->state = 1;
                child->timer = 0;
            }
            if (spiral->timer & 1) {
                child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->y += 0x100 - (func_80071A54() & 0x1FF);
                    child->x += (func_80071A54() & 0x1FF) - 0x100;
                    child->z += (func_80071A54() & 0x1FF) - 0x100;
                    child->heading.x = func_80071A54() % 32 - 0x10;
                    child->heading.y = func_80071A54() % 32 - 0x10;
                    child->heading.z = func_80071A54() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = (RoomOrbitTrailParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (func_80071A54() & 0x1F) - 0x10;
                child->y += (func_80071A54() & 0x1F) - 0x10;
                child->z += (func_80071A54() & 0x1F) - 0x10;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (func_800C6B90(&position, 0x19A)) {
            if (D_800E2368->active) {
                channel = D_800F32D0;
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    (*D_8009D254)->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
        }
        if (spiral->timer < 0x18) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CE870((char *)D_800F32D0->pool, 0, (s16 *)&position);
        if (spiral->timer < 0x19) {
            glow = spiral->glow;
            if (D_800E27EC & 1)
                glow = glow * 3 / 4;
            D_800F3368.depth = 0x28;
            func_800CF3AC(D_80199890, &color, (spiral->timer << 5) / 24);
            func_800D004C(&position, 900, 800, 0xC, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            /* The angle shares its register with the model page below. */
            page = (spiral->timer << 10) / 24;
            depth = func_80077CF4(page) / 2 + 0x800;
            width = func_80077CF4(page) / 4 + 0xC00;
            func_800CFB7C((GteShortVector *)spiral, 200, &tip);
            tip.x += position.x;
            tip.y += position.y;
            tip.z += position.z;
            rotation.x = spiral->x;
            rotation.y = spiral->y;
            rotation.z = spiral->z;
            rotation.y += 0x800;
            rotation.z += spiral->timer * 44;
            index = &D_800E11EA;
            tpages = D_800E2850;
            {
                int tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.depth = 4;
                D_800F3368.tpage = tpage;
            }
            {
                int tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            {
                int kind;
                int palette;
                int rawPage = func_80077A64(0, 1, 0, 0);
                page = (u16)(tpages[*index] | rawPage);
                kind = D_800F336C;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                palette = GetClut(0x40, palette);
                func_800C6EC0(page, (u16)palette);
            }
            func_800C6ED8(1);
            func_80079754(&rotation, &matrix);
            matrix.t[0] = tip.x;
            matrix.t[1] = tip.y;
            matrix.t[2] = tip.z;
            scale.x = width;
            scale.y = width;
            scale.z = depth;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(D_80199944);
            func_800C6FA0(D_80199944, (u16)glow);
            func_800C71E4(D_80199944, &matrix);
            func_800C6F4C(D_80199944);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
