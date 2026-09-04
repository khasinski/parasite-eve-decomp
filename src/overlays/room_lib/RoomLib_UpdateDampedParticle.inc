#include "common.h"
#include "pe1/room_fx.h"

typedef struct RoomDampedParticleState {
    short x;
    short y;
    short z;
    short size;
    short vx;
    short vy;
    short vz;
    short angle;
    short state;
    short timer;
} RoomDampedParticleState;

extern RoomFxSeed8 ROOMLIB_DAMPED_PARTICLE_SEED;
extern char ROOMLIB_DAMPED_PARTICLE_TABLE;
extern short D_800942EC;
extern int D_800E27EC;
extern short D_800F3368;
extern short D_800F336A;
extern unsigned short D_800F336C;
extern short D_800F3376;
extern short D_800F3378;
extern int D_800F3428;
extern unsigned short D_800E1204[];

int func_80077CF4(int angle);
int func_80077DC4(int angle);
int func_80077AA4(int width, int texture);
void func_800CF3AC(void *table, RoomFxSeed8 *out, int frame);
void func_800CEE20(RoomDampedParticleState *state, RoomFxSeed8 *seed,
                   int scaleX, int scaleY, int texture, int handle,
                   int mode, int fade, RoomFxSeed8 *out);

int ROOMLIB_UPDATE_DAMPED_PARTICLE_NAME(
    int mode, RoomDampedParticleState *state) {
    RoomFxSeed8 seed;
    RoomFxSeed8 out;
    int scale;
    int texture;
    int frame;
    void *table;

    seed = ROOMLIB_DAMPED_PARTICLE_SEED;
    switch (mode) {
    case 1:
    {
        unsigned short nextVy;
        int y;

        if (state->state != 0) {
            goto ret0;
        }
        state->timer++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vx = (state->vx * 0xFF) / 0x100;
        state->vz = (state->vz * 0xFF) / 0x100;
        {
            y = state->y;
            nextVy = (unsigned short)state->vy + 1;
            state->vy = nextVy;
            PE1_COMPILER_LAUNDER_MEM(nextVy);
            if (y >= D_800942EC) {
                int bounce = (short)nextVy;
                state->vy = -bounce;
            }
        }
        if (state->timer < 24) {
            goto ret0;
        }
        goto ret1;
    }
    case 2:
        if (state->state != 0) {
            goto ret0;
        }
        scale = func_80077CF4((state->timer << 10) / 24) * 2 + 0x1000;
        func_80077DC4((state->timer * 1204) / 24);
        table = &ROOMLIB_DAMPED_PARTICLE_TABLE;
        PE1_COMPILER_USE(table);
        D_800F3368 = 0x20;
        D_800F336A = 2;
        D_800F3376 = 0x20;
        D_800F3378 = 0x20;
        frame = D_800E27EC;
        *(unsigned short *)&seed.bytes[4] = frame * 24;
        PE1_COMPILER_MEMORY_BARRIER();
        func_800CF3AC(table, &out, state->timer);
        texture = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) {
            texture += 4;
        }
        func_800CEE20(state, &seed, scale, scale,
                      D_800F336A * (state->timer / 3) + 0x20,
                      (unsigned short)func_80077AA4(0x10, texture),
                      1, 0xA0, &out);
        goto ret0;
    default:
        goto ret0;
    }

ret0:
    return 0;
ret1:
    return 1;
}
