/* MASPSX_FLAGS: --expand-div */
/*
 * The room library: five actor classes, driven by RoomLib_HandlerD,
 * RoomLib_HandlerE, RoomLib_HandlerB, RoomLib_HandlerC and RoomLib_HandlerA.
 *
 * 117 room and scene overlays (and 8 more with a longer variant) link the
 * same 53 functions in the same order with the same four jump tables; this
 * unit is that library, compiled into each of them. Each class occupies the
 * seven-method run its room class table lists (no-op, Init, Configure,
 * no-op, Update, ..., Release, no-op), with the class's private states
 * between Update and Release. The zero word between the HandlerE and
 * HandlerB jump tables is the 8-byte alignment of the third table inside
 * this one object.
 */
#include "room_lib.h"
#include "pe1/gte_types.h"

int RoomLib_HandlerDNop0(void) {
    return 0;
}
int RoomLib_InitHandlerD(RoomEnt *o) {
    o->flag3 = 1;
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    RW32(o, 0x9C) = 0x400;
    RW16(o, 0xB0) = 0x100;
    o->sub.cb = RoomLib_HandlerD;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    RW32(o, 0x94) = 0;
    RW32(o, 0x98) = 0;
    RW32(o, 0x8C) = 0;
    RW32(o, 0x3C) = 0;
    RW32(o, 0x40) = 0;
    RW32(o, 0x44) = 0;
    RW32(o, 0xA4) = 0;
    RW16(o, 0xA8) = 0;
    RW16(o, 0xAA) = 0;
    RW16(o, 0xAC) = 0;
    RW16(o, 0x7C) = 0;
    RW16(o, 0x7E) = 0;
    RW16(o, 0x80) = 0;
    RW32(o, 0x90) = 0;
    RW16(o, 0xAE) = 0;
    RW32(o, 0x58) = 0;
    RW16(o, 0xB2) = 0;
    RW8(o, 0xB6) = 0;
    RW8(o, 0xB7) = 0;
    RW8(o, 0xB8) = 0;
    return 0;
}
int RoomLib_ConfigureHandlerD(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) {
    RoomLibHandlerDState *state = (RoomLibHandlerDState *)&o->sub;
    FieldActorNode *node;
    switch (op) {
    case 19:
        if (query == 0) {
            o->flag3 = arg0;
        } else {
            *(int *)arg0 = o->flag3;
            *(int *)arg1 = state->stateValue;
        }
        break;
    case 25:
        if (query == 1) {
            state->signal = (int *)arg0;
            *(int *)arg0 = query;
        }
        break;
    case 0:
        state->targetLink = (RoomLink *)D_8009D20C;
        while (state->targetLink != 0) {
            node = (FieldActorNode *)state->targetLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->targetLink = (RoomLink *)
                ((FieldActorNode *)state->targetLink)->next;
        }
        break;
    case 17:
        state->target[0] = arg0;
        state->target[2] = arg2;
        if (arg1 == -1) {
            state->target[1] = RW32(D_8009D254, 0x2C);
        } else {
            state->target[1] = arg1;
        }
        state->targetLink = 0;
        break;
    case 26:
        state->target[0] = arg0;
        state->target[1] = o->link->pos[1];
        state->target[2] = arg1;
        state->target[3] = arg2;
        state->targetLink = 0;
        break;
    case 4:
        state->value88 = arg0;
        break;
    case 2:
        state->value8C = arg0;
        break;
    case 3:
        state->value90 = arg0;
        state->rate = arg1;
        if (arg2 > 0) {
            state->duration = arg2;
        }
        break;
    case 23:
        arg0 = arg0 != 0;
        arg1 = (arg1 != 0) << 1;
        arg2 = (arg2 != 0) << 2;
        state->flags = arg0 | arg1 | arg2;
        break;
    case 10:
        state->variant = arg0;
        state->active = arg1;
        state->mirrorPosition = arg2;
        state->callback = (void (*)(void))RoomLib_ArmHandlerD;
        break;
    case 11:
        state->optionB = arg0;
        state->optionC = arg1;
        break;
    case 6:
        state->rotation[0] = arg0 >> 16;
        state->rotation[1] = arg1 >> 16;
        state->rotation[2] = arg2 >> 16;
        break;
    case 20:
        state->range[0] = arg0;
        state->range[1] = arg1;
        state->range[2] = arg2;
        break;
    case 21:
        state->secondaryLink = (RoomLink *)D_8009D20C;
        while (state->secondaryLink != 0) {
            node = (FieldActorNode *)state->secondaryLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->secondaryLink = (RoomLink *)
                ((FieldActorNode *)state->secondaryLink)->next;
        }
        state->heading = arg2 & 0xFFF;
        state->reverse = (unsigned int)arg2 >> 31;
        break;
    case 22:
        state->positionX = arg0;
        state->positionZ = arg1;
        state->heading = arg2 & 0xFFF;
        state->reverse = (unsigned int)arg2 >> 31;
        break;
    case 30:
        state->lockY = arg0;
        break;
    case 31:
        state->copyPosition = arg0;
        state->copyRotation = arg1;
        break;
    }
    return 0;
}
int RoomLib_HandlerDNop3(void) {
    return 0;
}
int RoomLib_UpdateHandlerD(RoomEnt *o) {
    switch (func_800DFB78()) {
    case 0:
        if (o->link->variant < 2) {
            return 0;
        }
        ((void (*)(RoomEnt *))o->sub.cb)(o);
        return 0;
    case 1:
        RoomLib_ReleaseHandlerD(o);
    case 2:
        return 0;
    }
    return 0;
}
void RoomLib_ArmHandlerD(RoomEnt *o) {
    signed char c;
    int t;
    unsigned short lo;
    if (o->bB4 != 0) {
        RoomLib_FxNotify(o->link, &o->sub, 0x1F800000);
    }
    c = o->t16;
    if (c >= 0) {
        if (c != o->link->variant) {
            return;
        }
    }
    t = o->t17;
    if (t >= 0) {
        RoomLink *l = o->link;
        lo = l->winLo;
        if (t >= l->winHi && t <= lo) {
            o->sub.cb = RoomLib_HandlerD;
        }
    } else {
        o->sub.cb = RoomLib_HandlerD;
    }
}

