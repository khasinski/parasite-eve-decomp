/* CC1_PROFILE: build0001 */
/* MASPSX_FLAGS: --expand-div */
typedef signed char s8;

typedef struct {
    int unk0;
    int duration;
    s8 target;
} AkaoGlobalSlideCommand;

extern short g_AkaoGlobalD2D0SlideCounter;
extern int g_AkaoGlobalD2D0SlideStep;
extern int D_8009D2D0;

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
