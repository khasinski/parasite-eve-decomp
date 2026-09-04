/* MASPSX_FLAGS: --expand-div */
#define RoomLib_HandlerB RoomLib_HandlerB_decl
#include "room_lib.h"
#undef RoomLib_HandlerB

#ifndef ROOMLIB_HANDLER_B_PHASE
#define ROOMLIB_HANDLER_B_PHASE func_80191108
#endif
#ifndef ROOMLIB_HANDLER_B_RESET
#define ROOMLIB_HANDLER_B_RESET RoomLib_ResetAndSignal_801914B0
#endif
#ifndef ROOMLIB_HANDLER_B_ACTOR_PTR
#define ROOMLIB_HANDLER_B_ACTOR_PTR RoomMain_ActorPtr
#endif
#ifndef ROOMLIB_HANDLER_B_ROT_TABLE
#define ROOMLIB_HANDLER_B_ROT_TABLE RoomMain_RotTable
#endif
#ifndef ROOMLIB_HANDLER_B_NAME
#define ROOMLIB_HANDLER_B_NAME RoomLib_HandlerB
#endif

extern void ROOMLIB_HANDLER_B_PHASE();
extern int ROOMLIB_HANDLER_B_RESET(RoomEnt *obj);

#ifndef ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR
#define ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR(base) \
    asm volatile("addiu $2,%0,D_1F800020\n\t" \
                 "lwc2 $0,0($2)\n\t" \
                 "lwc2 $1,4($2)" \
                 : : "r"(base) : "$2", "memory")
#endif

#ifndef ROOMLIB_HANDLER_B_GTE_STORE_MAC
#define ROOMLIB_HANDLER_B_GTE_STORE_MAC(base) \
    asm volatile("addiu $2,%0,D_1F800028\n\t" \
                 "swc2 $25,0($2)\n\t" \
                 "swc2 $26,4($2)\n\t" \
                 "swc2 $27,8($2)" \
                 : : "r"(base) : "$2", "memory")
#endif

void ROOMLIB_HANDLER_B_NAME(RoomEnt *obj) {
    RoomEnt *self = obj;
    char *state = (char *)self + 0xC;
    RoomLink *link = self->link;
    short *scratch = (short *)0x1F800000;
    int angle;

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
        v1 = (int)ROOMLIB_HANDLER_B_ROT_TABLE;
        v0 <<= 2;
        v0 += v1;
        v1 = RW16(v0, 2);
        *(volatile short *)(state + 0x12) = 0;
        *(volatile short *)(state + 0x10) = v1;
        a0 = RW32(v0, 0);
        v1 = RWU16(state, 0x10);
        v0 = 0x1000;
        *(volatile short *)(state + 0x18) = 0x1000;
        *(volatile short *)(state + 0x14) = a0;
        v0 = RWU16(state, 0x14);
        *(volatile short *)(state + 0x16) = 0;
        *(volatile short *)(state + 0x1A) = 0;
        *(volatile short *)(state + 0x1E) = 0;
        *(volatile short *)(state + 0x20) = v1;
        v1 = (int)ROOMLIB_HANDLER_B_ACTOR_PTR;
        v0 = -v0;
        *(volatile short *)(state + 0x1C) = v0;
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
#ifdef ROOMLIB_HANDLER_B_RESET_ON_ARM
                goto reset;
#endif
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
        register int phase asm("$3") = RW32(state, 0x74);
        register int speedRaw asm("$2");
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
    ROOMLIB_HANDLER_B_GTE_STORE_MAC(scratch);
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
                RW32(state, 0x0) = (int)ROOMLIB_HANDLER_B_PHASE;
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
#ifdef ROOMLIB_HANDLER_B_RESET_ON_ARM
    if (RW32(scratch, 0x48) == 0) {
        goto done;
    }
reset:
    ROOMLIB_HANDLER_B_RESET(self);
done:
#else
    if (RW32(scratch, 0x48) != 0) {
        ROOMLIB_HANDLER_B_RESET(self);
    }
#endif
}

#undef ROOMLIB_HANDLER_B_GTE_LOAD_VECTOR
#undef ROOMLIB_HANDLER_B_GTE_STORE_MAC