void RoomLib_HandlerD(RoomEnt *o) {
    RoomEnt *ent = o;
    char *state = (char *)&o->sub;
    short *scratch;
    RoomLink *link = o->link;
    char *left;
    register char *right asm("$16");
    int value;
    int product;
    int *rot;
    int rot_y;
    int rot_word;
    int rot_x;

    asm("" : : "r"(state) : "$18");
    scratch = (short *)0x1F800000;

    if (ent->t1A == 0) {
        ent->t1A = 1;
        ent->w04 = 0;
        RW32(link, 0x98) &= 0xFFF3FFFF;
        if (ent->active != 0 && link->target != 0) {
            link->target->flags |= 0x40000000;
        }
        RW32(state, 0x50) = RW32(link, 0x28);
        RW32(state, 0x54) = RW32(link, 0x2C);
        RW32(state, 0x58) = RW32(link, 0x30);
        RoomLib_HandlerDTransformTarget((struct RoomLibMotionState *)state,
                                        (void *)scratch);
        {
            int initial_speed;
            register char *left_arg asm("$4");
            int initial_velocity;

            left = state + 0x50;
            left_arg = left;
            right = state + 0x40;
            initial_speed = func_800DFC80((int *)left_arg, (int *)right);
            initial_speed += (RW32(state, 0x8C) * 0x70) >> 21;
            if (initial_speed < 0x148) {
                initial_speed = 0x148;
            }
            initial_velocity = (0x5160000 / initial_speed) << 8;
            RW32(state, 0x94) = initial_velocity;
            RW32(state, 0x98) = initial_velocity >> 3;
            func_800DFE94(left, right, state + 0x78);
            right = state + 0x78;
            RotMatrixYXZ(right, state + 0x10);
        }
    }

    {
        int bias = 0x8000;
        int *table = (int *)D_800966EC;

    scratch[0x34] = 0;
    {
        int angle;
        angle = ((RW32(state, 0x98) + bias) >> 16) - 0x400;
        scratch[0x35] = angle;
        rot = table + (angle & 0xFFF);
    }
    rot_y = ((short *)rot)[1];
    scratch[0x15] = 0;
    scratch[0x14] = rot_y;
    PE1_COMPILER_MEMORY_BARRIER();
    rot_word = *rot;
    PE1_COMPILER_USE(rot_word);
    rot_x = (unsigned short)scratch[0x14];
    scratch[0x17] = 0;
    scratch[0x19] = 0;
    scratch[0x1B] = 0;
    scratch[0x00] = 0;
    scratch[0x01] = 0;
    PE1_COMPILER_MEMORY_BARRIER();
    scratch[0x16] = rot_word;
    {
        register int negated asm("$3");
        negated = *(volatile unsigned short *)&scratch[0x16];
        scratch[0x18] = 0x1000;
        scratch[0x1C] = rot_x;
        asm("" : "=r"(negated) : "0"(negated) : "memory");
        negated = -negated;
        scratch[0x1A] = negated;
    }
    scratch[0x02] = RW32(state, 0x88) >> 12;
    gte_ldrotmatrix((void *)((int)scratch | 0x28));
    gte_ldv0((void *)scratch);
    gte_mvmva();
    asm("" : "=r"(scratch) : "0"(scratch));
    gte_stmac((char *)scratch + 8);
    RW32(state, 0x30) += *(int *)((char *)scratch + 8) << 12;
    RW32(state, 0x38) += *(int *)((char *)scratch + 0x10) << 12;

    if (RW32(state, 0x98) > 0x08000000) {
        ent->flag3 = 2;
    }
    {
        register int work asm("$3");

        if (*(int *)((char *)scratch + 0x10) >= 0) {
            int position;
            position = RW32(state, 0x88);
            position += RW32(state, 0x8C);
            work = (position + bias) >> 16;
            product = work * (RW32(state, 0x94) >> 12);
            PE1_COMPILER_MEMORY_BARRIER();
            RW32(state, 0x88) = position;
            PE1_COMPILER_MEMORY_BARRIER();
            work = RW32(state, 0x98);
        } else {
            int updated;
            work = RW32(state, 0x88);
            work = (work + bias) >> 16;
            product = work * (RW32(state, 0x94) >> 12);
            PE1_COMPILER_MEMORY_BARRIER();
            updated = RW32(state, 0x88);
            updated -= RW32(state, 0x8C);
            PE1_COMPILER_USE(updated);
            work = RW32(state, 0x98);
            RW32(state, 0x88) = updated;
        }
        RW32(state, 0x98) = work + ((product >> 8) << 12);
    }
    }

    {
        int bias = 0x8000;
        if (RW16(state, 0x9A) >= 0xC01 && RW32(state, 0x30) <= 0) {
            *(int *)((char *)scratch + 0x18) = 0;
            *(int *)((char *)scratch + 0x1C) = 0;
            *(int *)((char *)scratch + 0x20) = 0;
            RoomLib_ReleaseHandlerD(ent);
        } else {
            scratch[0] = ((((RW32(state, 0x30) + bias) >> 16) *
                           RW32(state, 0x90)) >> 12);
            scratch[1] = 0;
            scratch[2] = (RW32(state, 0x38) + bias) >> 16;
            gte_ldrotmatrix(state + 0x10);
            gte_ldv0((void *)scratch);
            gte_mvmva();
            gte_stmac((char *)scratch + 0x18);
        }
    }

    if (RW16(state, 0xA6) != 0 && RW16(state, 0xA6) < RW16(state, 0x9A)) {
        RoomLib_ReleaseHandlerD(ent);
    }
    RW32(link, 0x28) = RW32(state, 0x50) +
                       (*(int *)((char *)scratch + 0x18) << 16);
    RW32(link, 0x30) = RW32(state, 0x58) +
                       (*(int *)((char *)scratch + 0x20) << 16);
    {
        RW32(link, 0x1FC) = (RW32(link, 0x28) + 0x8000) >> 16;
        RW32(link, 0x204) = (RW32(link, 0x30) + 0x8000) >> 16;
        if (RW8(state, 0xAB) == 0) {
            value = RW32(state, 0x54) +
                    (*(int *)((char *)scratch + 0x1C) << 16);
            RW32(link, 0x2C) = value;
            RW32(link, 0x200) = (value + 0x8000) >> 16;
        }
    }
    if (RW16(state, 0x9C) != 0 &&
        *(int *)((char *)scratch + 0x10) >= 0) {
        if (ent->w04 >= (unsigned int)RW16(state, 0x9E) &&
            (RW16(state, 0xA0) == 0 ||
             ent->w04 <= (unsigned int)RW16(state, 0xA0))) {
            RoomLib_HandlerDTransformTarget((struct RoomLibMotionState *)state,
                                        (void *)scratch);
            scratch[1] = RW16(state, 0x7A);
            func_800DFE94(state + 0x50, state + 0x40, state + 0x78);
            right = state + 0x78;
            RW16(state, 0x7A) = FieldEng_TurnToward(
                (short)*(volatile unsigned short *)&scratch[1],
                RW16(state, 0x7A), RW16(state, 0x9C)) & 0xFFF;
            RotMatrixYXZ(right, state + 0x10);
        }
    }
    if (RW8(state, 0xAA) == 0) {
        RoomLib_FxNotify(link, (struct RoomSub *)state, (int)scratch);
    }
    func_800DFB20(ent);
}

