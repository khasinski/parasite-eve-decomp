#include "pe1/room_floor_walker.h"
#include "pe1/gte.h"
#ifndef ROOMLIB_FLOOR_WALKER_FUNC
#error "ROOMLIB_FLOOR_WALKER_FUNC must name the handler"
#endif

/* Walks the creature forward along its heading.  A step that leaves the
 * walkable polygon reflects the heading off the crossed edge (one retry per
 * frame); a creature already outside turns away from the player instead.
 * The vertical hop rises with velocity and falls with gravity until it lands
 * on the floor, where it rests before the next hop. */
void ROOMLIB_FLOOR_WALKER_FUNC(RoomFloorWalkerObject *object) {
    RoomFloorWalkerState *state = &object->state;
    RoomFloorWalkerCore *core = object->core;
    RoomFloorWalkerScratch *scratch = ROOM_FLOOR_WALKER_SCRATCH;
    RoomFloorWalkerTrig *entry;
    s16 retries;
    int i;
    int point;
    int previous;
    int current;

    if (state->started == 0) {
        state->started = 1;
        core->field8C = 0;
        core->field6C = 0;
        core->flags &= ~0x40000;
        state->angle = core->heading;
        state->velocity = state->hopVelocity;
    }
    if (core->phase < 2) {
        return;
    }
    if (core->flags & 0x40000) {
        state->angle = (state->angle + 0x800) & 0xFFF;
        core->flags &= ~0x40000;
    }
    if (state->rest != 0) {
        return;
    }
    retries = 1;
    do {
        entry = &D_800966EC[(state->angle + 0x800) & 0xFFF];
        {
            int cosine = entry->part.cosine;
            int sine;
            u16 c;
            u16 s;

            scratch->yaw.m[0][1] = 0;
            scratch->yaw.m[0][0] = cosine;
            sine = entry->word;
            c = scratch->yaw.m[0][0];
            scratch->yaw.m[1][0] = 0;
            scratch->yaw.m[1][2] = 0;
            scratch->yaw.m[2][1] = 0;
            scratch->step.words.xy = 0;
            scratch->yaw.m[0][2] = sine;
            s = scratch->yaw.m[0][2];
            scratch->yaw.m[1][1] = 0x1000;
            scratch->yaw.m[2][2] = c;
            scratch->yaw.m[2][0] = -s;
        }
        scratch->step.words.z = state->speed >> 12;
        gte_ldrotmatrix(&scratch->yaw);
        gte_ldv0(&scratch->step);
        gte_mvmva();
        gte_stmac(&scratch->rotated);
        scratch->next.x = core->pos[0] + (scratch->rotated.x << 12);
        scratch->next.z = core->pos[2] + (scratch->rotated.z << 12);
        point = ((u32)scratch->next.x >> 16) | (scratch->next.z & 0xFFFF0000);
        previous = D_8009D248[D_8009D1CC - 1].x |
                   (D_8009D248[D_8009D1CC - 1].yz & 0xFFFF0000);
        i = 0;
        while (i < D_8009D1CC) {
            current = D_8009D248[i].x | (D_8009D248[i].yz & 0xFFFF0000);
            gte_ldsxy0(point);
            gte_ldsxy2(previous);
            gte_ldsxy1(current);
            gte_nclip();
            gte_stmac0(&scratch->area);
            if (scratch->area < 0) break;
            i++;
            previous = current;
        }
        if (i >= D_8009D1CC) {
            state->turning = 1;
            break;
        }
        if (state->turning == 0) {
            state->angle = (FieldEng_VecToAngle(&core->pos[0],
                                                D_8009D254->pos) + 0x800) & 0xFFF;
            break;
        }
        {
            int edge = Gte_Atan2((s16)current - (s16)previous,
                                 (current >> 16) - (previous >> 16)) & 0xFFF;
            if (edge >= 0x800) edge -= 0x800;
            retries--;
            state->angle = (edge + (edge - state->angle)) & 0xFFF;
        }
    } while (retries != -1);
    if (retries < 0) {
        state->turning = 0;
    }
    core->pos[0] = scratch->next.x;
    core->pos[2] = scratch->next.z;
    core->pos[1] += state->velocity;
    if ((core->pos[1] >> 16) >= D_800942EC.y) {
        core->pos[1] = D_800942EC.y << 16;
        state->velocity = -state->hopVelocity;
        state->rest = state->restTime;
    } else {
        state->velocity += state->gravity;
    }
}
