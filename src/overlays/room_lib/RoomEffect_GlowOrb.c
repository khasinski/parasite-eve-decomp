/*
 * The glow orb: a module whose spawner drops a glow at the owner, which
 * drifts until it leaves the walkable area, leaves a fading flash every
 * third frame and is picked up when the player touches it.
 *
 * room_m034, room_m174 and room_m383 link the same fourteen functions in
 * this order, from the class's two no-ops after RoomLib_CloseTarget to the
 * flash update, with the orb's spin and the flash scale as their read-only
 * data at 0xC, which splat had merged into the room header's rodata. The
 * module's lists, script, matrices and sprite blocks are each room's own
 * data (pe1/room_glow_orb.h, pe1/room_module.h).
 */
#include "pe1/room_glow_orb.h"
#include "pe1/gte.h"
#include "pe1/field_collision.h"

static const GteShortVector s_GlowOrbSpin = { 0xC00, 0, 0, 0 };
static const GteVector s_GlowOrbFlashScale = { 0x448, 0x448, 0x448, 0 };

/* Class slots 6 and 0. */
int RoomEffect_GlowOrbNop6(void) {
    return 0;
}

int RoomEffect_GlowOrbNop0(void) {
    return 0;
}

/* Controller init: captures the owner's placement matrix, and the matrix
 * the orb starts from (the player's own when the owner is the player). */
void RoomEffect_GlowOrbCaptureOwner(RoomGlowOrbObject *object) {
    g_RoomGlowOrbOwner = object->link;
    g_RoomGlowOrbAxes = g_RoomGlowOrbOwner->matrices[0];

    g_RoomGlowOrbOrigin = g_RoomGlowOrbOwner->matrices[0x31];

    if ((void *)object->link == (void *)D_8009D254) {
        g_RoomGlowOrbOrigin = g_RoomGlowOrbOwner->matrices[0];
    }
}

void RoomEffect_GlowOrbControllerDraw(void) {
}

void RoomEffect_GlowOrbControllerUpdate(void *object, RoomFallingParticleControl *control) {
    control->state = 2;
}

void RoomEffect_GlowOrbSpawnerInit(void *object, void *control, RoomGlowOrbSpawner *spawner) {
    spawner->pad00 = 0;
    spawner->yaw = *func_800C2B10(4);
}

void RoomEffect_GlowOrbSpawnerDraw(void) {
}

/* Restores the recorded yaw and drops the orb. */
void RoomEffect_GlowOrbSpawnerUpdate(void *object, RoomFallingParticleControl *control,
                                     RoomGlowOrbSpawner *spawner) {
    int *yaw;

    yaw = func_800C2B10(4);
    *yaw = spawner->yaw;
    func_800C2B90(object, 2, g_RoomSpawnLayout, g_RoomInitList);
    control->state = 2;
}

void RoomEffect_GlowOrbInit(void *object, void *slot, RoomGlowOrb *orb) {
    GteShortVector tilt;
    GteShortVector spin = s_GlowOrbSpin;
    GteMatrix local;
    GteShortVector turn;
    GteMatrix base;
    int *yaw;

    turn.x = 0;
    yaw = func_800C2B10(4);
    turn.y = *yaw + *func_800C2B28(2);
    turn.z = 0;
    RotMatrixYXZ(&turn, &base);
    tilt.x = 0;
    tilt.y = 0;
    tilt.z = -*func_800C2B28(0);
    ApplyMatrixSV(&g_RoomGlowOrbAxes, &tilt, &orb->offset);
    ApplyMatrixSV(&base, &orb->offset, &orb->offset);
    orb->matrix = g_RoomGlowOrbAxes;
    orb->x = g_RoomGlowOrbOrigin.t[0];
    orb->y = D_800942EC.count - 0x100;
    orb->z = g_RoomGlowOrbOrigin.t[2];
    orb->size = 0x80;
    orb->h10 = 0;
    orb->h12 = 0;
    orb->h14 = 0;
    orb->depth = 0;
    orb->flag2 = 0;
    orb->state = 1;
    orb->slot = *func_800C2B10(3);
    RotMatrixYXZ(&spin, &local);
    gte_CompMatrix(&orb->matrix, &local, &orb->matrix);
    gte_CompMatrix(&base, &orb->matrix, &orb->matrix);
}

/* Draw the glow orb: two scaled halo layers at the orb, a touch test
 * against the player that flags the pickup, then three trailing sparks
 * stepping back along the orb's offset. The GNU constructor assignments
 * clear a temporary with memset and copy it, and the second scale lives in
 * a nested block so it takes the first temporary's freed stack slot. */
