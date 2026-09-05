#include "common.h"
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

typedef struct RoomBlob4 {
    char b[4];
} RoomBlob4;

typedef struct RoomBounceFx {
    short x;
    short y;
    short z;
    short pad6;
    short vx;
    short vy;
    short vz;
    short ay;
    short state;
    short timer;
} RoomBounceFx;

extern RoomBlob4 ROOMLIB_BOUNCE_RENDER_BLOB;
extern short g_FrameCount16;
extern short D_800F336A;
extern unsigned short D_800F336C;
extern unsigned short D_800E1204[];
extern int D_800F3428;
extern void *D_800BCFA4;

int func_80077CF4(int angle);
int func_80077DC4(int angle);
int func_80077AA4(int width, int texture);
int func_800CEE20();
int func_800D2104();

int ROOMLIB_UPDATE_BOUNCE_RENDER_NAME(int mode, RoomBounceFx *fx) {
    RoomBlob4 blob;
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
        register int rawVy asm("$3");
        int nextVy;
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
        PE1_COMPILER_USE(rawVy);
        fx->vz = vz >> 9;
        PE1_COMPILER_MEMORY_BARRIER();
        y = fx->y;
        PE1_COMPILER_USE(y);
        nextVy = rawVy - ROOMLIB_BOUNCE_GRAVITY;
        fx->vy = nextVy;
        PE1_COMPILER_MEMORY_BARRIER();
        if (y >= g_FrameCount16) {
            int bounce;

            PE1_COMPILER_MEMORY_BARRIER();
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
        PE1_COMPILER_MEMORY_BARRIER();
        if (fx->y >= g_FrameCount16) {
            fx->y = g_FrameCount16;
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
        register int angle asm("$4");
        register int fade asm("$2");
        int texture;

        sinValue = func_80077CF4((fx->timer << 10) / 24);
        angle = (fx->timer * 1204) / 24;
        PE1_COMPILER_USE(angle);
        scale = ((unsigned int)sinValue >> 31) + sinValue;
        scale >>= 1;
        saved2 = scale + 0x1000;
        fade = func_80077DC4(angle);
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
        func_800CEE20(fx, 0, saved2, saved2,
                      D_800F336A * (fx->timer / 6) + ROOMLIB_BOUNCE_FRAME_BASE,
                      (unsigned short)texture, 1, saved0, 0);
        goto ret0;
    }
drawRise:
    {
        int angle;
        void **matrixSlot;

        angle = func_80077DC4(fx->timer << 5);
        if (angle < 0) {
            angle += 0x1F;
        }
        saved0 = angle >> 5;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800D2104(fx, (void *)saved2, saved0, 1);
    }
ret0:
    return 0;
}
