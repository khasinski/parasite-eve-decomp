/* MASPSX_FLAGS: --expand-div */
#define RoomLib_HandlerC RoomLib_HandlerC_decl
#include "room_lib.h"
#undef RoomLib_HandlerC

#ifndef ROOMLIB_HANDLER_C_PHASE
#define ROOMLIB_HANDLER_C_PHASE func_80191D10
#endif
#ifndef ROOMLIB_HANDLER_C_RESET
#define ROOMLIB_HANDLER_C_RESET RoomLib_ResetAndSignal_801920A0
#endif
#ifndef ROOMLIB_HANDLER_C_ACTOR_PTR
#define ROOMLIB_HANDLER_C_ACTOR_PTR RoomMain_ActorPtr
#endif
#ifndef ROOMLIB_HANDLER_C_ROT_TABLE
#define ROOMLIB_HANDLER_C_ROT_TABLE RoomMain_RotTable
#endif

extern void ROOMLIB_HANDLER_C_PHASE(void);
extern int ROOMLIB_HANDLER_C_RESET(RoomEnt *obj);

void RoomLib_HandlerC(RoomEnt *obj) {
    RoomEnt *self = obj;
    RoomLibHandlerCState *state =
        (RoomLibHandlerCState *)((char *)self + 0xC);
    RoomLink *link = self->link;
    short *scratch = (short *)0x1F800000;
    int angle;

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
                RW32(scratch, 0x2C) = RW32(ROOMLIB_HANDLER_C_ACTOR_PTR, 0x2C);
            }
        } else {
            RW32(scratch, 0x28) = state->targetLink->pos[0];
            RW32(scratch, 0x2C) = state->targetLink->pos[1];
            RW32(scratch, 0x30) = state->targetLink->pos[2];
        }

        angle = FieldEng_VecToAngle(link->pos, (int *)((char *)scratch + 0x28)) &
                0xFFF;
        {
            int *rotBase = (int *)ROOMLIB_HANDLER_C_ROT_TABLE;
            int *entry = rotBase + angle;
            int hi = *(short *)((char *)entry + 2);
            int lo;
            int savedHi;
            *(volatile short *)&scratch[1] = 0;
            *(volatile short *)&scratch[0] = hi;
            lo = *entry;
            savedHi = RWU16(scratch, 0x0);
            *(volatile short *)&scratch[3] = 0;
            *(volatile short *)&scratch[5] = 0;
            *(volatile short *)&scratch[7] = 0;
            *(volatile short *)&scratch[2] = lo;
            *(volatile short *)&scratch[4] = 0x1000;
            *(volatile short *)&scratch[8] = savedHi;
            *(volatile short *)&scratch[6] = -RWU16(scratch, 0x4);
        }
        scratch[16] = state->localOffset[0] >> 12;
        scratch[17] = 0;
        scratch[18] = state->localOffset[2] >> 12;
        gte_ldrotmatrix((void *)scratch);
        gte_ldv0((void *)((char *)scratch + 0x20));
        gte_mvmva();
        gte_stmac((void *)((char *)scratch + 0x38));

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
            state->callback = ROOMLIB_HANDLER_C_PHASE;
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
        ROOMLIB_HANDLER_C_RESET(self);
    }
}
