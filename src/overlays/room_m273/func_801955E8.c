#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"

typedef struct RoomM273RingParticle {
    RoomM273Vector position;
    s16 pathY;
    s16 yaw;
    s16 size;
    s16 reserved0E;
    u8 reserved10[0x50];
    s16 reserved60;
    s16 reserved62;
} RoomM273RingParticle;

typedef struct RoomM273RingController {
    u8 reserved00[8];
    FieldActor *actor;
} RoomM273RingController;

extern RoomM273RingController *D_800F32D0;
extern RoomM273Vector D_8019AE9C[];
extern RoomM273Vector D_8019AE9E[];
extern RoomM273Vector D_8019AEA0[];
extern s16 D_800F3372;
extern volatile u16 D_800F3376, D_800F3378;
extern s16 D_8019ACA8;
extern s16 D_8019AE6C, D_8019AE70, D_8019AE74, D_8019AE78;
extern u8 D_8019AEF8, D_8019AEFA;
extern int Inv_ScrambleGrid(void);
extern int func_800CE560(void *pool, int capacity, int stride, int (*callback)());
extern void func_8006DCE4(int effect, int owner, int x, int y, int z);
extern int func_80194E6C();

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
        candidate = D_800F32D0->actor;
        if (candidate->mode != 11)
            return 0;
        actor = candidate;
        {
            u16 previousFrame =
                *(volatile u16 *)((u8 *)actor + 0x1A);
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
            RoomM273RingParticle *particle = func_800CE610(D_800F33E0->pool);
            if (particle == 0)
                break;
            particle->position.x = D_8019AE9C[i].x;
            particle->position.y = D_8019AE9E[i].x;
            pathOffset = i * 2;
            particle->position.z = D_8019AEA0[i].x;
            particle->yaw = actor->rot_y + size * 4;
            PE1_COMPILER_MEMORY_BARRIER();
            pathAddress = (s16 *)((u8 *)pathY + pathOffset);
            nextIndex = i + 1;
            particle->pathY = *pathAddress;
            i = nextIndex;
            particle->size = size;
            particle->reserved0E = 0;
            particle->reserved60 = 0;
            particle->reserved62 = 0;
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
