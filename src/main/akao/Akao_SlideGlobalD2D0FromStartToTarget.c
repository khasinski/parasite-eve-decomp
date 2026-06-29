/* CC1_PROFILE: build0001 */
/* MASPSX_FLAGS: --expand-div */
typedef signed char s8;

typedef struct {
    int unk0;
    int duration;
    int start;
    s8 target;
} AkaoGlobalSlideRangeCommand;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int g_AkaoGlobalD2D0SlideStep;
extern int D_8009D2D0;

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
