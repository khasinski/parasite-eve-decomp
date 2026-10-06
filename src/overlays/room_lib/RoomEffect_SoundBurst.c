/*
 * The sound burst: a particle that swings up and lights the battle actor,
 * and the controller that spawns it and plays the burst's sound. The two
 * functions follow each other in museum rooms m205, m231, m234 and m243 and
 * Chrysler rooms m406 and m410; the unit's rodata is the particle's colour
 * and the controller's spawn template.
 */
#include "common.h"
#include "pe1/room_sound_burst.h"

extern RoomSoundBurstParams D_800F3368;
extern u16 D_800F336C;
extern s16 D_800F336A;

/* Particle emitted by the sound burst controller: it swings on a sine
 * while rising, lights the battle actor when it lands near it, and draws a
 * cosine-sized sprite with its floor shadow. */
int RoomEffect_SoundBurstParticle(int mode, RoomSoundBurstParticle *particle,
                                  s32 *step) {
    GteShortVector position;
    RoomSoundBurstColor color = {0x40, 0x00, 0x80, 0x00};
    GteShortVector *world;
    int swing;
    int scale;
    int base;
    int size;
    u16 kind;
    int palette;
    int clut;
    int clut2;

    switch (mode) {
    case 1:
        particle->swing += step[0];
        particle->amplitude += step[1];
        particle->phase += step[2];
        swing = rsin((s16)particle->phase) * (s16)particle->amplitude / 4096;
        world = (GteShortVector *)&particle->wx;
        position.x = 0;
        position.y = particle->height;
        position.z = 0;
        func_800CF844(particle, world, (s16)particle->swing, &position, swing,
                      0);
        if (func_800C6B90(world, 100) != 0) {
            if (D_800E2368->active) {
                RoomSoundBurstChannel *channel = D_800F32D0;
                u32 *entry;
                if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                    D_8009D254->actor->flags |= 0x4000;
                    entry = *(u32 **)channel->pool;
                    *entry = (*entry & 0xC0FFFFFF) | 0x21000000;
                    entry = *(u32 **)channel->pool;
                    *entry |= 0x80000000;
                }
            }
        }
        if (D_800E27EC < 0x40) break;
        return 1;
    case 2:
        scale = rcos(D_800E27EC << 4) / 32;
        scale = particle->spread * scale / 128;
        base = (D_800E27EC << 8) + 0x1000;
        size = (rcos(particle->tilt) / 4 + 0x1000) * base / 4096;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x80, palette);
        func_800CEE20(&particle->wx, 0, size, size,
                      D_800F336A * ((D_800E27EC / 4) & 1) + 0xFD, clut, 1,
                      scale, &color);
        position.x = 0x400;
        position.y = 0;
        position.z = 0;
        position.pad = 1;
        particle->wy = g_RoomFloorY->y;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut2 = func_80077AA4(0x80, palette);
        func_800CEE20(&particle->wx, &position, size, size,
                      D_800F336A * ((D_800E27EC / 4) & 1) + 0xFD, clut2, 2,
                      scale / 4, 0);
        break;
    }
    return 0;
}

int RoomEffect_SoundBurstController(int mode, RoomSoundBurstState *state) {
    RoomSoundBurstTemplate8 template = {{0x00000000, 0x0000FFC4}};
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomSoundBurstParticle *child;
    int angle;

    switch (mode) {
    case 0:
        state->soundHandle = func_800D3F64(0x5A1, func_800D3FD8());
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 24, 24,
                             (FieldAnimCallbackListCallback)RoomEffect_SoundBurstParticle);
    case 1:
        state->frame++;
        if (D_800E27EC < 0x47) {
            pool = D_800F32D0->pool;
            func_800CE8F0(pool, 9, &template, position);
            angle = rsin((D_800E27EC << 11) / 70) / 32;
            if (D_800E27EC % 3 == 0) {
                pool = D_800F33E0->pool;
                child = func_800CE610(pool);
                if (child) {
                    pool = D_800F32D0->pool;
                    func_800CE9D4((struct RoomFxTransformOwner *)pool, 0,
                                  (GteShortVector *)target);
                    child->height = target[1];
                    child->x = position[0];
                    child->y = position[1];
                    child->z = position[2];
                    child->phase = 0;
                    child->amplitude = 0;
                    child->swing = 0;
                    child->tilt = func_80071A54();
                    child->spread = angle;
                }
                if (D_800E2368->active) {
                    RoomSoundBurstNode **slot =
                        (RoomSoundBurstNode **)D_800F32D0->pool;
                    if (slot && *slot) {
                        u8 *flag = (*slot)->state;
                        if (*flag == 1) *flag = 2;
                    }
                }
            }
        }
        if (D_800E27EC == 0x46 && state->soundHandle != -1) {
            func_800866A4(state->soundHandle, 0);
        }
        if (D_800E27EC < 8) break;
        return 2;
    case 2:
    {
        int idx = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        palette = D_800E2850[idx];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
        break;
    }
    }
    return 0;
}

#undef RoomEffect_SoundBurstParticle
