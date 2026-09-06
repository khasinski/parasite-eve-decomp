/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "common.h"

extern volatile u16 *_spu_RXX;

typedef struct SpuVoiceWaitWork {
    volatile int i;
    volatile int value;
} SpuVoiceWaitWork;

void Spu_SetVoiceAttr(int voice, u16 left, u16 right, s16 leftMode, u16 rightMode) {
    SpuVoiceWaitWork work;
    unsigned int leftFlags;
    unsigned int rightFlags;
    int current;

    left &= 0x7FFF;
    leftFlags = 0;
    voice <<= 3;
    switch ((s16)(leftMode - 1)) {
    case 0: leftFlags = 0x8000; break;
    case 1: leftFlags = 0x9000; break;
    case 2: leftFlags = 0xA000; break;
    case 3: leftFlags = 0xB000; break;
    case 4: leftFlags = 0xC000; break;
    case 5: leftFlags = 0xD000; break;
    case 6: leftFlags = 0xE000; break;
    }
    leftMode = right & 0x7FFF;
    rightFlags = 0;
    _spu_RXX[voice] = left | leftFlags;
    switch ((s16)(rightMode - 1)) {
    case 0: rightFlags = 0x8000; break;
    case 1: rightFlags = 0x9000; break;
    case 2: rightFlags = 0xA000; break;
    case 3: rightFlags = 0xB000; break;
    case 4: rightFlags = 0xC000; break;
    case 5: rightFlags = 0xD000; break;
    case 6: rightFlags = 0xE000; break;
    }
    _spu_RXX[voice + 1] = leftMode | rightFlags;
    work.value = 1;
    /* Keep the counter initialization after the delay seed store. */
    asm volatile("" : : : "memory");
    *(int *)&work.i = 0;
    while (work.i < 2) {
        current = work.value;
        work.value = (((current << 1) + current) << 2) + current;
        work.i = work.i + 1;
    }
}
