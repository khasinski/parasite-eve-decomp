#include "common.h"
extern volatile u16 D_1F801D98;
extern volatile u16 D_1F801D9A;

void Spu_WriteReverbEnable(u32 value) {
    D_1F801D98 = value;
    value >>= 16;
    D_1F801D9A = value;
}
