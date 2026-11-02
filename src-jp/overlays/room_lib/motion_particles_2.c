/* CC1_FLAGS: -fno-strength-reduce */

/*
 * The motion particle set, part 2 of 3: the particle draw.
 *
 * room_m075, room_m080, room_m082, scene_e09 and scene_e10 link the same
 * fifteen functions in the same order, after the room prelude and before
 * the room library, with the same 0x18 bytes of read-only data. The object
 * is split in three units only because its particle draw needs
 * -fno-strength-reduce, which the setup loop must not get; they are
 * merged once they compile together. Trails, glyphs and lookup codes live
 * in each room's own data (pe1/room_motion_init.h).
 */

#include "common.h"
#include "pe1/field_script_context.h"
#include "pe1/room_fx.h"
#include "pe1/room_motion_init.h"

#include "motion_particles.h"
#include "room_lib.h"

static const RoomFxVec4 s_MotionScale = { 0x590, 0x590, 0x590, 0 };

typedef struct RoomLibMotionScaleScratch {
    RoomFxVec4 value;
    /* Reserve the original stack space after the copied scale vector. */
    char pad[8];
} RoomLibMotionScaleScratch;

void RoomEffect_DrawMotionParticles(void *unused, RoomStatePair *control,
                   RoomMotionState *state)
{
    RoomSpriteMatrix matrix;
    RoomLibMotionScaleScratch scale;
    RoomMotionContext *context;
    RoomMotionTransform *savedSecondary;
    RoomMotionTransform *savedPrimary;
    RoomMotionTransform *current;
    RoomSpriteMatrix *primary;
    RoomSpriteMatrix *secondary;
    u16 *depth;
    char *packet;
    RoomMotionCursor *cursor;
    int i;

    context = FieldFx_GetScriptContext();
    FieldFx_SetSpriteTexturePreset(context->owner);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);

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
    scale.value = s_MotionScale;
    ScaleMatrix(&matrix, &scale.value);

    if (control->h2 == 30) {
        savedSecondary = context->link->p238;
        state->matrix[1] = savedSecondary->secondary;
        savedPrimary = context->link->p238;
        state->matrix[0] = savedPrimary->primary;
    }

    if (control->h2 >= 31) {
        g_RoomMotionGlyph.offset = 0;
        primary = &state->matrix[0];
        secondary = &state->matrix[1];
    } else {
        current = context->link->p238;
        primary = &current->primary;
        secondary = &current->secondary;
    }

    i = 0;
    if (state->count != 0) {
        depth = &g_RoomMotionGlyph.depth;
        packet = (char *)depth - 10;
        cursor = (RoomMotionCursor *)state;
        do {
            *depth = state->depth;
            ((u8 *)depth)[-6] =
                g_RoomMotionLookupCodes[state->lookup[i].id];
            matrix.t[0] = primary->t[0] + cursor->particle.primaryX;
            matrix.t[1] = primary->t[1] + cursor->particle.primaryY;
            matrix.t[2] = primary->t[2] + cursor->particle.primaryZ;
            FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
            matrix.t[0] = secondary->t[0] + cursor->particle.secondaryX;
            matrix.t[1] = secondary->t[1] + cursor->particle.secondaryY;
            matrix.t[2] = secondary->t[2] + cursor->particle.secondaryZ;
            FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
            i++;
            cursor = (RoomMotionCursor *)((RoomMotionPoint *)cursor + 1);
        } while ((unsigned int)i < state->count);
    }
}
