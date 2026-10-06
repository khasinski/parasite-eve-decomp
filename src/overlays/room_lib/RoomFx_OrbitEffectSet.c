/* CC1_FLAGS: -fno-strength-reduce */
/*
 * The orbit effect set: eight particles that orbit outwards from the actor,
 * a burst of eight sprites that fall to the floor over their shadows, and a
 * sprite that drifts away from the actor. Each effect is an init, draw and
 * update handler; the set's own init fills the four sprite records.
 *
 * room_m186, m187, m385, m388 and m389 link these twelve functions in this
 * order, with the same 0x20 bytes of read-only seeds; this unit is that
 * object, compiled into each of them. It starts after the no-op that ends
 * the room prefix. The four sprite records live in each room's own data.
 */
#include "pe1/room_orbit_set.h"
#include "RoomLib_RenderLayouts.h"

typedef struct RoomOrbitInitWords {
    int words[8];
} RoomOrbitInitWords;

/* Copies the owner's eight words into the state, allocates the set's
 * buffer and fills the four sprite records. The register pins and launders
 * reproduce retail's assignment of the shared constants. */
void RoomFx_OrbitSetInit(char *object, void *unused, char *state) {
    char *owner;
    void *allocation;
    RoomOrbitInitWords *source;
    register int callObject asm("$4");
    register int value asm("$2");
    int value128a;
    int value128b;
    int value32;
    register int value1 asm("$7");
    register int valueNeg42 asm("$5");

    func_800C2B40(state);
    owner = *(char **)(object + 8);
    *(char **)state = owner;
    source = *(RoomOrbitInitWords **)(owner + 0x238);
    *(RoomOrbitInitWords *)(state + 4) = *source;
    allocation = func_8006DC18(0x24);

    callObject = (int)object;
    *(void **)(state + 0x24) = allocation;
    value = -0x29;
    value128a = 0x80;
    value128b = 0x80;
    value32 = 0x20;
    value1 = 1;
    valueNeg42 = -0x2A;
    PE1_COMPILER_LAUNDER(value128a);
    PE1_COMPILER_LAUNDER(value128b);
    PE1_COMPILER_LAUNDER(value32);
    PE1_COMPILER_LAUNDER(value1);
    g_RoomOrbitParticlePacket.offset = value;
    value = 0x60;
    g_RoomOrbitBurstPacket.r = value;
    value = 0x40;
    asm volatile("" : "=r"(state) : "0"(state), "r"(value));
    state += 4;
    PE1_COMPILER_USE(state);
    g_RoomOrbitBurstPacket.offset = valueNeg42;
    g_RoomOrbitShadowPacket.offset = valueNeg42;
    g_RoomOrbitSpritePacket.offset = valueNeg42;
    g_RoomOrbitParticlePacket.depth = value128a;
    g_RoomOrbitBurstPacket.depth = value128a;
    g_RoomOrbitSpritePacket.depth = value128a;
    g_RoomOrbitParticlePacket.code = 0;
    g_RoomOrbitParticlePacket.mode = 0;
    g_RoomOrbitParticlePacket.r = value128b;
    g_RoomOrbitParticlePacket.g = value128b;
    g_RoomOrbitParticlePacket.b = value128b;
    g_RoomOrbitParticlePacket.zero = 0;
    g_RoomOrbitBurstPacket.code = value32;
    g_RoomOrbitBurstPacket.mode = value1;
    g_RoomOrbitBurstPacket.g = value;
    g_RoomOrbitBurstPacket.b = value;
    g_RoomOrbitBurstPacket.zero = 0;
    g_RoomOrbitShadowPacket.code = value32;
    g_RoomOrbitShadowPacket.mode = value1;
    g_RoomOrbitShadowPacket.depth = 0;
    g_RoomOrbitShadowPacket.r = 0;
    g_RoomOrbitShadowPacket.g = 0;
    g_RoomOrbitShadowPacket.b = 0;
    g_RoomOrbitShadowPacket.zero = 0;
    g_RoomOrbitSpritePacket.code = 0;
    g_RoomOrbitSpritePacket.mode = 0;
    g_RoomOrbitSpritePacket.r = value128b;
    g_RoomOrbitSpritePacket.g = value128b;
    g_RoomOrbitSpritePacket.b = value128b;
    g_RoomOrbitSpritePacket.zero = 0;
    func_800C66C8((void *)callObject, 0x592, state);
}

