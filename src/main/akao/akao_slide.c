/* MASPSX_FLAGS: --expand-div */

#include "pe1/akao/commands.h"

extern short g_AkaoGlobalD2D0SlideCounter;
extern int g_AkaoGlobalD2D0SlideStep;
extern int D_8009D2D0;

extern short g_AkaoGlobalD2CCSlideCounter;
extern int D_8009D2CC;

void Akao_SlideGlobalD2D0ToTarget(AkaoGlobalSlideCommand *cmd) {
    int raw_duration = cmd->duration;
    int duration = 1;
    int step;

    if (raw_duration != 0) {
        duration = raw_duration;
    }

    step = ((cmd->target << 16) - D_8009D2D0) / duration;
    g_AkaoGlobalD2D0SlideCounter = duration;
    g_AkaoGlobalD2D0SlideStep = step;
}

void Akao_SlideGlobalD2D0FromStartToTarget(AkaoGlobalSlideRangeCommand *cmd) {
    int duration;
    int start = cmd->start;
    int step;

    if (start == 0) {
        duration = 1;
    } else {
        duration = cmd->duration;
    }

    start <<= 24;
    start >>= 8;
    D_8009D2D0 = start;
    step = ((cmd->target << 16) - start) / duration;
    g_AkaoGlobalD2D0SlideCounter = duration;
    g_AkaoGlobalD2D0SlideStep = step;
}

void Akao_SetGlobalD2CCImmediate(AkaoGlobalParamCommand *cmd) {
    int value = cmd->value;

    g_AkaoGlobalD2CCSlideCounter = 0;
    D_8009D2CC = value << 16;
}
