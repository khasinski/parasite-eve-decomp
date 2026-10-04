#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Cross flash particle: the flash draws as a pulsing tilted ring, the
 * sparks spin their orientation by their offset and draw as flickering
 * beams from the cross flash centre. */
int func_8019646C(int mode, RoomCrossFlashParticle *p) {
    GteShortVector origin;
    RenderColor glowColor;
    RenderColor color = D_8018F1FC;
    int angle;
    int size;
    int glow;
    int length;
    int width;
    int u;
    int v;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->rotation.x += p->x;
            p->rotation.y += p->y;
            p->rotation.z += p->z;
            if ((s16)p->timer < 0x28) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = func_80077CF4((s16)p->timer << 5);
            func_800CF3AC(D_80199798, &glowColor, (s16)p->timer);
            func_800D0728((GteShortVector *)p, 0x578, 0x898, 0xC, &p->rotation, size,
                          size, 0, &glowColor, 0x80, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 40;
            glow = func_80077DC4(angle) / 32;
            if (p->timer & 1)
                glow = glow * 2 / 3;
            origin.x = D_80199930.x;
            origin.y = D_80199930.y;
            origin.z = D_80199930.z;
            length = 0x76C;
            if ((s16)p->timer < 0x21)
                length = func_80077CF4((s16)p->timer << 5) * 1900 / 4096;
            width = func_80077DC4(angle) / 64 + 0x20;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            /* The flicker frame reuses the flash size temporary. */
            size = (s16)(p->timer & 3);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            u = (size & 1) * 128;
            v = (size / 2) * 16 + 0x80;
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&origin, &p->rotation, length, width, u, v, 0x80, 0x10,
                          GetClut(0x50, palette), &color, &color, (s16)glow, 1);
            break;
        }
        }
        break;
    }
    return 0;
}

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
