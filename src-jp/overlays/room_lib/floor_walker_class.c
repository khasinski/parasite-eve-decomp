/*
 * The floor walker actor class driven by RoomLib_HandlerG: a sixth class
 * that 8 rooms (room_m145, m152, m153, m154, m332, m399, m401, m402) link
 * directly after the room library (actor_classes.c), listed in the
 * same room class table with the same seven-method layout (no-op, Init,
 * Configure, no-op, Update, ..., Release, no-op).
 */

#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/fixed_point.h"
#include "pe1/gte.h"
#include "pe1/room_floor_walker.h"

#include "room_lib.h"

/* The update handler's owner: the step callback it runs first and the
 * squash timer it plays out. */
typedef struct RoomTransformScaleOwner {
    char pad0[8];
    FieldActor *actor;
    void (*callback)(struct RoomTransformScaleOwner *);
    char pad10[0x22];
    s16 timer;
    s16 duration;
} RoomTransformScaleOwner;

int RoomLib_HandlerGNop0(void) {
    return 0;
}
int RoomLib_InitHandlerG(RoomEnt *o) {
    unsigned int m = 0x10002;
    RoomLink *l = o->link;
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    ((RoomFloorWalkerObject *)o)->state.rest = 0;
    ((RoomFloorWalkerObject *)o)->state.turning = 0;
    o->sub.cb = RoomLib_HandlerG;
    RW32(l, 0x98) |= m;
    l->h250 |= 0x400;
    return 0;
}

int RoomLib_ConfigureHandlerG(RoomEnt *o, int mode, unsigned int op,
                              int arg0, int arg1, int arg2) {
    RoomFloorWalkerState *p = &((RoomFloorWalkerObject *)o)->state;

    if (op != 4) {
    if (op < 5) {
        if (op == 0) {
            goto case0;
        }
        return 0;
    }
    if (op == 12) {
        goto case12;
    }
    if (op != 25) {
        return 0;
    }
    if (mode != 1) {
        return 0;
    }
    o->sub.signal = (int *)arg0;
    *(int *)arg0 = mode;

    }
    else
    {
case4:
    ((RoomFloorWalkerObject *)o)->state.speed = arg0;
    return 0;

case0:
    ((RoomFloorWalkerObject *)o)->state.leader = (RoomLink *)g_FieldActorListHead;
    if (g_FieldActorListHead != 0) {
        do {
            FieldActorNode *cur = (FieldActorNode *)p->leader;
            if (cur->b0C == arg0 && cur->b0D == arg1) {
                if ((cur->w98 & 0x10) == 0) {
                    break;
                }
            }
            p->leader = (RoomLink *)((FieldActorNode *)p->leader)->next;
        } while (p->leader != 0);
        if (p->leader != 0) {
            return 0;
        }
        RoomLib_ReleaseHandlerG(o);
    } else {
        RoomLib_ReleaseHandlerG(o);
    }
    return 0;

case12:
    ((RoomFloorWalkerObject *)o)->state.hopVelocity = arg0;
    ((RoomFloorWalkerObject *)o)->state.gravity = arg1;
    ((RoomFloorWalkerObject *)o)->state.restTime = arg2;

    }
ret:
    return 0;
}

int RoomLib_HandlerGNop3(void) {
    return 0;
}

int RoomLib_UpdateHandlerG(RoomTransformScaleOwner *owner) {
    FieldActor *actor;
    RenderMatrix *matrix;
    s32 *scale;
    s32 scaleValue;
    s32 curve;

    actor = owner->actor;
    scale = (s32 *)0x1F800008;
    owner->callback(owner);

    matrix = &actor->render_object.model_matrix;
    matrix->translation[0] = Pe1Fixed_Integer(&actor->pos_x);
    matrix->translation[1] = Pe1Fixed_Integer(&actor->pos_y);
    matrix->translation[2] = Pe1Fixed_Integer(&actor->pos_z);
    RotMatrixYXZ((void *)&actor->rot_x, (void *)matrix);

    scaleValue = actor->move_speed;
    Scratch_1F800010 = scaleValue;
    Scratch_1F80000C = scaleValue;
    Scratch_1F800008 = scaleValue;
    ScaleMatrix((void *)matrix, (void *)0x1F800008);

    if (owner->timer != 0) {
        curve = (*(s32 *)rcossin_tbl[(((owner->duration - owner->timer) << 11) /
                                     owner->duration) & 0xFFF].half << 16) >> 17;
        Scratch_1F800010 = 0x1000 + curve;
        scale[0] = 0x1000 + curve;
        Scratch_1F80000C = 0x1000 - curve;
        ScaleMatrix((void *)matrix, (void *)scale);
        owner->timer--;
    }

    return 0;
}