void RoomLib_FxNotify(RoomLink *arg0, struct RoomSub *arg1, int scratch) {
    char *ent = (char *)arg0;
    char *rec = (char *)arg1;
    if (*(short *)(rec + 0xA2) > 0) {
        int *dst = (int *)(rec + 0x60);
        char *v = *(char **)(rec + 0x84);
        if (v != 0) {
            dst[0] = *(int *)(v + 0x28);
            dst[2] = *(int *)(v + 0x30);
        }
        *(short *)(ent + 0x3A) = FieldEng_TurnToward(
            *(short *)(ent + 0x3A),
            (short)FieldEng_VecToAngle(dst, (int *)(ent + 0x28)),
            *(short *)(rec + 0xA2)) & 0xFFF;
        if ((*(unsigned char *)(rec + 0xA9) != 0)
            && (0x07FFFFFF < *(int *)(rec + 0x98))) {
            *(short *)(rec + 0xA2) = 0;
            *(short *)(rec + 0xA4) = 0;
        }
    } else {
        func_800DFE94(ent + 0x28, ent + 0x40, ent + 0x38);
        *(short *)(ent + 0x3A) = FieldEng_TurnToward(
            *(short *)(ent + 0x3A),
            *(short *)(ent + 0x3A),
            *(short *)(rec + 0xA4)) & 0xFFF;
    }
}
void RoomLib_HandlerDTransformTarget(RoomLibMotionState *state, RoomLibMotionWork *work) {
    int angle;
    int *entry;
    int *table;
    int height;
    int lo;
    int hi;
    if (state->target != 0) {
        state->position[0] = state->target->pos[0];
        state->position[1] = state->target->pos[1];
        state->position[2] = state->target->pos[2];
    }
    if (state->mode.word != 0) {
        angle = FieldEng_VecToAngle(state->anchor, state->position);
        angle &= 0xFFF;
        table = (int *)D_800966EC;
        entry = table + angle;
        hi = ((short *)entry)[1];
        work->matrix[1] = 0;
        work->matrix[0] = hi;
        lo = *entry;
        work->matrix[3] = 0;
        work->matrix[5] = 0;
        work->matrix[7] = 0;
        work->input[0] = 0;
        work->input[1] = 0;
        work->matrix[2] = lo;
        work->matrix[4] = 0x1000;
        work->matrix[8] = work->matrix[0];
        work->matrix[6] = -(unsigned short)work->matrix[2];
        height = state->mode.half.height;
        work->input[2] = height;
        gte_ldrotmatrix(work->matrix);
        gte_ldv0(work->input);
        gte_mvmva();
        gte_stmac(work->rotated);
        state->position[0] = state->anchor[0] + (work->rotated[0] << 16);
        state->position[2] = state->anchor[2] + (work->rotated[2] << 16);
    }
    angle = FieldEng_VecToAngle(state->anchor, state->position);
    angle &= 0xFFF;
    table = (int *)D_800966EC;
    entry = table + angle;
    hi = ((short *)entry)[1];
    work->matrix[1] = 0;
    work->matrix[0] = hi;
    lo = *entry;
    work->matrix[3] = 0;
    work->matrix[5] = 0;
    work->matrix[7] = 0;
    work->matrix[2] = lo;
    work->matrix[4] = 0x1000;
    work->matrix[8] = work->matrix[0];
    work->matrix[6] = -(unsigned short)work->matrix[2];
    gte_ldrotmatrix(work->matrix);
    gte_ldv0(state->localStep);
    gte_mvmva();
    gte_stmac(work->rotated);
    state->position[0] += work->rotated[0] << 16;
    state->position[1] += work->rotated[1] << 16;
    state->position[2] += work->rotated[2] << 16;
}
int RoomLib_ReleaseHandlerD(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    o->state = 4;
    o->flag3 = 0;
    if (o->bB8 == 0) {
        if (o->active != 0) {
            if (o->link->target != 0) {
                *o->link->target->state = 4;
            }
        }
    } else {
        if (o->link->target != 0) {
            unsigned char *state = o->link->target->state;
            if (*state == 1) {
                *state = 4;
            }
        }
    }
    if (s->signal != 0) {
        *s->signal = 0;
    }
    return 0;
}
int RoomLib_HandlerDNop6(void) {
    return 0;
}

int RoomLib_HandlerENop0(void) {
    return 0;
}
int RoomLib_InitHandlerE(RoomEnt *o) {
    o->flag3 = 1;
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    RW32(o, 0x74) = 0x10000;
    o->sub.cb = RoomLib_HandlerE;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    RW32(o, 0x4C) = 0;
    RW32(o, 0x50) = 0;
    RW32(o, 0x54) = 0;
    RW32(o, 0x6C) = 0;
    RW32(o, 0x70) = 0;
    RW16(o, 0x7A) = 0;
    RW16(o, 0x80) = 0;
    RW16(o, 0x82) = 0;
    RW16(o, 0x84) = 0;
    RW16(o, 0x86) = 0;
    RW16(o, 0x88) = 0;
    RW8(o, 0x90) = 0;
    RW8(o, 0x91) = 0;
    return 0;
}
int RoomLib_ConfigureHandlerE(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) {
    RoomLibHandlerEState *state = (RoomLibHandlerEState *)&o->sub;
    FieldActorNode *node;
    switch (op) {
    case 19:
        if (query == 0) {
            o->flag3 = arg0;
        } else {
            *(int *)arg0 = o->flag3;
        }
        break;
    case 25:
        if (query == 1) {
            state->signal = (int *)arg0;
            *(int *)arg0 = query;
        }
        break;
    case 0:
        state->targetLink = (RoomLink *)D_8009D20C;
        while (state->targetLink != 0) {
            node = (FieldActorNode *)state->targetLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->targetLink = (RoomLink *)
                ((FieldActorNode *)state->targetLink)->next;
        }
        break;
    case 17:
        state->target[0] = arg0;
        state->target[2] = arg2;
        if (arg1 == -1) {
            state->target[1] = RW32(D_8009D254, 0x2C);
        } else {
            state->target[1] = arg1;
        }
        state->targetLink = 0;
        break;
    case 4:
        state->speed = arg0;
        state->eased = arg1;
        state->duration = 0;
        break;
    case 6:
        state->localOffset[0] = arg0;
        state->localOffset[1] = arg1;
        state->localOffset[2] = arg2;
        break;
    case 15:
        state->duration = arg0;
        state->eased = arg1;
        break;
    case 21:
        state->secondaryLink = (RoomLink *)D_8009D20C;
        while (state->secondaryLink != 0) {
            node = (FieldActorNode *)state->secondaryLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->secondaryLink = (RoomLink *)
                ((FieldActorNode *)state->secondaryLink)->next;
        }
        state->secondaryHeading = arg2;
        break;
    case 22:
        state->secondary[0] = arg0;
        state->secondary[2] = arg1;
        state->secondaryHeading = arg2;
        break;
    case 23:
        arg0 = arg0 != 0;
        arg1 = (arg1 != 0) << 1;
        arg2 = (arg2 != 0) << 2;
        state->flags = arg0 | arg1 | arg2;
        break;
    case 10:
        state->variant = arg0;
        state->active = arg1;
        state->mirrorPosition = arg2;
        state->callback = (void (*)(void))RoomLib_ArmHandlerE;
        break;
    case 11:
        state->optionB = arg0;
        state->optionC = arg1;
        break;
    case 24:
        if (query == 1) {
            *(int *)arg0 = state->heading;
        }
        break;
    case 27:
        state->phaseFrame[1] = arg0;
        state->phaseFrame[2] = arg1;
        state->phaseFrame[3] = arg2;
        if (arg0 != -1) {
            if (arg1 != -1) {
                if (arg2 == -1) {
                    state->frameLimit = 2;
                } else {
                    state->frameLimit = 3;
                }
            } else {
                state->frameLimit = 1;
            }
        } else {
            state->frameLimit = 0;
        }
        break;
    case 31:
        state->lockY = arg0;
        break;
    case 32:
        state->copyPosition = arg0;
        break;
    }
    return 0;
}
int RoomLib_HandlerENop3(void) {
    return 0;
}
int RoomLib_UpdateHandlerE(RoomEnt *o) {
    switch (func_800DFB78()) {
    case 0:
        if (o->link->variant < 2) {
            return 0;
        }
        ((void (*)(RoomEnt *))o->sub.cb)(o);
        return 0;
    case 1:
        RoomLib_ReleaseHandlerE(o);
    case 2:
        return 0;
    }
    return 0;
}
void RoomLib_ArmHandlerE(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    signed char c;
    int t;
    unsigned short lo;
    if (RW16(o, 0x80) != 0) {
        RoomLib_HandlerESteerToward((void *)o->link, (void *)s);
    }
    c = o->t16;
    if (c >= 0) {
        if (c != o->link->variant) {
            return;
        }
    }
    t = o->t17;
    if (t >= 0) {
        RoomLink *l = o->link;
        lo = l->winLo;
        if (t >= l->winHi && t <= lo) {
            o->sub.cb = RoomLib_HandlerE;
        }
    } else {
        o->sub.cb = RoomLib_HandlerE;
    }
}
void RoomLib_HandlerE(RoomEnt *o) {
    RoomLink *link = o->link;
    RoomLink *targetLink;
    RoomLibHandlerEState *state = (RoomLibHandlerEState *)&o->sub;
    volatile short *scratch = (volatile short *)0x1F800000;
    int remain;
    int scale;
    int angle;
    int rotWord;
    int localX;
    int localZ;
    int rotY;
    short rotX;
    short negRotY;
    short *rotBase;
    short *rot;
    if (o->t1A == 0) {
        RW16(o, 0x86) = 1;
        o->t1A = 1;
        RW32(link, 0x98) &= 0xFFF3FFFF;
        if (o->active != 0 && link->target != 0) {
            link->target->flags |= 0x40000000;
        }
        state->start[0] = link->pos[0];
        state->start[1] = link->pos[1];
        state->start[2] = link->pos[2];
        targetLink = state->targetLink;
        if (targetLink != 0) {
            state->target[0] = targetLink->pos[0];
            state->target[1] = targetLink->pos[1];
            state->target[2] = targetLink->pos[2];
        }
        angle = FieldEng_VecToAngle(state->start, state->target) & 0xFFF;
        {
            rotBase = (short *)(D_800966EC);
            rot = rotBase + (angle << 1);
            rotY = rot[1];
            scratch[21] = 0;
            scratch[20] = rotY;
            rotWord = *(int *)rot;
            rotX = scratch[20];
        }
        scratch[23] = 0;
        scratch[25] = 0;
        scratch[27] = 0;
        scratch[22] = rotWord;
        negRotY = scratch[22];
        scratch[24] = 0x1000;
        scratch[28] = rotX;
        scratch[26] = -negRotY;
        localX = state->localOffset[0];
        scratch[1] = 0;
        scratch[0] = localX >> 12;
        localZ = state->localOffset[2];
        do {
            scratch[2] = localZ >> 12;
            gte_ldrotmatrix((void *)((int)scratch | 0x28));
        } while (0);
        gte_ldv0((void *)scratch);
        gte_mvmva();
        gte_stmac((void *)&scratch[4]);
        state->target[0] += *(int *)&scratch[4] << 12;
        state->target[1] += *(int *)&scratch[6] << 12;
        state->target[2] += *(int *)&scratch[8] << 12;
        if (state->duration == 0) {
            state->duration = ((func_800DFC80(state->start, state->target) << 16) / state->speed) + 1;
        }
        state->delta[0] = (state->target[0] - state->start[0] + 0x800) >> 12;
        state->delta[1] = (state->target[1] - state->start[1] + 0x800) >> 12;
        state->delta[2] = (state->target[2] - state->start[2] + 0x800) >> 12;
        state->heading = ratan2(state->delta[0], state->delta[2]);
    }
    scratch[52] = 0;
    remain = state->phase - state->phaseFrame[state->frame];
    if (remain < 0) {
        remain = 0;
    }
    if (state->duration < remain) {
        remain = state->duration;
    }
    if (state->eased != 0) {
        scale = 0x1000 - D_800966EE[(((remain << 11) / state->duration) & 0xFFF) << 1];
        link->pos[0] = state->start[0] + (((state->delta[0] * scale + 0x1000) >> 13) << 12);
        if (state->lockY == 0) {
            link->pos[1] = state->start[1] + (((state->delta[1] * scale + 0x1000) >> 13) << 12);
        }
        link->pos[2] = state->start[2] + (((state->delta[2] * scale + 0x1000) >> 13) << 12);
    } else {
        link->pos[0] = state->start[0] + ((state->delta[0] * remain / state->duration) << 12);
        if (state->lockY == 0) {
            link->pos[1] = state->start[1] + ((state->delta[1] * remain / state->duration) << 12);
        }
        link->pos[2] = state->start[2] + ((state->delta[2] * remain / state->duration) << 12);
    }
    if (state->copyPosition != 0) {
        link->posMirror[0] = link->pos[0];
        link->posMirror[1] = link->pos[1];
        link->posMirror[2] = link->pos[2];
        link->posMirror[3] = link->pos[3];
    }
    if (remain >= state->duration && state->frame >= state->frameLimit) {
        RoomLib_ReleaseHandlerE(o);
    } else {
        state->frame++;
        if (state->frameLimit < state->frame) {
            state->frame = 0;
            state->phase++;
        }
    }
    RoomLib_HandlerESteerToward(link, state);
    func_800DFB20(o);
}
void RoomLib_HandlerESteerToward(char *ent, char *rec) {
    if (*(short *)(rec + 0x6E) > 0) {
        int *dst = (int *)(rec + 0x50);
        char *v = *(char **)(rec + 0x64);
        if (v != 0) {
            dst[0] = *(int *)(v + 0x28);
            dst[2] = *(int *)(v + 0x30);
        }
        *(short *)(ent + 0x3A) = FieldEng_TurnToward(
            *(short *)(ent + 0x3A),
            (short)FieldEng_VecToAngle(dst, (int *)(ent + 0x28)),
            *(short *)(rec + 0x6E));
    }
}
int RoomLib_ReleaseHandlerE(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    o->state = 4;
    o->flag3 = 0;
    if (o->active != 0) {
        RoomLink *l = o->link;
        if (l->target != 0) {
            *l->target->state = 4;
        }
    }
    if (s->signal != 0) {
        *s->signal = 0;
    }
    return 0;
}
int RoomLib_HandlerENop6(void) {
    return 0;
}