/* The set itself draws nothing; each effect draws its own sprites. */
void RoomFx_OrbitSetDraw(void) {
}

void RoomFx_OrbitSetUpdate(void *arg0, char *event) {
    if (func_800C2B68() == 1) {
        event[1] = 2;
    }
}

void RoomFx_InitOrbitParticles(void *unused, void *unused2, RoomOrbitParticleState *state) {
    char *clock = func_800C2B50();
    unsigned int i;
    RoomOrbitParticleLaneView *particle;

    i = 0;
    particle = (RoomOrbitParticleLaneView *)state;
    while (i < 8) {
        int angle;

        *(u16 *)&particle->position.x = *(s32 *)(clock + 0x18);
        particle->position.y = (u16)g_RoomFloorY->y;
        particle->position.z = *(s32 *)(clock + 0x20);

        angle = func_80071A54() % 0x1000;
        particle->velocity.x = rsin(angle);
        angle = func_80071A54() % 0x1000;
        particle->velocity.z = rcos(angle);
        particle->velocity.angle = -(func_80071A54() % 8000 + 4000);
        i++;
        particle = (RoomOrbitParticleLaneView *)(&particle->position + 1);
    }

    state->decay = 0x80;
    state->height = 0x400;
    state->radiusStep = -30;
    state->intensity = 0;
    state->radius = 0x258;
}

typedef struct RoomOrbitBatchFxParams {
    struct {
        short x;
        short y;
        short z;
        short pad6;
    } positions[8];
    unsigned char pad40[0x40];
    short scale;
    unsigned short depth;
    unsigned char alpha;
} RoomOrbitBatchFxParams;


void RoomFx_DrawOrbitParticles(
    void *unused0, void *unused1, RoomOrbitBatchFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    unsigned short *depthSlot;
    short *position;
    unsigned int i;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    i = 0;
    depthSlot = (unsigned short *)&g_RoomOrbitParticlePacket.depth;
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
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

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x400;
    scratchScale = scale;
    ScaleMatrix(&matrix, &scratchScale);

    position = &fx->positions[0].x;
    while (i < 8) {
        matrix.t[0] = position[0];
        matrix.t[1] = position[1];
        matrix.t[2] = position[2];
        *depthSlot = (short)fx->depth >> 1;
        *((unsigned char *)depthSlot - 6) = fx->alpha << 1;
        func_800C42A4((char *)depthSlot - 0xA, &matrix, 1);
        i++;
        position += 4;
    }
}

void RoomFx_UpdateOrbitParticles(
    void *unused, u8 *control, RoomOrbitParticleState *state) {
    unsigned int i;
    RoomOrbitParticleLaneView *lane;
    u8 intensity;
    u16 height;

    state->radius += state->radiusStep;
    if (state->decay >= 9) {
        state->decay -= 8;
    }

    i = 0;
    lane = (RoomOrbitParticleLaneView *)state;
    while (i < 8) {
        s32 xDelta = (lane->velocity.x * state->radius) >> 16;

        lane->position.y += (s8)(lane->velocity.angle >> 8);
        lane->position.x += xDelta;
        lane->position.z += (lane->velocity.z * state->radius) >> 16;
        lane->velocity.angle += 0x258;
        i++;
        lane = (RoomOrbitParticleLaneView *)(&lane->position + 1);
    }

    height = state->height + 0x46;
    intensity = state->intensity + 6;
    state->intensity = intensity;
    intensity >>= 4;
    state->height = height;
    state->frame = intensity + 1;
    if (intensity == 7) {
        state->frame = 0;
        control[1] = 2;
    }
}