void RoomLib_HandlerG(RoomEnt *o) {
    RoomEnt *self = o;
    RoomLink *src = ((RoomFloorWalkerObject *)self)->state.leader;
    RoomLink *dst = self->link;
    dst->pos[0] = ((GteMatrix *)src->p238)[4].t[0] << 16;
    dst->pos[1] = ((GteMatrix *)src->p238)[4].t[1] << 16;
    dst->pos[2] = ((GteMatrix *)src->p238)[4].t[2] << 16;
    *(RoomLibPacked8 *)dst->rotation = *(RoomLibPacked8 *)src->rotation;
    if ((src->variant == 7) && (src->winLo >= 3) && (src->winHi < 3)) {
        self->sub.cb = RoomLib_HandlerGFloorWalker;
    }
    if (src->target != 0) {
        if (Battle_GetEnemyContextField((struct FieldActor *)src, 0x2C) <= 0) {
            Entity_WriteFieldByCmd(dst, 0x2D, 0);
            Entity_WriteFieldByCmd(dst, 0x2C, 0);
            Entity_WriteFieldByCmd(dst, 0x5F, 0);
            RoomLib_ReleaseHandlerG(self);
        }
    } else {
        RoomLib_ReleaseHandlerG(self);
    }
}

/* Walks the creature forward along its heading.  A step that leaves the
 * walkable polygon reflects the heading off the crossed edge (one retry per
 * frame); a creature already outside turns away from the player instead.
 * The vertical hop rises with velocity and falls with gravity until it lands
 * on the floor, where it rests before the next hop. */
void RoomLib_HandlerGFloorWalker(RoomEnt *obj) {
    RoomFloorWalkerObject *object = (RoomFloorWalkerObject *)obj;
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
        entry = (RoomFloorWalkerTrig *)rcossin_tbl[(state->angle + 0x800) & 0xFFF].half;
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
        point = ((unsigned)scratch->next.x >> 16) | (scratch->next.z & 0xFFFF0000);
        previous = FLOOR_POLYGON_AS(RoomFloorWalkerVertex)[(s16)g_FloorPolygonVertexCount - 1].x |
                   (FLOOR_POLYGON_AS(RoomFloorWalkerVertex)[(s16)g_FloorPolygonVertexCount - 1].yz & 0xFFFF0000);
        i = 0;
        while (i < (s16)g_FloorPolygonVertexCount) {
            current = FLOOR_POLYGON_AS(RoomFloorWalkerVertex)[i].x | (FLOOR_POLYGON_AS(RoomFloorWalkerVertex)[i].yz & 0xFFFF0000);
            gte_ldsxy0(point);
            gte_ldsxy2(previous);
            gte_ldsxy1(current);
            gte_nclip();
            gte_stmac0(&scratch->area);
            if (scratch->area < 0) break;
            i++;
            previous = current;
        }
        if (i >= (s16)g_FloorPolygonVertexCount) {
            state->turning = 1;
            break;
        }
        if (state->turning == 0) {
            state->angle = (FieldFx_VecToAngle((Vec3Fixed *)&core->pos[0],
                                                (Vec3Fixed *)((RoomFloorWalkerTarget *)D_8009CD10)->pos) + 0x800) & 0xFFF;
            break;
        }
        {
            int edge = ratan2((s16)current - (s16)previous,
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
    if ((core->pos[1] >> 16) >= g_RoomFloorY->y) {
        core->pos[1] = g_RoomFloorY->y << 16;
        state->velocity = -state->hopVelocity;
        state->rest = state->restTime;
    } else {
        state->velocity += state->gravity;
    }
}

int RoomLib_ReleaseHandlerG(RoomEnt *o) {
    int *p = o->sub.signal;
    o->state = 4;
    if (p != 0) {
        *p = 0;
    }
    return 0;
}
int RoomLib_HandlerGNop6(void) {
    return 0;
}
