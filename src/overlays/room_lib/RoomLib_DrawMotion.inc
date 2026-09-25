/* CC1_FLAGS: -fno-strength-reduce */
#include "../room_lib/RoomLib_DrawMotion.h"

typedef struct RoomLibMotionScaleScratch {
    RoomFxVec4 value;
    /* Reserve the original stack space after the copied scale vector. */
    char pad[8];
} RoomLibMotionScaleScratch;

void func_80190410(void *unused, RoomStatePair *control,
                   RoomMotionState *state)
{
    RoomSpriteMatrix matrix;
    RoomLibMotionScaleScratch scale;
    RoomMotionContext *context;
    register RoomMotionTransform *source asm("$2");
    RoomSpriteMatrix *primary;
    RoomSpriteMatrix *secondary;
    u16 *depth;
    char *packet;
    RoomMotionCursor *cursor;
    int i;

    context = (RoomMotionContext *)func_800C2B50();
    func_800C2EAC(context->owner);
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
    scale.value = D_8018EFFC;
    func_80078CC4(&matrix, &scale.value);

    if (control->h2 == 30) {
        source = (RoomMotionTransform *)context->link->p238;
        state->matrix[1] = source->secondary;
        source = (RoomMotionTransform *)context->link->p238;
        state->matrix[0] = source->primary;
    }

    if (control->h2 >= 31) {
        D_801940C0 = 0;
        primary = &state->matrix[0];
        secondary = &state->matrix[1];
    } else {
        source = (RoomMotionTransform *)context->link->p238;
        primary = &source->primary;
        secondary = &source->secondary;
    }

    i = 0;
    if (state->count != 0) {
        depth = (u16 *)&D_801940C2;
        packet = (char *)depth - 10;
        cursor = (RoomMotionCursor *)state;
        do {
            *depth = state->depth;
            *(u8 *)((char *)depth - 6) =
                D_80194008[state->lookup[i].id];
            matrix.t[0] = primary->t[0] + cursor->particle.primaryX;
            matrix.t[1] = primary->t[1] + cursor->particle.primaryY;
            matrix.t[2] = primary->t[2] + cursor->particle.primaryZ;
            func_800C42A4(packet, &matrix, 1);
            matrix.t[0] = secondary->t[0] + cursor->particle.secondaryX;
            matrix.t[1] = secondary->t[1] + cursor->particle.secondaryY;
            matrix.t[2] = secondary->t[2] + cursor->particle.secondaryZ;
            func_800C42A4(packet, &matrix, 1);
            i++;
            cursor = (RoomMotionCursor *)((RoomMotionParticle *)cursor + 1);
        } while ((unsigned int)i < state->count);
    }
}
