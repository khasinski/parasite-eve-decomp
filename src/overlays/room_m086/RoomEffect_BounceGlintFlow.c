#include "common.h"
#include "pe1/room_bounce_glint.h"

/* Bouncing glint particle callback for the room_m086 controller: mode 1
 * moves it (drift, then fall and bounce), mode 2 draws it. Both draws read
 * the palette kind back from the parameter block, which keeps the block
 * address in a saved register for the parameter02 read after the clut call.
 * The switches have no default arms so the last draw falls straight into
 * the shared return label, which lets each phase 0 draw cross-jump only the
 * call. */

int func_801900CC(int mode, RoomBounceGlint *glint, int *size) {
    GteRotation rotation;
    int extent = *size;
    int intensity;

    switch (mode) {
    case 1:
        switch (glint->phase) {
        case 0:
            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vy++;
            if (glint->timer < 24) {
                break;
            }
            return 1;
        case 1: {
            int fall;

            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vx = glint->vx * 31 / 32;
            glint->vz = glint->vz * 31 / 32;
            fall = (u16)glint->vy + 4;
            glint->vy = fall;
            if ((s16)glint->y >= D_800942EC.height) {
                int bounce = -(s16)fall;

                glint->vy = bounce;
            }
            if (glint->timer >= 32) {
                return 1;
            }
            break;
        }
        default:
            return 0;
        }
        break;
    case 2:
        switch (glint->phase) {
        case 0:
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            intensity = func_80077DC4((glint->timer << 10) / 24) / 32;
            if (glint->flag != 0) {
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20((GteShortVector *)glint, 0, extent, extent,
                              D_800F336A * (s16)(glint->timer / 6),
                              (u16)func_80077AA4(0, palette), 2, intensity, 0);
            } else {
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20((GteShortVector *)glint, 0, extent * 3 / 2,
                              extent * 3 / 2,
                              D_800F336A * ((glint->timer / 2) & 3),
                              (u16)func_80077AA4(0, palette), 1, intensity, 0);
            }
            break;
        case 1: {
            int kind;
            int palette;

            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            rotation.z = D_800E27EC << 7;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)glint, &rotation, extent * 3 / 2,
                          extent * 3 / 2,
                          (s16)D_800F3368.parameter02 * glint->flag + 0x1A,
                          (u16)func_80077AA4(0x70, palette), 0xFF, 0x80, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

typedef struct RoomM086Blob8 {
    char bytes[8];
} RoomM086Blob8;

extern void *D_800F32D0;
extern void *D_800F33E0;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern s16 D_800F3376;
extern s16 D_800F3378;
extern char D_8018EFF4[];

void func_800CE560(void *arg0, s32 arg1, s32 arg2, void *arg3);
void *func_800CE610(void *arg0);
void func_800CE8F0(void *arg0, s32 arg1, void *arg2, void *arg3);
s32 func_80071A54(void);
s32 func_80077CF4(s32 arg0);
s32 func_80077DC4(s32 arg0);
s32 func_800D3FD8(void);
void func_800D3F64(s32 arg0, s32 arg1);


#define COPY_POS(dst, src) \
    (dst)->x = (src)->x;   \
    (dst)->y = (src)->y;   \
    (dst)->z = (src)->z

s32 func_80190574(s32 mode, RoomBounceGlint *ent) {
    RoomM086Blob8 blob;
    RoomBounceGlint *actor;
    s32 phase;
    s32 angle;
    s32 mag;
    s32 i;
    s32 value;

    blob = *(RoomM086Blob8 *)D_8018EFF4;

    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
        goto ret0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode0:
    func_800CE8F0(((void **)D_800F32D0)[2], 7, &blob, ent);
    ent->y = D_800942EC.height;
    value = func_800D3FD8();
    func_800D3F64(0x579, value);
    func_800CE560(((void **)D_800F33E0)[2], 0x14, 0x18, func_801900CC);
    goto done;

mode1:
    value = D_800E27EC;
    if (value == 1) {
        angle = func_80071A54();
        i = 0;
        do {
            actor = func_800CE610(((void **)D_800F33E0)[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (func_80071A54() & 0xF) + 7;
                value = func_80077DC4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = func_80077CF4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(func_80071A54() & 0xF) - 0xE;
                actor->phase = 0;
                actor->timer = 0;
            }
            i++;
            angle += 0x2AA;
        } while (i < 6);
        value = D_800E27EC;
    }

    value = D_800E27EC - 2;
    if (value < 3U) {
        angle = func_80071A54();
        i = 0;
        do {
            actor = func_800CE610(((void **)D_800F33E0)[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (func_80071A54() & 0x1F) + 0x18;
                value = func_80077DC4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = func_80077CF4(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(func_80071A54() & 0x1F) - 0x18;
                actor->flag = func_80071A54() & 1;
                actor->phase = 1;
                actor->timer = 0;
            }
            i++;
            angle += 0xAAA;
        } while (i < 2);
    }

    value = D_800E27EC - 2;
    if (value < 0x15U) {
        angle = func_80071A54();
        actor = func_800CE610(((void **)D_800F33E0)[2]);
        if (actor != 0) {
            COPY_POS(actor, ent);
            value = func_80071A54();
            phase = actor->x;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->x = phase;
            value = func_80071A54();
            phase = actor->z;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->z = phase;
            mag = (func_80071A54() & 0xF) + 7;
            if (D_800E27EC & 1) {
                actor->vy = -(func_80071A54() & 0x1F) - 0x1E;
            } else {
                actor->vy = -(func_80071A54() & 0xF) - 8;
                mag = (s32)(mag + ((u32)mag >> 31)) >> 1;
            }
            value = func_80077DC4(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vx = value >> 12;
            value = func_80077CF4(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vz = value >> 12;
            actor->flag = func_80071A54() & 1;
            actor->phase = 0;
            actor->timer = 0;
        }
    }
    if (D_800E27EC < 0x46) {
        goto ret0;
    }
    return 1;

mode2:
    D_800F3374 = 0x10;
    D_800F3368.parameter00 = 0x20;
    D_800F336A = 2;
    D_800F3376 = 0x20;
    D_800F3378 = 0x20;
    D_800F336C = 3;
    D_800F336E = 0;
    D_800F3372 = 0;
    D_800F3370 = D_800E2850[D_800E11EA];

ret0:
    return 0;
done:;
}