void RoomFx_InitOrbitBurst(
    void *unused0, void *unused1, RoomOrbitBurstState *state) {
    RoomLibOrbitView *view;
    short *particle;
    unsigned int i;

    view = func_800C2B50();
    i = 0;
    particle = (short *)state;
    while (i < 8) {
        particle[0] = view->baseX;
        particle[1] = g_RoomFloorY->y;
        particle[2] = view->baseZ;
        particle[0x20] = func_80071A54() % 0x2000 - 0x1000;
        particle[0x22] = func_80071A54() % 0x2000 - 0x1000;
        particle[0x21] = -(func_80071A54() % 8000 + 8000);
        particle[0x40] = 0;
        particle[0x41] = 0;
        particle[0x42] = func_80071A54() % 0x1000;
        state->active[i] = 1;
        state->frame[i] = func_80071A54() % 3;
        i++;
        particle += 4;
    }

    state->depth = 0x80;
    state->scale = 0x200;
    state->phaseStep = 0xC8;
    state->count = 8;
}

void RoomFx_DrawOrbitBurst(
    void *unused0, void *unused1, RoomOrbitBurstState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    RoomOrbitBurstVector *position;
    unsigned char *configA;
    unsigned char *configB;
    unsigned int i;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    i = 0;
    configA = &g_RoomOrbitBurstPacket.code;
    configB = &g_RoomOrbitShadowPacket.code;
    PE1_COMPILER_USE(configB);
    position = state->position;
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    while (i < 8) {
        if (state->active[i] == 1) {
            RotMatrix(&state->secondary[i], &matrix);
            func_80071A44(&scale, 0, 0x10);
            scale.x = state->scale;
            scale.y = state->scale;
            scale.z = 0x400;
            scratchScale = scale;
            ScaleMatrix(&matrix, &scratchScale);

            matrix.t[0] = position->x;
            matrix.t[1] = position->y;
            matrix.t[2] = position->z;
            *configA = state->frame[i] * 2 + 0x20;
            func_800C42A4(configA - 4, &matrix, 1);

            matrix.t[0] = position->x;
            matrix.t[1] = g_RoomFloorY->y;
            matrix.t[2] = position->z;
            *configB = state->frame[i] * 2 + 0x20;
            func_800C42A4(configB - 4, &matrix, 0);
        }
        i++;
        position++;
    }
}

void RoomFx_UpdateOrbitBurst(
    void *unused, unsigned char *control, RoomOrbitBurstState *state) {
    unsigned char *particle;
    unsigned int i;

    if ((short)state->depth >= 9) {
        state->depth -= 8;
    }

    i = 0;
    particle = (unsigned char *)state;
    while (i < 8) {
        if (state->active[i] == 1) {
            int xStep =
                (signed char)(*(unsigned short *)(particle + 0x40) >> 8);
            int yStep =
                (signed char)(*(unsigned short *)(particle + 0x42) >> 8);
            int zStep =
                (signed char)(*(unsigned short *)(particle + 0x44) >> 8);

            *(unsigned short *)(particle + 0) += xStep;
            *(unsigned short *)(particle + 2) += yStep;
            *(unsigned short *)(particle + 4) += zStep;
            *(unsigned short *)(particle + 0x42) += 0x190;
            *(unsigned short *)(particle + 0x84) +=
                func_80071A54() % 0x200;
            if (*(short *)(particle + 2) > g_RoomFloorY->y) {
                state->count--;
                state->active[i] = 0;
            }
        }
        i++;
        particle += 8;
    }

    if (state->count == 0) {
        control[1] = 2;
    }
}