int RoomLib_HandlerBNop0(void) {
    return 0;
}
int RoomLib_InitHandlerB(RoomEnt *o) {
    RoomLink *l;
    o->flag3 = 1;
    o->t17 = -1;
    o->t16 = -1;
    o->t19 = 7;
    RW32(o, 0x8C) = 0x10000;
    l = o->link;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    RW32(o, 0x74) = 0;
    RW32(o, 0x78) = 0;
    RW32(o, 0x7C) = 0;
    RW32(o, 0x80) = 0;
    RW16(o, 0x92) = 0;
    RW32(o, 0x4C) = 0;
    RW32(o, 0x50) = 0;
    RW32(o, 0x54) = 0;
    RW16(o, 0x94) = 0;
    RW32(o, 0x6C) = 0;
    o->sub.cb = RoomLib_HandlerB;
    RW32(o, 0x70) = 0;
    RW16(o, 0x96) = 0;
    l->vel[0] = 0;
    l->vel[1] = 0;
    l->vel[2] = 0;
    l->accel[0] = 0;
    l->accel[1] = 0;
    l->accel[2] = 0;
    l->move[0] = 0;
    l->move[1] = 0;
    l->move[2] = 0;
    return 0;
}
int RoomLib_ConfigureHandlerB(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) {
    RoomLibHandlerBState *state = (RoomLibHandlerBState *)&o->sub;
    FieldActorNode *node;
    switch (op) {
    case 19:
        if (query == 0) {
            o->flag3 = arg0;
        } else {
            *(int *)arg0 = o->flag3;
        }
        break;
    case 25:
        if (query == 1) {
            state->signal = (int *)arg0;
            *(int *)arg0 = query;
        }
        break;
    case 4:
        state->speed = arg0;
        state->acceleration = arg1;
        break;
    case 2:
        state->duration = arg0;
        break;
    case 12:
        state->phaseValue = arg0;
        break;
    case 13:
        state->mode = arg0;
        break;
    case 14:
        state->rate = arg1;
        break;
    case 6:
        state->localOffset[0] = arg0;
        state->localOffset[1] = arg1;
        state->localOffset[2] = arg2;
        break;
    case 23:
        arg0 = arg0 != 0;
        arg1 = (arg1 != 0) << 1;
        arg2 = (arg2 != 0) << 2;
        state->flags = arg0 | arg1 | arg2;
        break;
    case 10:
        state->variant = arg0;
        state->active = arg1;
        state->callback = (void (*)(void))RoomLib_ArmHandlerB;
        break;
    case 11:
        state->optionB = arg0;
        state->optionC = arg1;
        break;
    case 16:
        state->heading = arg0;
        break;
    case 18:
        if (o->flag3 == 2) {
            state->callback = (void (*)(void))RoomLib_AdvanceArcToTarget;
            state->phaseValue = arg0;
            o->flag3 = 3;
        }
        break;
    case 0:
        state->targetLink = (RoomLink *)D_8009D20C;
        while (state->targetLink != 0) {
            node = (FieldActorNode *)state->targetLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->targetLink = (RoomLink *)
                ((FieldActorNode *)state->targetLink)->next;
        }
        break;
    case 17:
        state->target[0] = arg0;
        state->target[2] = arg2;
        if (arg1 != -1) {
            state->target[1] = arg1;
        } else {
            state->target[1] = RW32(D_8009D254, 0x2C);
        }
        state->targetLink = 0;
        break;
    case 21:
        state->secondaryLink = (RoomLink *)D_8009D20C;
        while (state->secondaryLink != 0) {
            node = (FieldActorNode *)state->secondaryLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->secondaryLink = (RoomLink *)
                ((FieldActorNode *)state->secondaryLink)->next;
        }
        state->secondaryHeading = arg2;
        break;
    case 22:
        state->secondaryX = arg0;
        state->secondaryZ = arg1;
        state->secondaryHeading = arg2;
        break;
    }
    return 0;
}
int RoomLib_HandlerBNop3(void) {
    return 0;
}
int RoomLib_UpdateHandlerB(RoomObj *obj) {
    obj->callback();
    return 0;
}
void RoomLib_ArmHandlerB(RoomEnt *o) {
    signed char c = o->t16;
    int t;
    unsigned short lo;
    if (c >= 0) {
        if (c != o->link->variant) {
            return;
        }
    }
    t = o->t17;
    if (t >= 0) {
        RoomLink *l = o->link;
        lo = l->winLo;
        if (t > l->winHi && t <= lo) {
            o->sub.cb = RoomLib_HandlerB;
        }
    } else {
        o->sub.cb = RoomLib_HandlerB;
    }
}

