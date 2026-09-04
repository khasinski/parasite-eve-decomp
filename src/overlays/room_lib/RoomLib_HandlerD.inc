/* MASPSX_FLAGS: --expand-div */
#define RoomLib_HandlerD RoomLib_HandlerD_decl
#include "room_lib.h"
#undef RoomLib_HandlerD

#ifndef ROOMLIB_HANDLER_D_TRANSFORM
#define ROOMLIB_HANDLER_D_TRANSFORM func_8018FC68
#endif
#ifndef ROOMLIB_HANDLER_D_RESET
#define ROOMLIB_HANDLER_D_RESET func_8018FEA8
#endif
#ifndef ROOMLIB_HANDLER_D_ROT_TABLE
#define ROOMLIB_HANDLER_D_ROT_TABLE RoomMain_RotTable
#endif

extern void ROOMLIB_HANDLER_D_TRANSFORM(void *state, void *scratch);
extern int ROOMLIB_HANDLER_D_RESET(RoomEnt *o);

#define ROOMLIB_HANDLER_D_GTE_STORE_MAC8(base) \
    asm volatile("addiu $2,%0,D_1F800008\n\t" \
                 "swc2 $25,0($2)\n\t" \
                 "swc2 $26,4($2)\n\t" \
                 "swc2 $27,8($2)" \
                 : : "r"(base) : "$2", "memory")

#define ROOMLIB_HANDLER_D_GTE_STORE_MAC18(base) \
    asm volatile("addiu $2,%0,D_1F800018\n\t" \
                 "swc2 $25,0($2)\n\t" \
                 "swc2 $26,4($2)\n\t" \
                 "swc2 $27,8($2)" \
                 : : "r"(base) : "$2", "memory")

void RoomLib_HandlerD(RoomEnt *o) {
    RoomEnt *ent = o;
    char *state = (char *)o + 0xC;
    short *scratch = (short *)0x1F800000;
    RoomLink *link = o->link;
    char *left;
    register char *right asm("$16");
    int value;
    int product;
    int *rot;
    int rot_y;
    int rot_word;
    register int rot_x asm("$4");

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
        ROOMLIB_HANDLER_D_TRANSFORM(state, (void *)scratch);
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
            func_80079754(right, state + 0x10);
        }
    }

    {
        int bias = 0x8000;
        int *table = (int *)ROOMLIB_HANDLER_D_ROT_TABLE;

    scratch[0x34] = 0;
    {
        int angle;
        angle = ((RW32(state, 0x98) + bias) >> 16) - 0x400;
        scratch[0x35] = angle;
        rot = (int *)((char *)table + ((angle & 0xFFF) << 2));
    }
    rot_y = *(short *)((char *)rot + 2);
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
    scratch[0x18] = 0x1000;
    scratch[0x1C] = rot_x;
    scratch[0x1A] = -scratch[0x16];
    scratch[0x02] = RW32(state, 0x88) >> 12;
    gte_ldrotmatrix((void *)((int)scratch | 0x28));
    gte_ldv0((void *)scratch);
    gte_mvmva();
    ROOMLIB_HANDLER_D_GTE_STORE_MAC8(scratch);
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
            ROOMLIB_HANDLER_D_RESET(ent);
        } else {
            scratch[0] = ((((RW32(state, 0x30) + bias) >> 16) *
                           RW32(state, 0x90)) >> 12);
            scratch[1] = 0;
            scratch[2] = (RW32(state, 0x38) + bias) >> 16;
            gte_ldrotmatrix(state + 0x10);
            gte_ldv0((void *)scratch);
            gte_mvmva();
            ROOMLIB_HANDLER_D_GTE_STORE_MAC18(scratch);
        }
    }

    if (RW16(state, 0xA6) != 0 && RW16(state, 0xA6) < RW16(state, 0x9A)) {
        ROOMLIB_HANDLER_D_RESET(ent);
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
            ROOMLIB_HANDLER_D_TRANSFORM(state, (void *)scratch);
            scratch[1] = RW16(state, 0x7A);
            func_800DFE94(state + 0x50, state + 0x40, state + 0x78);
            right = state + 0x78;
            RW16(state, 0x7A) = FieldEng_TurnToward(
                scratch[1], RW16(state, 0x7A), RW16(state, 0x9C)) & 0xFFF;
            func_80079754(right, state + 0x10);
        }
    }
    if (RW8(state, 0xAA) == 0) {
        RoomLib_FxNotify(link, (struct RoomSub *)state, (int)scratch);
    }
    func_800DFB20(ent);
}

#undef ROOMLIB_HANDLER_D_GTE_STORE_MAC8
#undef ROOMLIB_HANDLER_D_GTE_STORE_MAC18
