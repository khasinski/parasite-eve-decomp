#include "room_m273_boss.h"
#include "pe1/gte.h"
#include "pe1/field_actor.h"


typedef struct RoomM273RingController {
    u8 reserved_00[8];
    FieldActor *actor;
} RoomM273RingController;

/* Falling trail record: spins down from the boss, records where it lands or
 * touches the player, and draws a fading two-point trail ring behind it. */
int func_80194E6C(int mode, RoomM273FallingTrail *trail) {
    GteMatrix matrix;
    GteShortVector vector;
    GteShortVector point;
    GteRotation rotation;
    s16 *source;
    int i;
    s16 floor;
    s16 intensity;
    s16 scale;

    if (mode == 1) {
        if (trail->landed) {
            return --trail->count <= 0;
        }
        if (trail->spin) {
            trail->yaw += trail->spin;
            trail->spin += trail->spin < 0 ? -2 : 2;
        }
        vector.x = trail->pitch;
        vector.y = trail->yaw;
        vector.z = 0;
        RotMatrixYXZ(&vector, &matrix);
        for (i = 0, source = &trail->position.x; i < 3; i++) {
            matrix.t[i] = *source++;
        }
        gte_ldrotmatrix(&matrix);
        gte_ldtransmatrix(&matrix);
        gte_ldv0(&D_8019ACA4);
        gte_rtv0tr_mac();
        gte_stsv(&trail->position);
        floor = D_800942EC.value;
        if (trail->position.y >= floor - 0x80) {
            trail->position.y = floor;
            trail->landed = 1;
        }
        if (trail->position.y >= D_8019AE9C.floor && g_PlayerEntity->mode >= 4) {
            int dx = g_PlayerEntity->position[0] - trail->position.x;
            int dz = g_PlayerEntity->position[2] - trail->position.z;
            if (Math_IntSqrt(dx * dx + dz * dz) < 0x140) {
                trail->landed = 1;
                source = &trail->position.x;
                for (i = 0; i < 3; i++) {
                    D_8019AE9C.hit[i] = (g_PlayerEntity->transforms->t[i] + source[i]) / 2;
                }
                D_8019AE9C.hit_flag = 1;
                g_PlayerEntity->actor->flags |= 0x4000;
                if (D_800F32D0->instance->owner) {
                    D_800F32D0->instance->owner->flags |= 0x80000000;
                }
            }
        }
        if (trail->landed) {
            i = D_8019AE9C.landing_count++;
            D_8019AE9C.landing_x[i] = trail->position.x;
            D_8019AE9C.landing_y[i] = trail->position.y;
            D_8019AE9C.landing_z[i] = trail->position.z;
        }
        trail->head = (trail->head - 1) & 7; i = trail->head;
        gte_ldv0(&D_8019ACAC);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        trail->trail_y[i] = vector.y;
        i *= 2;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        gte_ldv0(&D_8019ACB4);
        gte_rtv0tr_mac();
        gte_stsv(&vector);
        i++;
        trail->trail_x[i] = vector.x;
        trail->trail_z[i] = vector.z;
        if (trail->count < 8) {
            trail->count++;
        }
    } else if (mode == 2) {
        if (!trail->landed) {
            int kind;
            int palette;
            int index = D_800E11EA.index;

            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            rotation.x = trail->pitch + 0x400;
            rotation.y = trail->yaw;
            rotation.z = -0x400;
            rotation.flags = 1;
            if (D_800E27EC < 17) {
                scale = D_800966EC[(D_800E27EC << 8 & 0x3F00) >> 2].sine * 3 * 2048 / 4096 + 0x800;
            } else {
                scale = 0x2000;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) {
                palette += 4;
            }
            func_800CEE20(&trail->position, &rotation, scale, 0x1000, 0xC,
                          GetClut(0x30, palette), 1, 0x80, 0);
            intensity = 0x80;
        } else {
            intensity = trail->count * 16;
        }
        scale = (0x80 - intensity) * 64 + 0x1000;
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.tpage = D_800E2850[D_800E11E8];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 8;
        rotation.flags = 0;
        {
            s16 step;
            s16 index = trail->head;

            for (step = 0; step < trail->count; step++, index = (index + 1) & 7) {
                s16 side;

                point.y = trail->trail_y[index];
                for (side = 0; side < 2; side++) {
                    int kind;
                    int palette;

                    point.x = trail->trail_x[index * 2 + side];
                    point.z = trail->trail_z[index * 2 + side];
                    kind = D_800F3368.palette;
                    palette = D_800E1204[kind];
                    if (kind == 4 && D_800F3428) {
                        palette += 4;
                    }
                    func_800CEE20(&point, &rotation, scale, scale, 0xDC,
                                  GetClut(0x30, palette), 1, intensity, &D_8019ACBC);
                    rotation.y += 0x800;
                }
                intensity -= 0x10;
                scale += 0x400;
                rotation.z += 0x100;
            }
        }
    }
    return 0;
}