void RoomEffect_GlowOrbDraw(RoomGlowOrbObject *object, u8 *slot, RoomGlowOrb *orb) {
    GteMatrix matrix;
    GteShortVector point;

    func_800C2EAC(0);
    func_800C3098(0x10);
    func_800C2FF0(0x40, 0x20);
    func_800C3238(2);
    if (orb->state == 1) {
        GteVector scale;

        g_RoomGlowOrbSprite.r = 0x10;
        g_RoomGlowOrbSprite.g = 0x10;
        g_RoomGlowOrbSprite.b = 0x20;
        g_RoomGlowOrbSprite.depth = orb->depth;
        matrix = orb->matrix;
        /* Retail subtracts offset.x on all three axes for the inner halo. */
        matrix.t[0] = orb->x - orb->offset.x;
        matrix.t[1] = orb->y - orb->offset.x;
        matrix.t[2] = orb->z - orb->offset.x;
        scale = (GteVector){ orb->size >> 1, 0x251, 0x251 };
        ScaleMatrix(&matrix, &scale);
        func_800C42A4(&g_RoomGlowOrbSprite, &matrix, 0);

        g_RoomGlowOrbSprite.r = 0x20;
        g_RoomGlowOrbSprite.g = 0x20;
        g_RoomGlowOrbSprite.b = 0x40;
        matrix = orb->matrix;
        matrix.t[0] = orb->x;
        matrix.t[1] = orb->y;
        matrix.t[2] = orb->z;
        {
            GteVector size;

            size = (GteVector){ orb->size, 0x448, 0x448 };
            ScaleMatrix(&matrix, &size);
            func_800C42A4(&g_RoomGlowOrbSprite, &matrix, 0);
        }

        point.x = D_8009D254->x.part.integer;
        point.y = D_8009D254->y.part.integer;
        point.z = D_8009D254->z.part.integer;
        if (func_800C61A8(&point, &matrix) != 0 && FieldEng_GetStatus(object) == 3) {
            D_8009D254->actor->flags |= 0x4000;
            if (object->link != 0) {
                *object->link->target |= 0x80000000;
            }
            slot[1] = 2;
        }

        g_RoomGlowOrbSprite.r = 0x78;
        g_RoomGlowOrbSprite.g = 0xF0;
        g_RoomGlowOrbSprite.b = 0x78;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&g_RoomGlowOrbSprite, &matrix, 0);

        matrix.t[0] = orb->x - orb->offset.x;
        matrix.t[1] = orb->y - orb->offset.y;
        matrix.t[2] = orb->z - orb->offset.z;
        g_RoomGlowOrbSprite.r = 0x3C;
        g_RoomGlowOrbSprite.g = 0x78;
        g_RoomGlowOrbSprite.b = 0x3C;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&g_RoomGlowOrbSprite, &matrix, 0);

        matrix.t[0] = orb->x - orb->offset.x * 2;
        matrix.t[1] = orb->y - orb->offset.y * 2;
        matrix.t[2] = orb->z - orb->offset.z * 2;
        g_RoomGlowOrbSprite.r = 0x1E;
        g_RoomGlowOrbSprite.g = 0x3C;
        g_RoomGlowOrbSprite.b = 0x1E;
        matrix.t[1] = orb->y - 0x100;
        func_800C42A4(&g_RoomGlowOrbSprite, &matrix, 0);
    }
}

/* Drifts the orb by its offset and brightens it; every third frame over the
 * walkable area it leaves a flash on the floor, and it ends once it leaves
 * the area. */
void RoomEffect_GlowOrbUpdate(void *owner, RoomFallingParticleControl *control,
                              RoomFallingParticleState *state) {
    u16 *spawn;

    state->x += state->velocityX;
    state->y += state->velocityY;
    state->z += state->velocityZ;
    state->phase += 30;
    if (state->intensity < 129) {
        state->intensity += 10;
    }

    state->frame++;
    if (Geo_PointInPoly((s16)state->x << 16, (s16)state->z << 16, D_8009D248, D_8009D1CC)) {
        if ((s8)state->frame % 3 == 0) {
            spawn = func_800C2B90(owner, 3, g_RoomSpawnLayout, g_RoomInitList);
            if (spawn != 0) {
                spawn[4] = state->x;
                spawn[5] = D_800942EC.count;
                spawn[6] = state->z;
            }
        }
    }

    if (!Geo_PointInPoly((s16)state->x << 16, (s16)state->z << 16, D_8009D248, D_8009D1CC)) {
        control->state = 2;
        state->active = 0;
    }
}

void RoomEffect_GlowOrbFlashInit(void *object, void *control, RoomGlowOrbFlash *flash) {
    flash->timer = 0;
    flash->depth = 0x80;
}

void RoomEffect_GlowOrbFlashDraw(void *object, void *control, RoomGlowOrbFlash *flash) {
    GteMatrix matrix;
    GteVector scale;
    s16 *depth;

    func_800C2EAC(0);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    depth = &g_RoomGlowOrbFlashSprite.depth;
    *depth = flash->depth;
    matrix.t[0] = flash->x;
    matrix.t[1] = flash->y;
    matrix.t[2] = flash->z;
    scale = s_GlowOrbFlashScale;
    ScaleMatrix(&matrix, &scale);
    func_800C42A4(&g_RoomGlowOrbFlashSprite, &matrix, 1);
}

void RoomEffect_GlowOrbFlashUpdate(void *object, RoomFallingParticleControl *control,
                                   RoomGlowOrbFlash *flash) {
    flash->timer++;

    if (flash->timer >= 0x15) {
        control->state = 2;
    }

    if (flash->depth >= 9) {
        flash->depth -= 8;
    }
}
