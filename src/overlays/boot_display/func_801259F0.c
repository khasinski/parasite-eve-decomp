#include "common.h"

extern volatile int D_80172CFC;
extern volatile int D_80172CF0;
extern int func_80080C48(int);

void func_801259F0(u8 mode, int value) {
    if (mode == 2) {
        D_80172CFC = func_80080C48(value + 5);
        if (D_80172CFC != 0) {
            D_80172CF0 = D_80172CFC;
        }
    }
}