#define ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR(base) \
    do { \
        asm("" : "=r"(base) : "0"(base)); \
        gte_ldv0((char *)(base) + 0x20); \
    } while (0)

void RoomLib_HandlerB(RoomEnt *obj) {
    RoomEnt *self = obj;
    char *state = (char *)&self->sub;
    RoomLink *link = self->link;
    short *scratch;
    int angle;

    asm("" : : "r"(state) : "$19");
    scratch = (short *)0x1F800000;

    if (self->t1A == 0) {
        register int v0 asm("$2");
        int v1;
        register int a0 asm("$4");

        self->t1A = 1;
        RW8(self, 0x98) = 0;
        RW32(link, 0x98) = (RW32(link, 0x98) & 0xFFF3FFFF) | 2;
        if (self->active != 0 && link->target != 0) {
            link->target->flags |= 0x40000000;
        }
        v1 = RWU16(state, 0x86);
        v0 = RWU16(link, 0x3A);
        v1 += 0x800;
        v0 += v1;
        v0 &= 0xFFF;
        v1 = (int)D_800966EC;
        v0 <<= 2;
        v0 += v1;
        v1 = RW16(v0, 2);
        *(short *)(state + 0x12) = 0;
        *(volatile short *)(state + 0x10) = v1;
        a0 = RW32(v0, 0);
        v1 = RWU16(state, 0x10);
        v0 = 0x1000;
        *(volatile short *)(state + 0x18) = 0x1000;
        *(volatile short *)(state + 0x14) = a0;
        v0 = RWU16(state, 0x14);
        *(short *)(state + 0x16) = 0;
        *(volatile short *)(state + 0x1A) = 0;
        *(volatile short *)(state + 0x1E) = 0;
        *(short *)(state + 0x20) = v1;
        v1 = (int)D_8009D254;
        v0 = -v0;
        *(short *)(state + 0x1C) = v0;
        v0 = RW16(v1, 0x2E);
        RW16(state, 0x84) = v0;
    }

    RW32(scratch, 0x48) = 0;
    if (RW8(state, 0x8C) == 0) {
        switch (((int (*)(RoomEnt *))func_800DFB78)(self)) {
        case 0:
            break;
        case 1:
            RW8(state, 0x8C) = 1;
            RW32(state, 0x78) = 0;
            RW32(state, 0x7C) = 0;
            if (RW32(state, 0x4) != 0) {
                RW32((void *)RW32(state, 0x4), 0) = 2;
            }
            return;
        case 2:
            return;
        default:
            break;
        }
    }

    if (RW8(link, 0xE) < 2) {
        return;
    }
    {
        register int y asm("$5") = link->pos[1];
        int phase = RW32(state, 0x74);
        int speedRaw;
        int speed;
        int phaseAgain;
        int nextY;
        scratch[0x10] = 0;
        scratch[0x11] = 0;
        speedRaw = RW32(state, 0x68);
        speed = speedRaw >> 12;
        scratch[0x12] = speed;
        phaseAgain = RW32(state, 0x74);
        nextY = y - phase;
        if (phaseAgain < 0 && (nextY >> 16) >= RW16(state, 0x84)) {
            register int numerator asm("$3");
            register int divisor asm("$2");

            nextY = RW16(state, 0x84) << 16;
            numerator = (short)speed;
            divisor = (y - nextY) >> 12;
            numerator *= divisor;
            divisor = phaseAgain >> 12;
            scratch[0x12] = numerator / divisor;
            RW32(scratch, 0x48) = 1;
        }
        link->pos[1] = nextY;
    }

    gte_ldrotmatrix(state + 0x10);
    ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR(scratch);
    gte_mvmva();
    gte_stmac((char *)scratch + 0x28);
    link->pos[0] += RW32(scratch, 0x28) << 12;
    link->pos[2] += RW32(scratch, 0x30) << 12;

    {
        register int previous asm("$5") = RW32(state, 0x74);
        register int delta asm("$2") = RW32(state, 0x80);
        int enabled = RW16(state, 0x88);
        register int current asm("$4") = previous - delta;
        RW32(state, 0x74) = current;
        if (enabled != 0) {
            register int crossed asm("$2") = previous ^ current;

            if (crossed < 0) {
                RW32(state, 0x0) = (int)RoomLib_HandlerBPhase;
                RW8(state, 0xE) = 0;
                self->flag3 = 2;
            }
        }
    }
    {
        int speed = RW32(state, 0x68) + RW32(state, 0x70);
        int limit = RW32(state, 0x6C);
        RW32(state, 0x68) = speed;
        if (limit != 0 && speed > limit) {
            RW32(state, 0x68) = limit;
        }
    }
    if (RW16(state, 0x8A) > 0) {
        RoomLink *secondary = (RoomLink *)RW32(state, 0x64);
        if (secondary != 0) {
            RW32(state, 0x50) = secondary->pos[0];
            RW32(state, 0x58) = secondary->pos[2];
        }
        angle = FieldEng_VecToAngle((int *)(state + 0x50), link->pos);
        RW16(link, 0x3A) = FieldEng_TurnToward(
            RW16(link, 0x3A), angle, RW16(state, 0x8A));
    }
    func_800DFB20(self);
    if (RW32(scratch, 0x48) != 0) {
        RoomLib_ReleaseHandlerB(self);
    }
}

void RoomLib_HandlerBPhase(RoomEnt *obj) {
}

