#include "../room_lib/room_lib.h"

int func_800DFB78(void);
extern void func_80196B0C(RoomEnt *o);

int func_801963D4(RoomEnt *o) {
    int step = func_800DFB78();

    if (step == 1) {
        goto step_one;
    }
    if (step >= 2) {
        goto done;
    }
    if (step != 0) {
        goto done;
    }

    if (o->link->variant < 2) {
        goto done;
    }
    ((void (*)(RoomEnt *))o->sub.cb)(o);
    asm volatile(
        ".set\tnoreorder\n\t"
        "j\t.Lfunc_801963D4_after_tick_jal\n\t"
        "move\t$2, $0\n\t"
        ".set\treorder"
        :
        :
        : "$2");

step_one:
    asm volatile(
        ".set\tnoreorder\n\t"
        "jal\tfunc_80196B0C\n"
        ".Lfunc_801963D4_after_tick_jal:\n\t"
        "move\t$4, %0\n\t"
        ".set\treorder"
        :
        : "r"(o)
        : "$4", "$31", "memory");

done:
    return 0;
}
