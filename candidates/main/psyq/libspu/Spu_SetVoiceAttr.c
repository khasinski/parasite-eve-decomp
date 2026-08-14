/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */
#include "common.h"

extern u16 *D_8009B3FC;

void Spu_SetVoiceAttr(int voice, int volL, int volR, s16 modeL, u16 modeR) {
    int keyL;
    int keyR;
    volatile int i;
    volatile int d;

    volL &= 0x7FFF;
    keyL = 0;
    voice *= 8;
    switch ((s16)(modeL - 1)) {
    case 0:
        keyL = 0x8000;
        break;
    case 1:
        keyL = 0x9000;
        break;
    case 2:
        keyL = 0xA000;
        break;
    case 3:
        keyL = 0xB000;
        break;
    case 4:
        keyL = 0xC000;
        break;
    case 5:
        keyL = 0xD000;
        break;
    case 6:
        keyL = 0xE000;
        break;
    }

    modeL = volR & 0x7FFF;
    volR = 0;
    D_8009B3FC[voice] = volL | keyL;
    switch ((s16)(modeR - 1)) {
    case 0:
        volR = 0x8000;
        break;
    case 1:
        volR = 0x9000;
        break;
    case 2:
        volR = 0xA000;
        break;
    case 3:
        volR = 0xB000;
        break;
    case 4:
        volR = 0xC000;
        break;
    case 5:
        volR = 0xD000;
        break;
    case 6:
        volR = 0xE000;
        break;
    }

    D_8009B3FC[voice + 1] = modeL | volR;

    d = 1;
    for (i = 0; i < 2; i++) {
        d = d * 13;
    }
}