void RoomLib_AdvanceArcToTarget(RoomEnt *obj) {
    register RoomEnt *self = obj;
    register char *state = (char *)&self->sub;
    register RoomLink *link = self->link;
    register short *scratch = (short *)0x1F800000;
    int angle;
    int y;

    if (self->t1A == 0) {
        int count;
        register int step;
        register int targetY;
        register int accel;
        self->t1A = 1;
        RW8(self, 0x98) = 0;
        if (RW32(self, 0x6C) != 0) {
            RW32((void *)0x1F800000, 0x28) =
                RW32((void *)RW32(self, 0x6C), 0x28);
            RW32((void *)0x1F800000, 0x2C) =
                RW32((void *)RW32(self, 0x6C), 0x2C);
            RW32((void *)0x1F800000, 0x30) =
                RW32((void *)RW32(self, 0x6C), 0x30);
        } else {
            RW32((void *)0x1F800000, 0x28) = RW32(self, 0x3C);
            RW32((void *)0x1F800000, 0x2C) = RW32(self, 0x40);
            RW32((void *)0x1F800000, 0x30) = RW32(self, 0x44);
        }

        angle = FieldEng_VecToAngle(link->pos, (int *)&scratch[20]) & 0xFFF;
        {
            int *rotBase = (int *)D_800966EC;
            int *entry = rotBase + angle;
            int hi = ((short *)entry)[1];
            int lo;
            int savedHi;
            scratch[1] = 0;
            *(volatile short *)&scratch[0] = hi;
            lo = *entry;
            savedHi = RWU16(scratch, 0x0);
            scratch[3] = 0;
            scratch[5] = 0;
            scratch[7] = 0;
            scratch[2] = lo;
            scratch[4] = 0x1000;
            scratch[8] = savedHi;
            scratch[6] = -RWU16(scratch, 0x4);
        }
        scratch[16] = RW32(state, 0x40) >> 12;
        scratch[17] = 0;
        scratch[18] = RW32(state, 0x48) >> 12;
        gte_ldrotmatrix((void *)scratch);
        gte_ldv0((void *)((int)scratch + 0x20));
        gte_mvmva();
        gte_stmac((void *)((int)scratch + 0x38));

        RW32(state, 0x30) = RW32(scratch, 0x28) +
                            (RW32(scratch, 0x38) << 12);
        RW32(state, 0x34) = RW32(scratch, 0x2C) +
                            (RW32(scratch, 0x3C) << 12);
        RW32(state, 0x38) = RW32(scratch, 0x30) +
                            (RW32(scratch, 0x40) << 12);

        count = 0;
        step = RW32(state, 0x74);
        accel = RW32(state, 0x80);
        targetY = RW32(state, 0x34);
        y = link->pos[1];
        do {
            y += step;
            step += accel;
            count++;
        } while (y < targetY);
        {
            /* Both horizontal divisions share the v0 numerator lifetime. */
            register int value asm("$2");
            int base;
            value = RW32(state, 0x30);
            base = link->pos[0];
            value -= base;
            value /= count;
            RVW32(state, 0x78) = value;
            value = RW32(state, 0x38);
            base = link->pos[2];
            value -= base;
            value /= count;
            RVW32(state, 0x7C) = value;
        }
    }

    RW32(scratch, 0x48) = 0;
    if (RW8(state, 0x8C) == 0) {
        switch (((int (*)(RoomEnt *))func_800DFB78)(self)) {
        case 0:
            break;
        case 1:
            RW8(state, 0x8C) = 1;
            RW32(state, 0x78) = 0;
            RW32(state, 0x7C) = 0;
            if (RW32(state, 0x4) != 0) {
                RW32((void *)RW32(state, 0x4), 0) = 2;
            }
            break;
        case 2:
            return;
        default:
            break;
        }
    }

    if (RW8(link, 0xE) < 2) {
        return;
    }
    link->pos[0] += RW32(state, 0x78);
    link->pos[1] += RW32(state, 0x74);
    link->pos[2] += RW32(state, 0x7C);
    RW32(state, 0x74) += RW32(state, 0x80);
    if (RW16(link, 0x2E) >= RW16(state, 0x36)) {
        RW32(scratch, 0x48) = 1;
        link->pos[0] = RW32(state, 0x30);
        link->pos[1] = RW32(state, 0x34);
        link->pos[2] = RW32(state, 0x38);
    }
    func_800DFB20(self);
    if (RW32(scratch, 0x48) != 0) {
        RoomLib_ReleaseHandlerB(self);
    }
}

int RoomLib_ReleaseHandlerB(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    if (o->active != 0) {
        RoomLink *l = o->link;
        if (l->target != 0) {
            *l->target->state = 4;
        }
    }
    o->flag3 = 0;
    o->state = 4;
    if (s->signal != 0) {
        *s->signal = 0;
    }
    return 0;
}
int RoomLib_HandlerBNop6(void) {
    return 0;
}

int RoomLib_HandlerCNop0(void) {
    return 0;
}
int RoomLib_InitHandlerC(RoomEnt *o) {
    RoomLink *l;
    o->flag3 = 1;
    o->t17 = -1;
    o->t16 = -1;
    o->t19 = 7;
    o->w94 = 0x10000;
    o->h9C = 0x10;
    l = o->link;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    o->w7C = 0;
    o->w5C = 0;
    o->w60 = 0;
    o->w64 = 0;
    o->h9E = 0;
    o->sub.cb = RoomLib_HandlerC;
    o->w80 = 0;
    o->hA0 = 0;
    l->vel[0] = 0;
    l->vel[1] = 0;
    l->vel[2] = 0;
    l->accel[0] = 0;
    l->accel[1] = 0;
    l->accel[2] = 0;
    l->move[0] = 0;
    l->move[1] = 0;
    l->move[2] = 0;
    return 0;
}
int RoomLib_ConfigureHandlerC(RoomEnt *o, int query, unsigned int op, int arg0, int arg1, int arg2) {
    RoomLibHandlerCState *state = (RoomLibHandlerCState *)&o->sub;
    FieldActorNode *node;
    switch (op) {
    case 19:
        if (query == 0) {
            o->flag3 = arg0;
        } else {
            *(int *)arg0 = o->flag3;
        }
        break;
    case 25:
        if (query == 1) {
            state->signal = (int *)arg0;
            *(int *)arg0 = query;
        }
        break;
    case 0:
        state->targetLink = (RoomLink *)D_8009D20C;
        while (state->targetLink != 0) {
            node = (FieldActorNode *)state->targetLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->targetLink = (RoomLink *)
                ((FieldActorNode *)state->targetLink)->next;
        }
        break;
    case 17:
        state->target[0] = arg0;
        state->target[2] = arg2;
        if (arg1 == -1) {
            state->target[1] = RW32(D_8009D254, 0x2C);
        } else {
            state->target[1] = arg1;
        }
        state->targetLink = 0;
        break;
    case 15:
        state->rate = arg0;
        break;
    case 13:
        state->mode = arg0;
        break;
    case 6:
        state->localOffset[0] = arg0;
        state->localOffset[1] = arg1;
        state->localOffset[2] = arg2;
        break;
    case 23:
        arg0 = arg0 != 0;
        arg1 = (arg1 != 0) << 1;
        arg2 = (arg2 != 0) << 2;
        state->flags = arg0 | arg1 | arg2;
        break;
    case 10:
        state->variant = arg0;
        state->active = arg1;
        if (o->flag3 == 1) {
            state->callback = (void (*)(void))RoomLib_ArmHandlerC;
        }
        break;
    case 11:
        if (o->flag3 == 1) {
            state->optionB = arg0;
            state->optionC = arg1;
        }
        break;
    case 16:
        state->heading = arg0;
        break;
    case 18:
        if (o->flag3 == 2) {
            state->callback = (void (*)(void))RoomLib_AdvanceArcToTargetY;
            state->phaseValue = arg0;
            o->flag3 = 3;
        }
        break;
    case 21:
        state->secondaryLink = (RoomLink *)D_8009D20C;
        while (state->secondaryLink != 0) {
            node = (FieldActorNode *)state->secondaryLink;
            if (node->b0C == arg0 && node->b0D == arg1 &&
                (node->w98 & 0x10) == 0) {
                break;
            }
            state->secondaryLink = (RoomLink *)
                ((FieldActorNode *)state->secondaryLink)->next;
        }
        state->secondaryHeading = arg2;
        break;
    case 22:
        state->secondaryX = arg0;
        state->secondaryZ = arg1;
        state->secondaryHeading = arg2;
        break;
    }
    return 0;
}
int RoomLib_HandlerCNop3(void) {
    return 0;
}
int RoomLib_UpdateHandlerC(RoomObj *obj) {
    obj->callback();
    return 0;
}
void RoomLib_ArmHandlerC(RoomEnt *o) {
    signed char c = o->t16;
    int t;
    unsigned short lo;
    if (c >= 0) {
        if (c != o->link->variant) {
            return;
        }
    }
    t = o->t17;
    if (t >= 0) {
        RoomLink *l = o->link;
        lo = l->winLo;
        if (t >= l->winHi && t <= lo) {
            o->sub.cb = RoomLib_HandlerC;
        }
    } else {
        o->sub.cb = RoomLib_HandlerC;
    }
}

