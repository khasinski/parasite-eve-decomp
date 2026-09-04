#include "room_lib.h"

extern int func_800C6CE0();

void ROOMLIB_TICK_TRIGGERED_EFFECT_NAME(RoomEnt *ent, unsigned char *signal,
                                        RoomTimer2 *timer) {
    register RoomEnt *ent_reg asm("$17") = ent;
    register unsigned char *signal_reg asm("$18") = signal;
    register RoomTimer2 *timer_reg asm("$16") = timer;
    register RoomTimer2 *destination asm("$4");
    volatile char scratch[8];
    register int initial_countdown asm("$2");
    register int countdown asm("$3");
    register int value asm("$2");

    PE1_COMPILER_LAUNDER(ent_reg);
    PE1_COMPILER_LAUNDER(signal_reg);
    initial_countdown = timer_reg->h2A;
    destination = timer_reg;
    countdown = initial_countdown;

    if (initial_countdown == 0) {
        if (timer_reg->h2C != 1 || func_800C6CE0(ent_reg) != 3 ||
            ent_reg->link->target->state[0] != 2) {
            goto update_signal;
        }

        timer_reg->h2C = 0;
        timer_reg->h2A = timer_reg->h28;
        RW32(*(char **)D_8009D254, 0x4C) |= 0x4000;

        destination = (RoomTimer2 *)ent_reg->link->target;
        value = ((RoomRenderNode *)destination)->flags;
        value |= 0x80000000;
        ((RoomRenderNode *)destination)->flags = value;
    } else {
        value = countdown - 1;
    }

    destination->h2A = value;
    destination->h2C = 0;

update_signal:
    if (func_800C2B68() == 1) {
        signal_reg[1] = 2;
    }
}
