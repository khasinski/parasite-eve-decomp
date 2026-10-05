#include "common.h"
void func_80191E48(void *arg0, u8 *state, char *work) {
    *(u16 *)(work + 0x12) -= 8;
    *(u16 *)(work + 0x10) += 0x1E;
    *(u16 *)(work + 2) -= 0xF;

    if (*(s16 *)(work + 0x12) < 0) {
        *(u16 *)(work + 0x12) = 0;
        state[1] = 2;
    }
}