extern s16 D_800F3372;
extern u16 D_800F3376, D_800F3378;
extern s16 D_8019ACA8;
extern s16 D_8019AE6C, D_8019AE70, D_8019AE74, D_8019AE78;
extern u8 D_8019AEF8, D_8019AEFA;
extern int Inv_ScrambleGrid(void);
extern void func_8006DCE4(int effect, int owner, int x, int y, int z);

int func_801955E8(int mode) {
    s16 nextIndex;
    FieldActor *actor;
    FieldActor *candidate;
    s16 size;
    s16 i;
    s32 pulse;
    s16 *pathAddress;
    s32 pathOffset;

    switch (mode) {
    case 0: {
        D_8019AE6C = 0;
        D_8019AE70 = 0;
        D_8019AE74 = (Inv_ScrambleGrid() & 7) + 4;
        D_8019AE78 = (Inv_ScrambleGrid() & 7) + 4;
        return func_800CE560(D_800F33E0->pool, 100, 10, func_80194E6C);
    }
    case 1: {
        s16 pathY[2];
        RenderMatrix *matrices;

        if (D_8019AEF8)
            return 2;
        candidate = ((RoomM273RingController *)D_800F32D0)->actor;
        if (candidate->mode != 11)
            return 0;
        actor = candidate;
        {
            u16 previousFrame =
                *(u16 *)((u8 *)actor + 0x1A);
        if (*(s16 *)((u8 *)actor + 0x16) < 6 ||
            (s16)previousFrame >= 35)
            return 0;
        }
        {
            s16 countdown = D_8019AE70;
            D_8019AE70 = countdown - 1;
            if (countdown > 0)
                return 0;
        }

        if (D_8019AEFA == 0)
            goto deterministicPattern;
        if (D_8019AEFA == mode)
            goto randomPattern;
        goto phasePattern;

deterministicPattern: {
            register s16 phase asm("$2") = D_8019AE6C;
            s16 offset;
            pulse = (u16)phase << 16;
            offset = (pulse >> 18) * 32 - 0x300;
            phase &= 3;
            asm("");
            pathY[1] = offset;
            pathY[0] = offset;
            pulse = (pulse >> 15) + 0x80;
            D_8019ACA8 = pulse;
            PE1_COMPILER_MEMORY_BARRIER();
            size = phase << 6;
            goto particleSetup;
        }
randomPattern: {
            int random = Inv_ScrambleGrid() & 0x3F;
            int center = 0x20 - random;
                if (D_8019AE6C & 1) {
                    pathY[0] = center - 0x340;
                    PE1_COMPILER_MEMORY_BARRIER();
                    pathY[1] = center - 0x2A0;
                } else {
                    pathY[1] = center - 0x2A0;
                    PE1_COMPILER_MEMORY_BARRIER();
                    pathY[0] = center - 0x340;
            }
            size = 0x10;
            D_8019ACA8 = 0x80;
            goto particleSetup;
        }
phasePattern: {
            u16 phase = D_8019AE6C;
            register int offset asm("$3");
            offset = (phase & 3) * 24;
                if (phase & 1) {
                    s16 pathValue;
                    pathValue = offset - 0x3C0;
                    pathY[0] = pathValue;
                    PE1_COMPILER_MEMORY_BARRIER();
                    pathValue = offset - 0x2A0;
                    pathY[1] = pathValue;
                } else {
                    s16 pathValue;
                    pathValue = offset - 0x2A0;
                    pathY[1] = pathValue;
                    PE1_COMPILER_MEMORY_BARRIER();
                    pathValue = offset - 0x3C0;
                    pathY[0] = pathValue;
            }
            size = 0x20;
            D_8019ACA8 = 0x80;
        }

particleSetup:
        i = 0;
        do {
            RoomM273FallingTrail *particle = func_800CE610(D_800F33E0->pool);
            if (particle == 0)
                break;
            particle->position.x = D_8019AE9C.hands[i].x;
            particle->position.y = D_8019AE9C.hands[i].y;
            pathOffset = i * 2;
            particle->position.z = D_8019AE9C.hands[i].z;
            particle->yaw = actor->rot_y + size * 4;
            PE1_COMPILER_MEMORY_BARRIER();
            pathAddress = (s16 *)((u8 *)pathY + pathOffset);
            nextIndex = i + 1;
            particle->pitch = *pathAddress;
            i = nextIndex;
            particle->spin = size;
            particle->landed = 0;
            particle->head = 0;
            particle->count = 0;
            size = -size;
        } while (nextIndex < 2);

        D_8019AE70 = 6;
        D_8019AE6C++;
        matrices = actor->render_object.matrices;
        func_8006DCE4(0x5D0, ((int *)actor->state)[2],
                      ((s16 *)matrices[2].translation)[0],
                      ((s16 *)matrices[2].translation)[2],
                      ((s16 *)matrices[2].translation)[4]);
        return 0;
    }
    case 2: {
        u16 drawSize = *(u16 *)&D_800F3368;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3376 = drawSize;
        D_800F3378 = drawSize;
        break;
    }
    }
    return 0;
}