/* Sprite and floor-point offsets from the actor, for the two sides. */
static const RoomFxSeed8 s_OrbitSpriteSeed0 = {
    { 0xB8, 0x0B, 0x00, 0x00, 0x70, 0x17, 0x00, 0x00 }
};
static const RoomFxSeed8 s_OrbitSpriteSeed1 = {
    { 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};
static const RoomFxSeed8 s_OrbitSpriteSeed2 = {
    { 0x48, 0xF4, 0x00, 0x00, 0x70, 0x17, 0x00, 0x00 }
};
static const RoomFxSeed8 s_OrbitSpriteSeed3 = {
    { 0x38, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

void RoomFx_InitOrbitSprite(
    void *unused0, void *unused1, RoomOrbitSpriteFxParams *state) {
    RoomFxSeed8 seed0;
    RoomFxSeed8 seed1;
    RoomFxSeed8 seed2;
    RoomFxSeed8 seed3;
    unsigned short position[4];
    RoomLibOrbitView *view;

    view = func_800C2B50();
    seed0 = s_OrbitSpriteSeed0;
    seed1 = s_OrbitSpriteSeed1;
    seed2 = s_OrbitSpriteSeed2;
    seed3 = s_OrbitSpriteSeed3;

    if (*func_800C2B10(1) == 0) {
        ApplyMatrixSV((GteMatrix *)view->transform, &seed0.vector,
                      (GteShortVector *)&state->pad6[2]);
        ApplyMatrixSV((GteMatrix *)view->transform, &seed1.vector,
                      (GteShortVector *)position);
    } else {
        ApplyMatrixSV((GteMatrix *)view->transform, &seed2.vector,
                      (GteShortVector *)&state->pad6[2]);
        ApplyMatrixSV((GteMatrix *)view->transform, &seed3.vector,
                      (GteShortVector *)position);
    }

    state->x = position[0] + view->baseX;
    PE1_COMPILER_MEMORY_BARRIER();
    state->y = g_RoomFloorY->y;
    state->z = position[2] + view->baseZ;
    state->depth = 0x80;
    state->scale = 0x800;
    *(unsigned short *)state->pad14 = 0x258;
    state->alpha = 0;
}

void RoomFx_DrawOrbitSprite(
    void *unused, void *unused2, RoomOrbitSpriteFxParams *fx) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scratchScale;
    RoomFxVec4 scale;
    char *owner;
    u8 *drawSlot;

    owner = func_800C2B50();
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[0] = matrix.t[1] = matrix.t[2] = 0;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] =
        matrix.m[1][2] = matrix.m[2][0] = matrix.m[2][1] = 0;

    func_80071A44(&scale, 0, 0x10);
    scale.x = fx->scale;
    scale.y = fx->scale;
    scale.z = 0x400;
    scratchScale = scale;
    ScaleMatrix(&matrix, &scratchScale);

    drawSlot = &g_RoomOrbitSpritePacket.code;
    matrix.t[0] = fx->x;
    matrix.t[1] = fx->y;
    matrix.t[2] = fx->z;
    *drawSlot = ((s16)fx->alpha >> 1) << 1;
    g_RoomOrbitSpritePacket.depth = fx->depth;
    func_800C42A4(drawSlot - 4, &matrix, 1);
}

void RoomFx_UpdateOrbitSprite(unsigned char *arg0, unsigned char *signal, RoomFxDriftState *state) {
    RoomFxDriftState *p = state;
    unsigned short counter = state->counter16;

    state->counter16 = counter + 1;
    if ((short)counter >= 16) {
        state->counter16 = 15;
    }

    if (state->limit12 >= 9) {
        state->limit12 -= 8;
    }
    if (state->phase14 >= 21) {
        state->phase14 -= 20;
    }

    p->phase10 -= 0x64;
    p->x += (p->dx * p->phase14) >> 16;
    p->z += (p->dz * p->phase14) >> 16;

    if (*(short *)(signal + 2) >= 21) {
        signal[1] = 2;
    }
}
