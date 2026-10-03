/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

/* One falling sprite: drifts with damped sideways speed, bounces off the
 * floor and spins through a seven-frame strip tinted by the scene track. */
int func_800D4928(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    u8 *track;
    int scale;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 6 / 7;
        state->velocity_z = state->velocity_z * 6 / 7;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        switch (D_800E2368->variables[6]) {
        case 0:
            track = D_800E141C;
            break;
        case 1:
            track = D_800E1444;
            break;
        default:
            track = D_800E146C;
            break;
        }
        func_800CF3AC(track, &color, D_800E27EC * 48 / state->duration);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (D_800E27EC << 7);
        rotation.flags = 0;
        scale = (D_800E27EC << 11) / state->duration + 0x400;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(32, palette);
        func_800CEE20(&position, &rotation, scale, scale,
                      D_800F336A * (D_800E27EC % 7) + 0xE8, clut, 1, 128, &color);
        break;
    }
    return 0;
}

/* Drops one falling sprite per frame for eight frames from above the
 * player (lower in scene variant 0) on an angle that turns by 0x200. */
int func_800D4C24(int mode, RenderPhaseEmitter *state)
{
    GteShortVector position;
    RenderBouncingSprite *sprite;
    int speed;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        return func_800CE560(D_800F33E0->end, 16, 16,
                             (FieldAnimTaskCallback)func_800D4928);
    case 1:
        if (D_800E27EC < 8) {
            for (i = 0; i < 1; i++) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    func_800CE870((char *)D_8009D254, 1, &position.x);
                    sprite->x = position.x;
                    if (D_800E2368->variables[6] == 0)
                        sprite->y = position.y - 600;
                    else
                        sprite->y = position.y - 400;
                    sprite->z = position.z;
                    speed = (rand() & 7) + 32;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 26;
                    sprite->angle = rand();
                    state->phase += 0x200;
                }
            }
        }
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.extent_x *= 2;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    return 0;
}
