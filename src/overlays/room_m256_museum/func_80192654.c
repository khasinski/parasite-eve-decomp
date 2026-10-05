/* MASPSX_FLAGS: --expand-div */
#include "common.h"

extern short D_800966EE[];
extern int func_80192808(char *state);

void func_80192680(char *state);

void func_80192654(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (slot[0xE] == 0xB) {
        *(void (**)(void))(obj + 0xC) = (void (*)(void))func_80192680;
    }
}

void func_80192680(char *state) {
    char *actor = *(char **)(state + 8);
    char *base = state + 0xC;
    int first;
    int phase;
    int value;
    int timer;
    int baseValue;

    if (state[0x1A] == 0) {
        state[0x1A] = 1;
        *(s16 *)(state + 0x32) = 0;
        first = *(u16 *)(base + 0x14 + *(s16 *)(state + 0x1C) * 2);
        *(s16 *)(state + 0x2E) = first;
        *(s16 *)(state + 0x30) =
            *(u16 *)(base + ((int)*(s16 *)(state + 0x1C) << 1) + 0x16) - first;
        *(s16 *)(state + 0x34) =
            *(u16 *)(base + 0x1C + *(s16 *)(state + 0x1C) * 2);
    }

    timer = *(u16 *)(state + 0x32) + 1;
    phase = ((timer << 16) >> 5) / *(s16 *)(state + 0x34);
    *(s16 *)(state + 0x32) = timer;
    value = (-*(s16 *)((char *)D_800966EE + (phase & 0xFFF) * 4) + 0x1000) *
            *(s16 *)(state + 0x30);
    if (value < 0) {
        value += 0x1FFF;
    }

    baseValue = *(u16 *)(state + 0x2E);
    actor[0x25A] = 3;
    actor[0x25B] = 2;
    *(s16 *)(actor + 0x256) = baseValue + (value >> 13);

    if (*(s16 *)(state + 0x32) >= *(s16 *)(state + 0x34)) {
        *(s16 *)(state + 0x1C) = *(u16 *)(state + 0x1C) + 1;
        if (*(s16 *)(state + 0x1C) >= 3) {
            func_80192808(state);
        } else {
            state[0x1A] = 0;
        }
    }

    if (*(u16 *)(actor + 0x16) >= (u8)actor[0xF]) {
        *(int *)(actor + 0x14) = 0;
    }
}
