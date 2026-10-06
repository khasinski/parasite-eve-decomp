/*
 * The falling sprite burst: a sprite and its floor shadow fall until they
 * reach the floor, then spawn an impact shimmer, a ground pulse and six
 * particles that fall back to the floor.
 *
 * Ten hospital and Chrysler rooms link the same fifteen functions in this
 * order, with the same 0x38 bytes of read-only seeds; this unit is that
 * object, compiled into each of them. The five sprite records and the spawn
 * tables live in each room's own data.
 */
#include "pe1/room_falling_burst.h"

static const RoomFxSeed8 s_FallingSpritePositionSeed = {
    { 0x00, 0x00, 0x14, 0x00, 0xD8, 0xFF, 0x00, 0x00 }
};
static const RoomFxSeed8 s_FallingSpriteVelocitySeed = {
    { 0x00, 0x00, 0x00, 0x00, 0xFA, 0xFF, 0x00, 0x00 }
};
static const RoomFxVec4 s_FallingShadowScale = { 0x100, 0x100, 0x100, 0 };
static const RoomFxSeed8 s_FloorDecalRotation = {
    { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};
static const RoomFxVec4 s_ImpactParticleScale = { 0x190, 0x190, 0x190, 0 };

/* Sets up the burst state and the room's five sprite records. */
void RoomFx_FallingBurstInit(void *arg0, int unused, void *arg2) {
    char *state = arg2;
    char *self = arg0;

    func_800C2B40(state);
    *(int *)(state + 0x10) = (int)func_8006DC18(0x28);
    *(short *)(state + 0x6) = 0;
    *(short *)(state + 0x8) = 0;
    *(int *)(state + 0x0) = *(int *)(self + 0x8);
    *(short *)(state + 0xA) = *func_800C2B28(1);
    *(short *)(state + 0xC) = *func_800C2B28(2);
    *(short *)(state + 0x4) = *func_800C2B28(3);

    g_RoomFallingBurstSprite.offset = 0xA;
    g_RoomFallingBurstShimmer.offset = 0x64;
    g_RoomFallingBurstPulse.code = 0x20;
    g_RoomFallingBurstPulse.offset = 0xA0;
    {
        int value24 = 0x24;
        int value1 = 1;
        int value80arg = 0x80;
        register unsigned char value80 asm("$3") = 0x80;
        int value6e = 0x6E;
        g_RoomFallingBurstSprite.code = value24;
        g_RoomFallingBurstSprite.mode = value1;
        g_RoomFallingBurstSprite.depth = value80arg;
        g_RoomFallingBurstSprite.r = value80;
        g_RoomFallingBurstSprite.g = value80;
        g_RoomFallingBurstSprite.b = value80;
        g_RoomFallingBurstSprite.zero = 0;
        g_RoomFallingBurstShimmer.code = 0;
        g_RoomFallingBurstShimmer.mode = 0;
        g_RoomFallingBurstShimmer.depth = value80arg;
        g_RoomFallingBurstShimmer.r = value80;
        g_RoomFallingBurstShimmer.g = value80;
        g_RoomFallingBurstShimmer.b = value80;
        g_RoomFallingBurstShimmer.zero = 0;
        g_RoomFallingBurstPulse.mode = value1;
        g_RoomFallingBurstPulse.depth = value80arg;
        g_RoomFallingBurstPulse.r = value80;
        g_RoomFallingBurstPulse.g = value80;
        g_RoomFallingBurstPulse.b = value80;
        g_RoomFallingBurstPulse.zero = 0;
        g_RoomFallingBurstParticle.code = value24;
        g_RoomFallingBurstParticle.mode = value1;
        g_RoomFallingBurstParticle.offset = value6e;
        g_RoomFallingBurstParticle.depth = value80arg;
        g_RoomFallingBurstParticle.r = value80;
        g_RoomFallingBurstParticle.g = value80;
        g_RoomFallingBurstParticle.b = value80;
        g_RoomFallingBurstParticle.zero = 0;
        g_RoomFallingBurstShadow.offset = value6e;
        g_RoomFallingBurstShadow.depth = 0x30;
        g_RoomFallingBurstShadow.code = value24;
        g_RoomFallingBurstShadow.mode = value1;
        g_RoomFallingBurstShadow.r = 0xA0;
        g_RoomFallingBurstShadow.g = 0xA0;
        g_RoomFallingBurstShadow.b = 0xA0;
        g_RoomFallingBurstShadow.zero = 0;
    }
}

void RoomFx_FallingBurstNop(void) {
}

/* Counts the hold timer down and re-sends the burst when it is rearmed; ends
 * the effect when the script reports completion. */
void RoomFx_FallingBurstTick(int arg0, char *arg1, char *arg2) {
    short *state = (short *)arg2;

    if (state[3] != 0) {
        state[3]--;
    }

    if (state[4] == 1) {
        int timer = state[3];

        state[4] = 0;
        if (timer == 0) {
            state[3] = *(unsigned short *)(arg2 + 4);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}

typedef struct RoomFallingSpriteView {
    unsigned char pad0[0xA0];
    unsigned char transform[0x14];
    int baseX;
    int baseY;
    int baseZ;
} RoomFallingSpriteView;

typedef struct RoomFallingSpriteRoot {
    unsigned char pad0[0x238];
    RoomFallingSpriteView *view;
} RoomFallingSpriteRoot;

typedef struct RoomFallingSpriteState {
    short x;
    short y;
    short z;
    unsigned char pad6[2];
    short velocityX;
    short velocityY;
    short velocityZ;
    unsigned char padE[2];
    short scale;
    unsigned short depth;
    short phase;
    unsigned char active;
} RoomFallingSpriteState;

/* Places the sprite at the actor's offset and gives it a random sideways
 * drift, or the actor's facing when script register 1 is set. */
void RoomFx_InitFallingSprite(void *entity, void *unused,
                              RoomFallingSpriteState *state) {
    RoomFxSeed8 positionSeed;
    unsigned short position[4];
    RoomFxSeed8 velocitySeed;
    unsigned short velocity[4];
    RoomFallingSpriteRoot **root;
    RoomFallingSpriteView *view;

    root = func_800C2B50();
    positionSeed = s_FallingSpritePositionSeed;
    velocitySeed = s_FallingSpriteVelocitySeed;
    view = (*root)->view;
    ApplyMatrixSV(&view->transform, &positionSeed, position);

    state->x = position[0] + view->baseX;
    state->y = position[1] + view->baseY;
    state->z = position[2] + view->baseZ;
    state->scale = 0x100;
    state->depth = 0x80;
    state->phase = 0;
    state->active = 0;

    if (*func_800C2B10(1) == 0) {
        state->velocityX = func_80071A54() % 10 - 5;
        state->velocityY = 0;
        state->velocityZ = func_80071A54() % 10 - 5;
    } else {
        ApplyMatrixSV(&view->transform, &velocitySeed, velocity);
        state->velocityX = velocity[0] * 2;
        state->velocityY = 0;
        state->velocityZ = velocity[2] * 2;
    }

    func_800C6800(entity, 0x583, state);
}

/* Draws the sprite stretched by its fall speed, then its shadow on the
 * floor. */
void RoomFx_DrawFallingSprite(void *unused0, void *unused1,
                              RoomSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    unsigned short *depthSlot;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x10, 0x10);
    func_800C3098(0x10);
    func_800C3238(0);

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

    func_80071A44(&scale, 0, 0x10);
    scale.x = 0x100;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratchScale = scale;
    ScaleMatrix(&matrix, &scratchScale);

    depthSlot = &g_RoomFallingBurstSprite.depth;
    *depthSlot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y - ((fx->scale - 0x100) >> 4);
    matrix.t[2] = fx->z;
    func_800C42A4((char *)depthSlot - 0xA, &matrix, 1);

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

    scale = s_FallingShadowScale;
    ScaleMatrix(&matrix, &scale);
    func_800C3238(2);
    matrix.t[0] = fx->x;
    matrix.t[1] = g_FrameCount16.height;
    matrix.t[2] = fx->z;
    func_800C42A4(&g_RoomFallingBurstShadow, &matrix, 1);
}

typedef struct RoomFallingSpriteMotion {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    unsigned char pad6[2];
    unsigned short velocityX;
    unsigned char padA[2];
    unsigned short velocityZ;
    unsigned char padE[2];
    unsigned short scale;
    unsigned char phase;
    unsigned char pad13;
    unsigned short verticalStep;
    unsigned char active;
} RoomFallingSpriteMotion;

/* Moves the sprite until it reaches the floor; there it shrinks away and,
 * once, spawns the impact shimmer (kind 3), the ground pulse (kind 2) and
 * the six particles (kind 4) at the landing point. */
void RoomFx_UpdateFallingSprite(void *entity, RoomFallingBurstControl *control,
                                RoomFallingSpriteMotion *effect) {
    void *self = entity;
    RoomFallingSpriteMotion *state;
    RoomFallingBurstParticles *burst;
    unsigned short *frame;
    unsigned int i;
    int belowFloor;

    belowFloor = (short)effect->y < g_FrameCount16.height;
    state = effect;
    if (!belowFloor) {
        if ((short)effect->scale >= 0x15) {
            effect->scale -= 0x14;
        } else {
            control->state = 2;
        }

        if (state->active == 0) {
            state->active = 1;

            {
                RoomFallingBurstDecal *shimmer = func_800C2B90(
                    self, 3, g_RoomFallingBurstSpawnScript,
                    g_RoomFallingBurstSpawnData);
                if (shimmer != 0) {
                    shimmer->x = state->x;
                    shimmer->y = g_FrameCount16.height;
                    shimmer->z = state->z;
                }
            }

            {
                RoomFallingBurstDecal *pulse = func_800C2B90(
                    self, 2, g_RoomFallingBurstSpawnScript,
                    g_RoomFallingBurstSpawnData);
                if (pulse != 0) {
                    pulse->x = state->x;
                    pulse->y = g_FrameCount16.height;
                    pulse->z = state->z;
                }
            }

            burst = func_800C2B90(
                self, 4, g_RoomFallingBurstSpawnScript,
                g_RoomFallingBurstSpawnData);
            if (burst != 0) {
                i = 0;
                frame = (unsigned short *)&g_FrameCount16;
                do {
                    burst->position[i].x = state->x;
                    burst->position[i].y = *frame;
                    burst->position[i].z = state->z;
                    i++;
                } while (i < 6);
            }
        }
    } else {
        register RoomFallingSpriteMotion *motion asm("$16") = state;
        unsigned short *motionWords;
        PE1_COMPILER_LAUNDER(motion);
        motionWords = (unsigned short *)motion;
        motionWords[0] += motionWords[4];
        motionWords[2] += motionWords[6];
        motionWords[1] += (short)motionWords[10] >> 8;
        motionWords[8] += 0x28;
        motionWords[10] += 0x64;
    }
}

void RoomFx_InitImpactShimmer(int a, int b, RoomFallingBurstDecal *c) {
    c->shimmerScale = 0x258;
    c->shimmerDepth = 0x80;
    c->shimmerAlpha = 0;
}

/* Draws the shimmer flat on the floor. */
void RoomFx_DrawImpactShimmer(void *arg0, void *arg1, RoomSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxSeed8 seed;
    RoomFxVec4 scratch_scale;
    RoomFxVec4 scale;
    char *owner;
    u16 *depth_slot;

    owner = func_800C2B50();
    seed = s_FloorDecalRotation;
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(1);
    RotMatrix(&seed, &matrix);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x1000;
    scratch_scale = scale;
    ScaleMatrix(&matrix, &scratch_scale);

    depth_slot = &g_RoomFallingBurstShimmer.depth;
    *depth_slot = fx->depth;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    g_RoomFallingBurstShimmer.code = fx->alpha << 1;
    func_800C42A4((char *)depth_slot - 0xA, &matrix, 0);
}

/* Grows the shimmer; in its last thirty ticks it fades in and ends. */
void RoomFx_UpdateImpactShimmer(int a, RoomFallingBurstControl *st,
                                RoomFallingBurstDecal *c) {
    RoomFallingBurstClock *r = func_800C2B50();
    c->shimmerScale += 3;
    if (r->tick - 0x1E < st->endTick) {
        unsigned char v = c->shimmerPhase + 8;
        c->shimmerPhase = v;
        c->shimmerAlpha = v >> 4;
        if (v >> 4 == 8) {
            st->state = 2;
        }
    }
}

void RoomFx_InitGroundPulse(int a, int b, RoomFallingBurstDecal *c) {
    c->pulseScale = 0xC8;
    c->pulseDepth = 0x80;
}

/* Draws the pulse flat on the floor. */
void RoomFx_DrawGroundPulse(void *unused0, void *unused1,
                            RoomUniformSpriteFxParams *fx) {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomSpriteMatrix *matrixPtr;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    u16 *depthSlot;

    owner = func_800C2B50();
    matrixPtr = &matrix;
    seed = s_FloorDecalRotation;
    func_800C2EAC(owner[0x10]);
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(1);
    RotMatrix(&seed, matrixPtr);

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = fx->scale;
    scratchScale = scale;
    ScaleMatrix(matrixPtr, &scratchScale);

    depthSlot = &g_RoomFallingBurstPulse.depth;
    *depthSlot = fx->depth;
    matrixPtr->t[0] = fx->x;
    matrixPtr->t[1] = fx->y;
    matrixPtr->t[2] = fx->z;
    func_800C42A4((char *)depthSlot - 0xA, matrixPtr, 0);
}

/* Expands the pulse with the clock, fades it in its last sixty ticks and
 * flags the clock when the pulse touches the player. */
void RoomFx_UpdateGroundPulse(int unused, RoomFallingBurstControl *state,
                              RoomFallingBurstDecal *fx) {
    RoomFallingBurstClock *clock = func_800C2B50();
    if (clock->tick - 0x3C < state->endTick) {
        if (fx->pulseDepth >= 5) {
            fx->pulseDepth -= 4;
        }
        if (clock->tick - 0x1E < state->endTick &&
            fx->pulseScale > clock->step * 6) {
            fx->pulseScale -= clock->step * 6;
        }
    } else {
        fx->pulseScale += clock->step;
    }
    if (func_800C6B90(fx, fx->pulseScale >> 3) != 0) {
        clock->hit = 1;
    }
    if (state->endTick == clock->tick) {
        state->state = 2;
    }
}

/* Launches the six particles upwards with random spreads. */
void RoomFx_InitImpactParticles(int unused, void *state,
                                RoomFallingBurstParticles *fx) {
    RoomFxVec4 scale;
    unsigned int i;
    fx->matrix.m[2][2] = 0x1000;
    fx->matrix.m[1][1] = 0x1000;
    fx->matrix.m[0][0] = 0x1000;
    fx->matrix.t[2] = 0;
    fx->matrix.t[1] = 0;
    fx->matrix.t[0] = 0;
    fx->matrix.m[2][1] = 0;
    fx->matrix.m[2][0] = 0;
    fx->matrix.m[1][2] = 0;
    fx->matrix.m[1][0] = 0;
    fx->matrix.m[0][2] = 0;
    fx->matrix.m[0][1] = 0;
    scale = s_ImpactParticleScale;
    ScaleMatrix(&fx->matrix, &scale);
    fx->liveCount = 6;
    for (i = 0; i < 6; i++) {
        fx->active[i] = 1;
        fx->depth[i] = 0x80;
        fx->velocity[i].x = func_80071A54() % 31 - 15;
        fx->velocity[i].y = -(func_80071A54() % 100 + 50);
        fx->velocity[i].z = func_80071A54() % 31 - 15;
    }
}

/* Draws each live particle and its dimmer reflection on the floor. */
void RoomFx_DrawImpactParticles(int unused, void *state,
                                RoomFallingBurstParticles *fx) {
    RoomFallingBurstClock *clock = func_800C2B50();
    unsigned int i;
    func_800C2EAC(clock->renderOwner);
    func_800C2FF0(0x10, 0x10);
    func_800C3098(0x10);
    func_800C3238(1);
    for (i = 0; i < 6; i++) {
        if (fx->active[i] == 1) {
            fx->matrix.t[0] = (short)fx->position[i].x;
            fx->matrix.t[1] = (short)fx->position[i].y;
            fx->matrix.t[2] = (short)fx->position[i].z;
            g_RoomFallingBurstParticle.depth = fx->depth[i];
            func_800C42A4(&g_RoomFallingBurstParticle, &fx->matrix, 1);
            g_RoomFallingBurstParticle.depth = (short)fx->depth[i] >> 2;
            fx->matrix.t[1] = g_FrameCount16.height;
            func_800C42A4(&g_RoomFallingBurstParticle, &fx->matrix, 1);
        }
    }
}

/* Per-frame particle step: velocity/16 integration, +4 gravity, kill on
 * floor hit; when the last one dies the effect ends. */
void RoomFx_UpdateImpactParticles(int a, unsigned char *st,
                                  RoomFallingBurstParticles *sys) {
    int i;
    int grav;
    unsigned short *gp;
    for (i = 0; (unsigned)i < 6; i++) {
        grav = 4;
        if (sys->active[i] == 1) {
            int vx = (short)sys->velocity[i].x >> grav;
            int vy = (short)sys->velocity[i].y >> grav;
            int vz = (short)sys->velocity[i].z >> grav;
            sys->position[i].x += vx;
            sys->position[i].y = sys->position[i].y + vy;
            sys->position[i].z += vz;
            gp = (unsigned short *)&g_FrameCount16;
            sys->velocity[i].y += grav;
            if ((short)sys->position[i].y >= (short)*gp) {
                short r;
                sys->active[i] = 0;
                sys->liveCount = (r = sys->liveCount - 1);
                if (r == 0) {
                    st[1] = 2;
                }
            }
        }
    }
}
