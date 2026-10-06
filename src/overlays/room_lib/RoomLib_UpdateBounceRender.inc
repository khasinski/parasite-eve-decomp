/*
 * Bounce particle: state 0 bounces along the floor and draws a fading
 * sprite, state 1 rises to the floor height and draws a coloured tile.
 * Instantiated by the line burst unit (RoomEffect_LineBurst.c) and by the
 * burst orb rooms, which differ only in gravity, CLUT column and frame base.
 *
 * The includer names the function (ROOMLIB_UPDATE_BOUNCE_RENDER_NAME) and
 * the tile colour constant (ROOMLIB_BOUNCE_RENDER_BLOB).
 */
#include "pe1/room_line_burst.h"
#include "pe1/field_tile.h"
#include "pe1/gte.h"

#ifndef ROOMLIB_BOUNCE_GRAVITY
#define ROOMLIB_BOUNCE_GRAVITY 1
#endif
#ifndef ROOMLIB_BOUNCE_CLUT_X
#define ROOMLIB_BOUNCE_CLUT_X 0x10
#endif
#ifndef ROOMLIB_BOUNCE_FRAME_BASE
#define ROOMLIB_BOUNCE_FRAME_BASE 0xC8
#endif

extern const RoomLineBurstColor ROOMLIB_BOUNCE_RENDER_BLOB;

int ROOMLIB_UPDATE_BOUNCE_RENDER_NAME(int mode, RoomLineBurstParticle *fx) {
    RoomLineBurstColor blob;
    int saved0;
    int saved2;

    blob = ROOMLIB_BOUNCE_RENDER_BLOB;
    saved2 = (int)&blob;
    if (mode == 1) {
        goto mode1;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode1:
    if (fx->state == 0) {
        goto updateBounce;
    }
    if (fx->state == 1) {
        goto updateRise;
    }
    goto ret0;

updateBounce:
    {
        int vx;
        int vz;
        int rawVy;
        int nextVy;
        /* Pinned: unpinned, the height lands in another register than retail's. */
        register int y asm("$2");

        fx->timer++;
        fx->x += fx->vx;
        fx->y += fx->vy;
        fx->z += fx->vz;
        vx = fx->vx * 511;
        if (vx < 0) {
            vx += 511;
        }
        fx->vx = vx >> 9;
        vz = fx->vz * 511;
        if (vz < 0) {
            vz += 511;
        }
        rawVy = (unsigned short)fx->vy;
        fx->vz = vz >> 9;
        PE1_COMPILER_MEMORY_BARRIER();
        y = fx->y;
        nextVy = rawVy - ROOMLIB_BOUNCE_GRAVITY;
        fx->vy = nextVy;
        if (y >= D_800942EC.count) {
            int bounce;

            bounce = (nextVy << 16) >> 16;
            bounce = -bounce;
            fx->vy = bounce;
        }
        if (fx->timer < 24) {
            goto ret0;
        }
        return 1;
    }
updateRise:
    {
        fx->timer++;
        fx->y += fx->vy;
        fx->vy += fx->ay;
        if (fx->y >= D_800942EC.count) {
            fx->y = D_800942EC.count;
        }
        if (fx->timer < 32) {
            goto ret0;
        }
        return 1;
    }

mode2:
    if (fx->state == 0) {
        goto drawBounce;
    }
    if (fx->state == 1) {
        goto drawRise;
    }
    goto ret0;

drawBounce:
    {
        int sinValue;
        int scale;
        /* Pinned: unpinned, the angle and fade take other registers than retail's. */
        register int angle asm("$4");
        register int fade asm("$2");
        int texture;

        sinValue = rsin((fx->timer << 10) / 24);
        angle = (fx->timer * 1204) / 24;
        PE1_COMPILER_USE(angle);
        scale = ((unsigned int)sinValue >> 31) + sinValue;
        scale >>= 1;
        saved2 = scale + 0x1000;
        fade = rcos(angle);
        if (fade < 0) {
            fade += 0x7F;
        }
        fade >>= 7;
        saved0 = fade + 0x28;
        texture = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) {
            texture += 4;
        }
        texture = func_80077AA4(ROOMLIB_BOUNCE_CLUT_X, texture);
        func_800CEE20((GteShortVector *)fx, 0, saved2, saved2,
                      D_800F336A * (fx->timer / 6) + ROOMLIB_BOUNCE_FRAME_BASE,
                      (unsigned short)texture, 1, saved0, 0);
        goto ret0;
    }
drawRise:
    {
        int angle;
        RenderMatrixSlot *matrixSlot;

        angle = rcos(fx->timer << 5);
        if (angle < 0) {
            angle += 0x1F;
        }
        saved0 = angle >> 5;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        func_800D2104((GteShortVector *)fx, (u8 *)saved2, saved0, 1);
    }
ret0:
    return 0;
}