void RoomLib_HandlerC(RoomEnt *obj) {
    RoomEnt *self = obj;
    RoomLibHandlerCState *state =
        (RoomLibHandlerCState *)((char *)&self->sub);
    RoomLink *link = self->link;
    short *scratch;
    int angle;
    scratch = (short *)0x1F800000;
    if (self->t1A == 0) {
        self->t1A = 1;
        RW8(self, 0xA2) = 0;
        RW32(link, 0x98) = (RW32(link, 0x98) & 0xFFF3FFFF) | 2;
        if (self->active != 0 && link->target != 0) {
            link->target->flags |= 0x40000000;
        }
        state->phaseValue = (state->mode * (state->rate - 1)) >> 1;
        if (state->targetLink == 0) {
            RW32(scratch, 0x28) = state->target[0];
            RW32(scratch, 0x2C) = state->target[1];
            RW32(scratch, 0x30) = state->target[2];
            if (RW32(scratch, 0x2C) == -1) {
                RW32(scratch, 0x2C) = RW32(D_8009D254, 0x2C);
            }
        } else {
            RW32(scratch, 0x28) = state->targetLink->pos[0];
            RW32(scratch, 0x2C) = state->targetLink->pos[1];
            RW32(scratch, 0x30) = state->targetLink->pos[2];
        }

        angle = FieldEng_VecToAngle(link->pos, (int *)((int)scratch | 0x28)) &
                0xFFF;
        {
            int *rotBase = (int *)D_800966EC;
            int *entry = rotBase + angle;
            int hi = ((short *)entry)[1];
            int lo;
            int savedHi;
            scratch[1] = 0;
            *(volatile short *)&scratch[0] = hi;
            lo = *entry;
            savedHi = RWU16(scratch, 0x0);
            scratch[3] = 0;
            scratch[5] = 0;
            scratch[7] = 0;
            scratch[2] = lo;
            scratch[4] = 0x1000;
            scratch[8] = savedHi;
            scratch[6] = -RWU16(scratch, 0x4);
        }
        scratch[16] = state->localOffset[0] >> 12;
        scratch[17] = 0;
        scratch[18] = state->localOffset[2] >> 12;
        gte_ldrotmatrix((void *)scratch);
        gte_ldv0((char *)scratch + 0x20);
        gte_mvmva();
        gte_stmac((char *)scratch + 0x38);

        RW32(state, 0x40) = RW32(scratch, 0x28) +
                            (RW32(scratch, 0x38) << 12);
        RW32(state, 0x44) = RW32(scratch, 0x2C) +
                            (RW32(scratch, 0x3C) << 12);
        RW32(state, 0x48) = RW32(scratch, 0x30) +
                            (RW32(scratch, 0x40) << 12);
        RW32(state, 0x80) =
            (RW32(state, 0x40) - link->pos[0]) / state->rate;
        RW32(state, 0x84) =
            (RW32(state, 0x48) - link->pos[2]) / state->rate;
    }

    RW32(scratch, 0x48) = 0;
    if (RW8(state, 0x96) == 0) {
        switch (((int (*)(RoomEnt *))func_800DFB78)(self)) {
        case 0:
            break;
        case 1:
            RW8(state, 0x96) = 1;
            RW32(state, 0x80) = 0;
            RW32(state, 0x84) = 0;
            if (state->signal != 0) {
                *state->signal = 2;
            }
            break;
        case 2:
            return;
        default:
            break;
        }
    }

    if (RW8(link, 0xE) < 2) {
        return;
    }
    link->pos[0] += RW32(state, 0x80);
    link->pos[1] -= state->phaseValue;
    link->pos[2] += RW32(state, 0x84);
    {
        int previous = state->phaseValue;
        state->phaseValue = previous - state->mode;
        if (state->heading != 0 && (previous ^ state->phaseValue) < 0) {
            state->callback = RoomLib_HandlerCPhase;
            RW8(state, 0xE) = 0;
            state->localOffset[0] = 0;
            state->localOffset[1] = 0;
            state->localOffset[2] = 0;
            self->flag3 = 2;
        }
    }
    if (state->phaseValue < 0) {
        int y = link->pos[1];
        int targetY = RW32(state, 0x44);
        int delta = y - targetY;
        if (delta >= 0) {
            link->pos[1] = targetY;
            RW32(scratch, 0x48) = 1;
        }
    }
    if (state->secondaryHeading > 0) {
        RoomLink *secondary = state->secondaryLink;
        if (secondary != 0) {
            state->secondaryX = secondary->pos[0];
            state->secondaryZ = secondary->pos[2];
        }
        angle = FieldEng_VecToAngle(&state->secondaryX, link->pos);
        RW16(link, 0x3A) = FieldEng_TurnToward(
            RW16(link, 0x3A), angle, state->secondaryHeading);
    }
    func_800DFB20(self);
    if (RW32(scratch, 0x48) != 0) {
        RoomLib_ReleaseHandlerC(self);
    }
}

void RoomLib_HandlerCPhase(void) {
}

