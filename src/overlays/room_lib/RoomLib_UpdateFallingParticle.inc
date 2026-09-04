#include "common.h"
#include "pe1/room_fx.h"

extern s32 D_8009D248;
extern u16 D_8009D1CC;
extern u16 D_800942EC;
extern u8 ROOMLIB_FALLING_PARTICLE_CONFIG[];
extern u8 ROOMLIB_FALLING_PARTICLE_CALLBACK[];

int Geo_PointInPoly(s32 x, s32 z, s32 arg2, s32 arg3);
u16 *func_800C2B90(void *owner, int kind, void *config, void *callback);

void ROOMLIB_UPDATE_FALLING_PARTICLE_NAME(
    void *owner, RoomFallingParticleControl *control,
    RoomFallingParticleState *state) {
    register u16 *spawn asm("$3");

    state->x += state->velocityX;
    state->y += state->velocityY;
    state->z += state->velocityZ;
    state->phase += 30;
    if (state->intensity < 129) {
        state->intensity += 10;
    }

    state->frame++;
    if (Geo_PointInPoly((s16)state->x << 16, (s16)state->z << 16,
                        D_8009D248, D_8009D1CC)) {
        if ((s8)state->frame % 3 == 0) {
            spawn = func_800C2B90(owner, 3,
                                  ROOMLIB_FALLING_PARTICLE_CONFIG,
                                  ROOMLIB_FALLING_PARTICLE_CALLBACK);
            if (spawn != 0) {
                spawn[4] = state->x;
                asm volatile("" ::: "memory");
                spawn[5] = D_800942EC;
                spawn[6] = state->z;
            }
        }
    }

    if (!Geo_PointInPoly((s16)state->x << 16, (s16)state->z << 16,
                         D_8009D248, D_8009D1CC)) {
        control->state = 2;
        state->active = 0;
    }
}

#undef ROOMLIB_UPDATE_FALLING_PARTICLE_NAME
#undef ROOMLIB_FALLING_PARTICLE_CONFIG
#undef ROOMLIB_FALLING_PARTICLE_CALLBACK
