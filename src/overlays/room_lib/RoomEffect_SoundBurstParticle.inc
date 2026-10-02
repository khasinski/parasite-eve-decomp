#include "pe1/room_sound_burst.h"
#ifndef ROOMEFFECT_SOUND_BURST_PARTICLE_FUNC
#error "ROOMEFFECT_SOUND_BURST_PARTICLE_FUNC must name the room entry point"
#endif

extern RoomSoundBurstColor D_8018F1CC;
extern RoomSoundBurstParams D_800F3368;
extern u16 D_800F336C;
extern s16 D_800F336A;

/* Particle emitted by the sound burst controller: it swings on a sine
 * while rising, lights the battle actor when it lands near it, and draws a
 * cosine-sized sprite with its floor shadow. */
int ROOMEFFECT_SOUND_BURST_PARTICLE_FUNC(int mode,
                                         RoomSoundBurstParticle *particle,
                                         s32 *step) {
    GteShortVector position;
    RoomSoundBurstColor color = D_8018F1CC;
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
        particle->wy = D_800942EC;
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

#undef ROOMEFFECT_SOUND_BURST_PARTICLE_FUNC