void RoomLib_AdvanceArcToTargetY(RoomEnt *obj) {
    register RoomEnt *self = obj;
    register char *state = (char *)&self->sub;
    register RoomLink *link = self->link;
    register short *scratch = (short *)0x1F800000;
    int angle;
    int y;

    if (self->t1A == 0) {
        int count;
        register int step;
        register int targetY;
        register int accel;
        self->t1A = 1;
        RW8(self, 0xA2) = 0;
        if (RW32(self, 0x7C) == 0) {
            RW32((void *)0x1F800000, 0x28) = RW32(self, 0x3C);
            RW32((void *)0x1F800000, 0x2C) = RW32(self, 0x40);
            RW32((void *)0x1F800000, 0x30) = RW32(self, 0x44);
        } else {
            RW32((void *)0x1F800000, 0x28) =
                RW32((void *)RW32(self, 0x7C), 0x28);
            RW32((void *)0x1F800000, 0x2C) =
                RW32((void *)RW32(self, 0x7C), 0x2C);
            RW32((void *)0x1F800000, 0x30) =
                RW32((void *)RW32(self, 0x7C), 0x30);
        }

        angle = FieldEng_VecToAngle(link->pos, (int *)&scratch[20]) & 0xFFF;
        {
            int *rotBase = (int *)D_800966EC;
            int *entry = rotBase + angle;
            int hi = ((short *)entry)[1];
            int lo;
            int savedHi;
            scratch[1] = 0;
            *(volatile short *)&scratch[0] = hi;
            lo = *entry;
            savedHi = RWU16(scratch, 0x0);
            scratch[3] = 0;
            scratch[5] = 0;
            scratch[7] = 0;
            scratch[2] = lo;
            scratch[4] = 0x1000;
            scratch[8] = savedHi;
            scratch[6] = -RWU16(scratch, 0x4);
        }
        scratch[16] = RW32(state, 0x50) >> 12;
        scratch[17] = 0;
        scratch[18] = RW32(state, 0x58) >> 12;
        gte_ldrotmatrix((void *)scratch);
        gte_ldv0((void *)((int)scratch + 0x20));
        gte_mvmva();
        gte_stmac((void *)((int)scratch + 0x38));

        RW32(state, 0x40) = RW32(scratch, 0x28) +
                            (RW32(scratch, 0x38) << 12);
        RW32(state, 0x44) = RW32(scratch, 0x2C) +
                            (RW32(scratch, 0x3C) << 12);
        RW32(state, 0x48) = RW32(scratch, 0x30) +
                            (RW32(scratch, 0x40) << 12);

        count = 0;
        step = RW32(state, 0x7C);
        accel = RW32(state, 0x88);
        targetY = RW32(state, 0x44);
        y = link->pos[1];
        do {
            y += step;
            step += accel;
            count++;
        } while (y < targetY);
        {
            /* Both horizontal divisions share the v0 numerator lifetime. */
            register int value asm("$2");
            int base;
            value = RW32(state, 0x40);
            base = link->pos[0];
            value -= base;
            value /= count;
            RVW32(state, 0x80) = value;
            value = RW32(state, 0x48);
            base = link->pos[2];
            value -= base;
            value /= count;
            RVW32(state, 0x84) = value;
        }
    }

    RW32(scratch, 0x48) = 0;
    if (RW8(state, 0x96) == 0) {
        switch (((int (*)(RoomEnt *))func_800DFB78)(self)) {
        case 0:
            break;
        case 1:
            RW8(state, 0x96) = 1;
            RW32(state, 0x80) = 0;
            RW32(state, 0x84) = 0;
            if (RW32(state, 0x4) != 0) {
                RW32((void *)RW32(state, 0x4), 0) = 2;
            }
            break;
        case 2:
            return;
        default:
            break;
        }
    }

    if (RW8(link, 0xE) < 2) {
        return;
    }
    link->pos[0] += RW32(state, 0x80);
    link->pos[1] += RW32(state, 0x7C);
    link->pos[2] += RW32(state, 0x84);
    RW32(state, 0x7C) += RW32(state, 0x88);
    if (RW16(link, 0x2E) >= RW16(state, 0x46)) {
        RW32(scratch, 0x48) = 1;
        link->pos[1] = RW32(state, 0x44);
    }
    func_800DFB20(self);
    if (RW32(scratch, 0x48) != 0) {
        RoomLib_ReleaseHandlerC(self);
    }
}

int RoomLib_ReleaseHandlerC(RoomEnt *o) {
    struct RoomSub *s = &o->sub;
    if (o->active != 0) {
        RoomLink *l = o->link;
        if (l->target != 0) {
            *l->target->state = 4;
        }
    }
    o->flag3 = 0;
    o->state = 4;
    if (s->signal != 0) {
        *s->signal = 0;
    }
    return 0;
}
int RoomLib_HandlerCNop6(void) {
    return 0;
}

int RoomLib_HandlerANop0(void) {
    return 0;
}
int RoomLib_InitHandlerA(RoomEnt *o) {
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    o->sub.cb = RoomLib_HandlerA;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    o->h46 = 0;
    return 0;
}
int RoomLib_ConfigureHandlerA(RoomEnt *o, int arg1, unsigned int op, int arg3, int sp10, int sp14) {
    if (op == 0xA) {
        goto case10;
    }
    if (op >= 0xB) {
        goto high;
    }
    if (op == 4) {
        goto case4;
    }
    goto done;
high:
    if (op == 0x19) {
        goto case25;
    }
    if (op == 0x1C) {
        goto case28;
    }
    goto done;
case25:
    if (arg1 != 1) {
        goto done;
    }
    o->sub.signal = (int *)arg3;
    *(int *)arg3 = arg1;
    goto done;
case4:
    o->pos[0] = arg3;
    o->pos[1] = sp10;
    o->h44 = sp14;
    goto done;
case28:
    o->h48 = arg3;
    o->h46 = sp10;
    goto done;
case10:
    o->t16 = arg3;
    o->sub.cb = (void (*)(void))RoomLib_RearmHandlerA;
done:
    return 0;
}
int RoomLib_HandlerANop3(void) {
    return 0;
}
int RoomLib_UpdateHandlerA(RoomObj *obj) {
    obj->callback();
    return 0;
}
void RoomLib_RearmHandlerA(RoomEnt *o) {
    if (o->link->variant == o->t16) {
        o->sub.cb = RoomLib_HandlerA;
    }
}
void RoomLib_HandlerA(RoomEnt *o) {
    char *g = D_8009D254;
    if (RW8(g, 0xE) < 4) {
        int *sig;
        RW32(g, 0x98) &= 0xFFF3FFFF;
        sig = o->sub.signal;
        o->sub.cb = RoomLib_HandlerF;
        if (sig != 0) {
            *sig = 2;
        }
        if (RW16(o, 0x44) == -1) {
            RW16(o, 0x44) = FieldEng_VecToAngle(
                (int *)((char *)o->link + 0x28),
                (int *)(D_8009D254 + 0x28));
        }
        if (RW16(o, 0x48) == -1) {
            RW16(o, 0x48) = RWU16(o, 0x44) + 0x800;
        }
        {
            int *e = (int *)((char *)D_800966EC
                             + ((RWU16(o, 0x44) & 0xFFF) << 2));
            int hi = ((short *)e)[1];
            int lo;
            int savedHi;
            RVW16(o, 0x1E) = 0;
            RVW16(o, 0x1C) = hi;
            lo = *e;
            savedHi = RWU16(o, 0x1C);
            RVW16(o, 0x22) = 0;
            RVW16(o, 0x26) = 0;
            RVW16(o, 0x2A) = 0;
            RVW16(o, 0x20) = lo;
            RVW16(o, 0x24) = 0x1000;
            RVW16(o, 0x2C) = savedHi;
            RVW16(o, 0x28) = -RWU16(o, 0x20);
        }
    }
}
void RoomLib_HandlerF(RoomEnt *o) {
    int height;
    if (RW8(D_8009D254, 0xE) >= 4) {
        RoomLib_ReleaseHandlerA(o);
    } else if (RW32(D_8009D254, 0x98) & 0xC0000) {
        RW32(D_8009D254, 0x98) &= 0xFFF3FFFF;
        RoomLib_ReleaseHandlerA(o);
    } else {
        volatile short *scratch = (volatile short *)0x1F800000;
        scratch[0] = 0;
        scratch[1] = 0;
        scratch[2] = o->pos[0] >> 12;
        gte_ldrotmatrix(o->mat);
        gte_ldv0((void *)scratch);
        gte_mvmva();
        gte_stmac((void *)&scratch[4]);
        RW32(D_8009D254, 0x28) += *(int *)&scratch[4] << 12;
        RW32(D_8009D254, 0x30) += *(int *)&scratch[8] << 12;
        height = o->pos[0] - o->pos[1];
        o->pos[0] = height;
        if (height < 0) {
            RoomLib_ReleaseHandlerA(o);
        }
        if (o->h46 != 0) {
            RW16(D_8009D254, 0x3A) = FieldEng_TurnToward(
                RW16(D_8009D254, 0x3A), o->h48, o->h46);
        }
    }
}
int RoomLib_ReleaseHandlerA(RoomEnt *o) {
    int *p = o->sub.signal;
    o->state = 4;
    if (p != 0) {
        *p = 0;
    }
    return 0;
}
int RoomLib_HandlerANop6(void) {
    return 0;
}

