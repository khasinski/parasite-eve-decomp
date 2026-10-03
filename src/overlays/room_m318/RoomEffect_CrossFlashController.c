#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Cross flash: takes the actor's orientation, then at frame 2 drops a
 * flash and four sparks turned a quarter turn about one axis, and at
 * frame 8 the same about the other axis; the draw publishes the anchor. */
int func_80196820(int mode, RoomCrossFlash *flash) {
    RoomCrossFlashParticle *child;
    int i;

    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, (s16 *)flash);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0,
                      (GteShortVector *)&flash->rotation);
        flash->rotation.x = 0;
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_8019646C);
    case 1:
        if (D_800E2368->running == 0)
            return 1;
        if (D_800E27EC == 2) {
            child = (RoomCrossFlashParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = flash->x;
                child->y = flash->y;
                child->z = flash->z;
                child->rotation.x = flash->rotation.x;
                child->rotation.y = flash->rotation.y;
                child->rotation.z = flash->rotation.z;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 4; i++) {
                child = (RoomCrossFlashParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = (func_80071A54() & 0x7F) - 0x40;
                    child->y = 0;
                    child->z = (func_80071A54() & 0x1F) + 0x10;
                    child->rotation.x = flash->rotation.x;
                    child->rotation.y = flash->rotation.y;
                    child->rotation.z = flash->rotation.z;
                    child->rotation.y += 0x400;
                    child->rotation.x += func_80071A54();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 8) {
            child = (RoomCrossFlashParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = flash->x;
                child->y = flash->y;
                child->z = flash->z;
                child->rotation.x = flash->rotation.x;
                child->rotation.y = flash->rotation.y;
                child->rotation.z = flash->rotation.z;
                child->rotation.x += 0x400;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 4; i++) {
                child = (RoomCrossFlashParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = 0;
                    child->y = (func_80071A54() & 0x7F) - 0x40;
                    child->z = (func_80071A54() & 0x1F) + 0x10;
                    child->rotation.x = flash->rotation.x;
                    child->rotation.y = flash->rotation.y;
                    child->rotation.z = flash->rotation.z;
                    child->rotation.y += func_80071A54();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 8) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_80199930.x = flash->x;
        D_80199930.y = flash->y;
        D_80199930.z = flash->z;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
