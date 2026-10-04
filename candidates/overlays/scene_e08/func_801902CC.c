/* Non-matching reconstruction of scene_e08 [0x12E4, 0x1478).
 * Stock GCC 2.7.2 emits the correct 0x194 bytes. Four words at +0x38..+0x44
 * remain reordered: retail loads the division constant before initializing
 * position and angle; GCC does the reverse. Do not count this as a match.
 */
#include "common.h"

void *func_800C2B50(void);
int func_80071A54(void);

void func_801902CC(void *unused0, void *unused1, char *effect) {
    char *base = effect;
    char *owner4;
    register char *position asm("$17");
    register char *angle asm("$18");
    register unsigned i asm("$19");
    char *state;
    int r1, r2;
    int c1, c2;
    owner4 = (char *)func_800C2B50() + 4;
    *(u8 *)(base + 0x44) = 4;
    i = 0;
    position = base;
    angle = position;
    for (; i < 4; i++) {
        state = base + i;
        *(u8 *)(state + 0x3C) = 0;
        r1 = func_80071A54(); c1 = *(s32 *)(owner4 + 0x14) - 100;
        *(s16 *)(position + 0) = c1 + r1 % 200;
        *(s16 *)(position + 2) = *(s32 *)(owner4 + 0x18);
        r2 = func_80071A54(); c2 = *(s32 *)(owner4 + 0x1C) - 100;
        *(s16 *)(position + 4) = c2 + r2 % 200;
        *(u8 *)(state + 0x40) = func_80071A54() % 5 + 1;
        *(s16 *)(angle + 0x20) = func_80071A54() % 256 + 0x400;
        *(u8 *)(state + 0x30) = 0x48;
        *(u8 *)(state + 0x34) = 4;
        *(s16 *)(angle + 0x28) = 0x40;
        angle += 2; position += 8;
        *(u8 *)(state + 0x38) = 0;
    }
}
